#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#include <stdlib.h>
#include <iostream>
#include "draw.hpp"

namespace {
  Painter painter;
  std::list<Point2d> clip_window {
    std::make_pair(150, 150),
      std::make_pair(400, 150),
      std::make_pair(400, 400),
      std::make_pair(150, 400)
      };
}

void init() {
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glLineWidth(2.0);
  glShadeModel(GL_FLAT);
  painter = Painter();
}

void draw_clipping_window() {
  glLineStipple(1, 0xF0F0);
  glEnable(GL_LINE_STIPPLE);
  glBegin(GL_LINE_LOOP);
  for (auto& pt: clip_window) {
    glVertex2i(pt.first, pt.second);
  }
  glEnd();
  glDisable(GL_LINE_STIPPLE);
}

void display() {
  //Clear all pixels
  glClear(GL_COLOR_BUFFER_BIT);

  glColor3f(0.0, 0.0, 0.0);
  draw_clipping_window();
  painter.paint();

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
      painter.clip(clip_window);
    }
    break;
  case 'f':
    if (!painter.is_painting()) {
      painter.fill();
    }
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

