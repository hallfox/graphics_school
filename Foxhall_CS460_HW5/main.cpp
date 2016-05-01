#include "Patch.hpp"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#include <iostream>
#include <assert.h>
#include <math.h>

namespace {
  Patch bezier;
  int position;
  int dim;
}

void init() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  //glLineWidth(2.0);
  glShadeModel(GL_FLAT);
  bezier = Patch{};
  position = 45;
  dim = 0;
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Set projection mode
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, 1, 1, 500);

  // Set model mode
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // Camera adjustment
  gluLookAt(45, 45, position, 0, 0, 0, 0, 1, 0);

  glBegin(GL_LINES);
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(100.0, 0.0, 0.0);
  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 100.0, 0.0);
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 100.0);
  glEnd();
  bezier.draw();

  glFlush();
  glutSwapBuffers();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, h, 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyboard_handler(unsigned char key, int, int) {
  switch (key) {
    case 27: // ESC
      exit(0);
      break;
    case 'z':
      position -= 10;
      break;
    case 'x':
      position += 10;
      break;
    case 'q':
      bezier[5][dim] += 5.0;
      break;
    case 'a':
      bezier[5][dim] -= 5.0;
      break;
    case 'w':
      bezier[6][dim] += 5.0;
      break;
    case 's':
      bezier[6][dim] -= 5.0;
      break;
    case 'e':
      bezier[9][dim] += 5.0;
      break;
    case 'd':
      bezier[9][dim] -= 5.0;
      break;
    case 'r':
      bezier[10][dim] += 5.0;
      break;
    case 'f':
      bezier[10][dim] -= 5.0;
      break;
    case ' ':
      dim = (dim + 1) % 3;
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
  glutKeyboardFunc(keyboard_handler);

  //Enter the GLUT event loop
  glutMainLoop();

  return 0;
}

