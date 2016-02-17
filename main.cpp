#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#include <stdlib.h>
#include <iostream>
#include "draw.h"

enum Renderer { RENDER_OPENGL, RENDER_BRENSENHAM, RENDER_MIDPOINT };

// TODO make all the states for the mode
static Renderer renderer = RENDER_OPENGL;
static Painter painter;

void init() {
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
  painter = Painter();
  renderer = RENDER_OPENGL;
  std::cout << "Using renderer " << renderer << std::endl;
}

void brensenham_line(int x0, int y0, int x1, int y1) {
  int dx = x1 - x0;
  int dy = y1 - y0;
  int y = y0;
  int decider = 2*dy - dx;

  glBegin(GL_POINTS);
  glVertex2f(x0, y0);
  if (decider > 0) {
    y++;
    decider -= 2*dx;
  }
  for (int x = x0+1; x < x1; x++) {
    glVertex2f(x, y);
    decider += 2*dy;
    if (decider > 0) {
      y++;
      decider -= 2*dx;
    }
  }
  glEnd();
}

void draw_user_points() {
  switch (renderer) {
  case RENDER_OPENGL:
    for (const auto &drawing: painter.get_drawings()) {
      glBegin(GL_LINE_STRIP);
      for (const auto &p: drawing->get_points()) {
        glVertex2i(p.first, p.second);
      }
      glEnd();
    }
    break;
  default:
    break;
  }
}

void display() {
  //Clear all pixels
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw the versions of wake
  // brensenham_wake();
  // opengl_wake();
  draw_user_points();

  glFlush();
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
    // TODO
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

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);
  //Set Display Mode
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  //Set the window size
  glutInitWindowSize(640, 480);
  //Set the window position
  glutInitWindowPosition(100, 100);
  //Create the window
  glutCreateWindow("CS 460");
  init();

  //Call "display" function
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse_handler);

  //Enter the GLUT event loop
  glutMainLoop();

  return 0;
}

