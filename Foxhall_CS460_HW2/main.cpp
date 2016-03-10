#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#include <stdlib.h>
#include <iostream>
#include "draw.hpp"

struct Window {
  Window(int x, int y, int w, int h):
    x{x}, y{y}, w{w}, h{h} {}
  void draw() {
    glLineStipple(1, 0xF0F0);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINE_LOOP);
    glVertex2i(x, y);
    glVertex2i(x+w, y);
    glVertex2i(x+w, y+h);
    glVertex2i(x, y+h);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
  }
  inline bool contains(const Point2d& p) {
    return x <= p.first && p.first <= x+w && y <= p.second && p.second <= y+h;
  }
  int x, y, w, h;
};

namespace {
  Painter painter, viewport_painter;
  Window clip_window{150, 150, 250, 250};
  Window viewport{500, 300, 100, 100};
}

void init() {
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glLineWidth(2.0);
  glShadeModel(GL_FLAT);
  painter = Painter{};
  viewport_painter = Painter{};
}

void map_viewport() {
  // Draw painter's drawings that lie in the clipping window
  viewport_painter.delete_drawings();
  for (auto& pic: painter.get_drawings()) {
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
  painter.paint();
  viewport_painter.paint();

  glutSwapBuffers();
}

void mouse_handler(int button, int state, int x, int y) {
  // NOTE x and y are relative to the top left of the window,
  // they will be transformed before they go into the user list
  int height = glutGet(GLUT_WINDOW_HEIGHT);
  auto pt = std::make_pair(x, height - y);
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
}

void mouse_motion_handler(int x, int y) {
  int h = glutGet(GLUT_WINDOW_HEIGHT);
  painter.move_brush(std::make_pair(x, h - y));
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
    if (!painter.is_painting()) {
      std::list<Point2d> bounds{
        std::make_pair(clip_window.x, clip_window.y),
          std::make_pair(clip_window.x+clip_window.w, clip_window.y),
          std::make_pair(clip_window.x+clip_window.w, clip_window.y+clip_window.h),
          std::make_pair(clip_window.x, clip_window.y+clip_window.h)
          };
      painter.clip(bounds);
    }
    break;
  case 'f':
    if (!painter.is_painting()) {
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

