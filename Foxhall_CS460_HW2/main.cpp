#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include "draw.hpp"

#include <stdlib.h>
#include <iostream>
#include <assert.h>

struct Window {
  Window(int x, int y, int w, int h):
    x{x}, y{y}, w{w}, h{h}, is_resizing{false}, is_dragging{false} {}
  void draw() {
    glLineStipple(1, 0xF0F0);
    glEnable(GL_LINE_STIPPLE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glRecti(x, y, x+w, y+h);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glRecti(x-10, y-10, x, y);
    glDisable(GL_LINE_STIPPLE);
  }
  inline bool in_zoom_box(const Point2d& p) {
    return x-10 <= p.first && p.first <= x && y-10 <= p.second && p.second <= y;
  }
  inline bool contains(const Point2d& p) {
    return x <= p.first && p.first <= x+w && y <= p.second && p.second <= y+h;
  }
  int x, y, w, h;
  bool is_resizing;
  bool is_dragging;
};

namespace {
  Painter painter, viewport_painter;
  Window clip_window{150, 150, 250, 250};
  Window viewport{500, 300, 100, 100};
  bool clip_enabled;
}

void init() {
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glLineWidth(2.0);
  glShadeModel(GL_FLAT);
  painter = Painter{};
  viewport_painter = Painter{};
  clip_enabled = false;
}

inline bool inside_boundary(Point2d a, Point2d b, Point2d x) {
  return (b.first - a.first)*(x.second - a.second) - (x.first - a.first)*(b.second - a.second) >= 0;
}

Point2d intersect(const std::pair<Point2d, Point2d>& v1, const std::pair<Point2d, Point2d>& v2) {
  int a1 = (v1.first.first*v1.second.second - v1.first.second*v1.second.first)*(v2.first.first - v2.second.first) -
    (v1.first.first - v1.second.first)*(v2.first.first*v2.second.second - v2.first.second*v2.second.first);

  int a2 = (v1.first.first*v1.second.second - v1.first.second*v1.second.first)*(v2.first.second - v2.second.second) -
    (v1.first.second - v1.second.second)*(v2.first.first*v2.second.second - v2.first.second*v2.second.first);

  int b = (v1.first.first - v1.second.first)*(v2.first.second - v2.second.second) -
    (v1.first.second - v1.second.second)*(v2.first.first - v2.second.first);

  assert(b != 0);
  return std::make_pair(a1/b, a2/b);
}

void clip(const std::list<Point2d>& clip_window) {
  std::list<Point2d> output_verts;

  for (auto& pic: painter.get_drawings()) {
    output_verts = pic->get_points();

    for (auto it = clip_window.cbegin(); it != clip_window.cend();) {
      std::pair<Point2d, Point2d> clip_edge;
      if (it == --clip_window.cend()) {
        clip_edge = std::make_pair(*it, *clip_window.cbegin());
        it++;
      } else {
        clip_edge = std::make_pair(*it, *(++it));
      }
      auto input_verts = output_verts;
      output_verts.clear();
      auto last = input_verts.back();
      // Clipping assumes vertices are drawn counterclockwise
      for (auto& p: input_verts) {
        if (inside_boundary(clip_edge.first, clip_edge.second, p)) {
          if (!inside_boundary(clip_edge.first, clip_edge.second, last)) {
            // outside -> inside
            output_verts.push_back(intersect(std::make_pair(p, last), clip_edge));
          }
          // inside -> inside or outside -> inside
          output_verts.push_back(p);
        } else if (inside_boundary(clip_edge.first, clip_edge.second, last)) {
          // inside -> outside
          output_verts.push_back(intersect(std::make_pair(p,last), clip_edge));
        }
        last = p;
      }
    }
    viewport_painter.add_drawing(output_verts);
  }
}

void map_viewport() {
  // Draw painter's drawings that lie in the clipping window
  viewport_painter.delete_drawings();
  std::list<Point2d> bounds{
    std::make_pair(clip_window.x, clip_window.y),
      std::make_pair(clip_window.x+clip_window.w, clip_window.y),
      std::make_pair(clip_window.x+clip_window.w, clip_window.y+clip_window.h),
      std::make_pair(clip_window.x, clip_window.y+clip_window.h)
      };
  clip(bounds);
  for (auto& pic: viewport_painter.get_drawings()) {
    if (dynamic_cast<BlobDrawing *>(pic.get())) {
      continue;
    }

    for (auto& pt: pic->get_points()) {
      if (clip_window.contains(pt)) {
        Point2d t1 = std::make_pair(pt.first-clip_window.x, pt.second-clip_window.y);
        Point2d t2 = std::make_pair(static_cast<int>(t1.first * (static_cast<double>(viewport.w)/clip_window.w)),
                                    static_cast<int>(t1.second * (static_cast<double>(viewport.h)/clip_window.h)));
        Point2d t3 = std::make_pair(t2.first+viewport.x, t2.second+viewport.y);

        if (!viewport_painter.is_painting()) {
          viewport_painter.start_drawing(t3);
        } else {
          viewport_painter.add_point(t3);
        }
      }
    }
    if (viewport_painter.is_painting()) {
      viewport_painter.stop_drawing();
    }
  }

}

void display() {
  //Clear all pixels
  glClear(GL_COLOR_BUFFER_BIT);

  glColor3f(0.0, 0.0, 0.0);
  clip_window.draw();
  viewport.draw();
  if (!clip_enabled) {
    painter.paint();
  } else {
    viewport_painter.paint();
  }

  glutSwapBuffers();
}

void mouse_handler(int button, int state, int x, int y) {
  int height = glutGet(GLUT_WINDOW_HEIGHT);
  auto pt = Point2d(x, height - y);
  if (!clip_enabled) {
    // NOTE x and y are relative to the top left of the window,
    // they will be transformed before they go into the user list
    if (!painter.is_painting()) {
      // Go into drawing mode
      if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        std::cout << "Starting drawing at: " << pt.first << ", " << pt.second << std::endl;
        painter.start_drawing(pt);
      }
    }
    else {
      // Things you can do in drawing mode:
      // - left click to add new point
      // - right click to stop drawing
      if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        std::cout << "Adding new point: " << pt.first << ", " << pt.second << std::endl;
        painter.add_point(pt);
      }
      else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
        std::cout << "Drawing ended.\n";
        painter.stop_drawing();
      }
    }
  } else {
    // If left mouse down
    //   If mouse in viewport zoom window
    //     If in left mouse down
    //       Draw rectangle to mouse
    //     Else if left mouse up
    //       Change viewport size to mouse location
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
      if (viewport.in_zoom_box(pt)) {
        viewport.is_resizing = true;
      } else if (clip_window.in_zoom_box(pt)) {
        clip_window.is_resizing = true;
      } else if (clip_window.contains(pt)) {
        clip_window.is_dragging = true;
      }
    } else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
      // WHO EVER SAID REPEAT CODE IS BAD WHEN YOU HAVE A DEADLINE
      if (viewport.is_resizing) {
        viewport.is_resizing = false;
        viewport.w = std::abs(x-viewport.x-viewport.w);
        viewport.h = std::abs(height-y - viewport.y-viewport.h);
        viewport.x = x;
        viewport.y = height-y;
        map_viewport();
      } else if (clip_window.is_resizing) {
        clip_window.is_resizing = false;
        clip_window.w = std::abs(x-clip_window.x-clip_window.w);
        clip_window.h = std::abs(height-y - clip_window.y-clip_window.h);
        clip_window.x = x;
        clip_window.y = height-y;
        map_viewport();
      } else if (clip_window.is_dragging) {
        // LOL DOESN'T WORK QUITE RIGHT WHATEVS
        clip_window.is_dragging = false;
        clip_window.x += x - clip_window.x;
        clip_window.y += (height-y) - clip_window.y;
        map_viewport();
      }
    }
  }
}

void mouse_motion_handler(int x, int y) {
  int h = glutGet(GLUT_WINDOW_HEIGHT);
  painter.move_brush(std::make_pair(x, h - y));

  if (viewport.is_resizing) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(0.0, 0.0, 1.0);
    glRecti(x, h-y, viewport.x, viewport.y);
    glColor3f(0.0, 0.0, 0.0);
  } else if (clip_window.is_dragging) {
    clip_window.x += x - clip_window.x;
    clip_window.y += (h-y) - clip_window.y;
    map_viewport();
  }
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
}

void keyboard_handler(unsigned char key, int x, int y) {
  switch (key) {
  case 27: // ESC
    exit(0);
    break;
  case 'u':
    // Move this to painter
    if (!painter.is_painting()) {
      painter.undo();
    }
    break;
  case 'c':
    if (clip_enabled) {
      clip_enabled = false;
    } else if (!painter.is_painting()) {
      clip_enabled = true;
    }
    map_viewport();
    break;
  case 'f':
    if (!clip_enabled && !painter.is_painting()) {
      painter.fill();
    }
    break;
  case 'v':
    map_viewport();
    break;
  default:
    break;
  }
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);
  //Set Display Mode
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  //Set the window size
  glutInitWindowSize(640, 480);
  //Set the window position
  glutInitWindowPosition(100, 100);
  //Create the window
  glutCreateWindow("CS 460");
  init();

  glutDisplayFunc(display);
  glutIdleFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse_handler);
  glutMotionFunc(mouse_motion_handler);
  glutPassiveMotionFunc(mouse_motion_handler);
  glutKeyboardFunc(keyboard_handler);

  //Enter the GLUT event loop
  glutMainLoop();

  return 0;
}

