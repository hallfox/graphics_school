#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#include <stdlib.h>
#include <iostream>
#include "draw.h"

#define RENDER_COUNT 3
enum Renderer { RENDER_OPENGL = 0, RENDER_BRENSENHAM, RENDER_MIDPOINT };

// TODO make all the states for the mode
namespace {
  Renderer renderer = RENDER_OPENGL;
  Painter painter;
  bool stipple_enabled = false;
  bool mimic_enabled = false;
}

void log_renderer() {
  std::cout << "Using renderer ";
  switch (renderer) {
  case RENDER_OPENGL:
    std::cout << "OpenGL";
    break;
  case RENDER_BRENSENHAM:
    std::cout << "Brensenham";
    break;
  case RENDER_MIDPOINT:
    std::cout << "Midpoint";
    break;
  default:
    std::cout << "Unknown";
  }
  std::cout << std::endl;
}

void init() {
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glShadeModel(GL_FLAT);
  glLineWidth(2);
  // glMatrixMode(GL_PROJECTION);
  // glLoadIdentity();
  // int w = glutGet(GLUT_WINDOW_WIDTH);
  // int h = glutGet(GLUT_WINDOW_HEIGHT);
  // glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
  painter = Painter();
  renderer = RENDER_OPENGL;
  log_renderer();
}

Point2d bren_switch_input(int octant, int x, int y) {
  switch (octant) {
  case 0:
    return std::make_pair(x, y);
  case 1:
    return std::make_pair(y, x);
  case 2:
    return std::make_pair(y, -x);
  case 3:
    return std::make_pair(-x, y);
  case 4:
    return std::make_pair(-x, -y);
  case 5:
    return std::make_pair(-y, -x);
  case 6:
    return std::make_pair(-y, x);
  case 7:
    return std::make_pair(x, -y);
  }
  return std::make_pair(x, y);
}

Point2d bren_switch_output(int octant, int x, int y) {
  switch (octant) {
  case 0:
    return std::make_pair(x, y);
  case 1:
    return std::make_pair(y, x);
  case 2:
    return std::make_pair(-y, x);
  case 3:
    return std::make_pair(-x, y);
  case 4:
    return std::make_pair(-x, -y);
  case 5:
    return std::make_pair(-y, -x);
  case 6:
    return std::make_pair(y, -x);
  case 7:
    return std::make_pair(x, -y);
  }
  return std::make_pair(x, y);
}

int get_octant(int w, int h) {
  // Determine Octant
  int octant = 0;
  if (w > 0) {
    // Either 0, 1, 6, 7
    if (h > 0) { // Positive slope
      // Either 0, 1
      if (abs(w) >= abs(h)) { // flat
        octant = 0;
      }
      else { // steep
        octant = 1;
      }
    }
    else { // Negative slope
      // Either 6, 7
      if (abs(w) >= abs(h)) {
        octant = 7;
      }
      else {
        octant = 6;
      }
    }
  }
  else {
    // Either 2, 3, 4, 5, 6
    if (h > 0) {
      // Either 2, 3
      if (abs(w) >= abs(h)) {
        octant = 3;
      }
      else {
        octant = 2;
      }
    }
    else {
      // Either 4, 5
      if (abs(w) >= abs(h)) {
        octant = 4;
      }
      else {
        octant = 5;
      }
    }
  }
  return octant;
}

void brensenham_line(int x0, int y0, int x1, int y1) {
  int octant = get_octant(x1-x0, y1-y0);

  Point2d t = bren_switch_input(octant, x0, y0);
  x0 = t.first;
  y0 = t.second;

  t = bren_switch_input(octant, x1, y1);
  x1 = t.first;
  y1 = t.second;

  int dx = x1 - x0;
  int dy = y1 - y0;
  int y = y0;
  int decider = 2*dy - dx;

  glBegin(GL_POINTS);
  for (int x = x0; x <= x1; x++) {
    t = bren_switch_output(octant, x, y);
    glVertex2i(t.first, t.second);
    decider += 2*dy;
    if (decider > 0) {
      y++;
      decider -= 2*dx;
    }
  }
  glEnd();
}

void midpoint_line(int x0, int y0, int x1, int y1) {
  int octant = get_octant(x1-x0, y1-y0);

  Point2d t = bren_switch_input(octant, x0, y0);
  x0 = t.first;
  y0 = t.second;

  t = bren_switch_input(octant, x1, y1);
  x1 = t.first;
  y1 = t.second;

  int dx = x1 - x0;
  int dy = y1 - y0;
  int y = y0;
  int decider = 2*dy - dx;
  int inc_e = 2*dy;
  int inc_ne = 2*(dy - dx);

  glBegin(GL_POINTS);
  for (int x = x0; x <= x1; x++) {
    t = bren_switch_output(octant, x, y);
    glVertex2i(t.first, t.second);
    if (decider > 0) {
      decider += inc_ne;
      y++;
    }
    else {
      decider += inc_e;
    }
  }
  glEnd();
}

void draw_user_points() {
  auto drawings = painter.get_drawings();
  switch (renderer) {
  case RENDER_OPENGL:
    for (auto& drawing: drawings) {
      glBegin(GL_LINE_STRIP);
      if (drawing.get_points().size() > 1) {
        for (const auto& p: drawing.get_points()) {
          glVertex2i(p.first, p.second);
        }
      }
      glEnd();
    }
    break;
  case RENDER_BRENSENHAM:
  case RENDER_MIDPOINT:
    for (auto& drawing: drawings) {
      if (drawing.get_points().size() > 1) {
        int x_prev, y_prev;
        bool first = true;
        for (const auto& p: drawing.get_points()) {
          if (first) {
            x_prev = p.first;
            y_prev = p.second;
            first = false;
          }
          else {
            if (renderer == RENDER_BRENSENHAM) {
              brensenham_line(x_prev, y_prev, p.first, p.second);
            }
            else {
              midpoint_line(x_prev, y_prev, p.first, p.second);
            }
            x_prev = p.first;
            y_prev = p.second;
          }
        }
      }
    }
    break;
  default:
    break;
  }

  // Translate the user's drawings and draw with OpenGL if the mimic is turned on
  if (mimic_enabled) {
    int width = glutGet(GLUT_WINDOW_WIDTH);
    for (auto& drawing: drawings) {
      glBegin(GL_LINE_STRIP);
      if (drawing.get_points().size() > 1) {
        for (const auto& p: drawing.get_points()) {
          glVertex2i(p.first + width/2, p.second);
        }
      }
      glEnd();
    }
  }
}

void draw_stalker_line() {
  if (!painter.is_drawing()) {
    return;
  }
  auto last_pt = painter.get_current_drawing().get_points().back();
  auto brush = painter.get_brush();

  switch (renderer) {
  case RENDER_OPENGL:
    glBegin(GL_LINES);
    glVertex2i(last_pt.first, last_pt.second);
    glVertex2i(brush.first, brush.second);
    glEnd();
    break;
  case RENDER_BRENSENHAM:
    brensenham_line(last_pt.first, last_pt.second, brush.first, brush.second);
    break;
  case RENDER_MIDPOINT:
    midpoint_line(last_pt.first, last_pt.second, brush.first, brush.second);
    break;
  default:
    break;
  }
}

void display() {
  //Clear all pixels
  glClear(GL_COLOR_BUFFER_BIT);

  glColor3f(0.0, 0.0, 0.0);
  draw_user_points();
  draw_stalker_line();

  glutSwapBuffers();
}

void mouse_handler(int button, int state, int x, int y) {
  // NOTE x and y are relative to the top left of the window,
  // they will be transformed before they go into the user list
  int height = glutGet(GLUT_WINDOW_HEIGHT);
  auto pt = std::make_pair(x, height - y);
  if (!painter.is_drawing()) {
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
      painter.get_current_drawing().add_point(pt);
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
      std::cout << "Drawing ended.\n";
      painter.stop_drawing();
    }
  }
}

void mouse_motion_handler(int x, int y) {
  int h = glutGet(GLUT_WINDOW_HEIGHT);
  painter.set_brush(std::make_pair(x, h - y));
  glutPostRedisplay();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
}

void toggle_mimic() {
  if (mimic_enabled) {
    std::cout << "Mimic enabled\n";
  }
  else {
    std::cout << "Mimic disabled\n";
  }
  mimic_enabled = !mimic_enabled;
}

void toggle_stipple() {
  if (stipple_enabled) {
    std::cout << "Line stippling enabled\n";
    stipple_enabled = false;
    glDisable(GL_LINE_STIPPLE);
  }
  else {
    std::cout << "Line stippling disabled\n";
    stipple_enabled = true;
    glLineStipple(1, 0x0101);
    glEnable(GL_LINE_STIPPLE);
  }
}

void toggle_renderer() {
  switch (renderer) {
  case RENDER_OPENGL:
    renderer = RENDER_BRENSENHAM;
    break;
  case RENDER_BRENSENHAM:
    renderer = RENDER_MIDPOINT;
    break;
  case RENDER_MIDPOINT:
    // TODO add support for MIDPOINT
    renderer = RENDER_OPENGL;
    break;
  }

  log_renderer();
}

void keyboard_handler(unsigned char key, int x, int y) {
  switch (key) {
  case 27: // ESC
    exit(0);
    break;
  case 'm':
    toggle_mimic();
    break;
  case 's':
    toggle_stipple();
    break;
  case 'r':
    toggle_renderer();
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

