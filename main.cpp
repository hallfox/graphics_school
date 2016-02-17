#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#include <stdlib.h>
#include <iostream>

void init() {
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

void opengl_wake() {
  glColor3f(0.0, 0.0, 0.0);

  // W
  glBegin(GL_LINE_STRIP);
    glVertex2f(0.54, 0.75);
    glVertex2f(0.58, 0.25);
    glVertex2f(0.62, 0.55);
    glVertex2f(0.66, 0.25);
    glVertex2f(0.70, 0.75);
  glEnd();

  // A
  glBegin(GL_LINE_STRIP);
    glVertex2f(0.70, 0.25);
    glVertex2f(0.74, 0.75);
    glVertex2f(0.78, 0.25);
  glEnd();
  glBegin(GL_LINES);
    glVertex2f(0.72, 0.50);
    glVertex2f(0.76, 0.50);
  glEnd();

  // K
  glBegin(GL_LINES);
    glVertex2f(0.80, 0.75);
    glVertex2f(0.80, 0.25);
  glEnd();
  glBegin(GL_LINE_STRIP);
    glVertex2f(0.88, 0.75);
    glVertex2f(0.80, 0.50);
    glVertex2f(0.88, 0.25);
  glEnd();

  // E
  glBegin(GL_LINE_STRIP);
    glVertex2f(0.96, 0.75);
    glVertex2f(0.90, 0.75);
    glVertex2f(0.90, 0.25);
    glVertex2f(0.96, 0.25);
  glEnd();
  glBegin(GL_LINES);
    glVertex2f(0.90, 0.50);
    glVertex2f(0.96, 0.50);
  glEnd();
}

void brensenham_line(float x0, float y0, float x1, float y1) {
  // TODO
  // plotLine(x0,y0, x1,y1)
  //   dx = x1 - x0
  //   dy = y1 - y0

  //   D = 2*dy - dx
  //   plot(x0,y0)
  //   y = y0
  //   if D > 0
  //            y = y+1
  //            D = D - (2*dx)
  //            for x from x0+1 to x1
  //                    plot(x,y)
  //                    D = D + (2*dy)
  //                    if D > 0
  //                             y = y+1
  //                             D = D - (2*dx)
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

void display() {
  //Clear all pixels
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw the versions of wake
  // brensenham_wake();
  opengl_wake();

  glFlush();
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
  // glutReshapeFunc(reshape);

  //Enter the GLUT event loop
  glutMainLoop();

  return 0;
}

