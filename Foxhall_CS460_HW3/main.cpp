#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#include <iostream>
#include <assert.h>

namespace {
  float camera_z = 40.0;
  float theta_x = 0.0;
  float theta_y = 0.0;
  float theta_z = 0.0;
  GLsizei width;
  GLsizei height;
}

void init() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  //glLineWidth(2.0);
  glShadeModel(GL_FLAT);
}

void draw_ground() {
  glLineWidth(1);
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_LINES);
  for(float i =-1; i<=1; i += 0.1) {
    glVertex3f(i*100.f, -10.f, -100.f);
    glVertex3f(i*100.f, -10.f, 100.f);
  }
  for(float j =-1; j<=1; j += 0.1) {
    glVertex3f(-100.f, -10.f, j*100.f);
    glVertex3f(100.f, -10.f, j*100.f);
  }
  glEnd();
}

void draw_scene() {
  glPushMatrix();
  glRotatef(theta_x, 1, 0, 0);
  glRotatef(theta_y, 0, 1, 0);
  glRotatef(theta_z, 0, 0, 1);

  // Draw cylinder
  GLUquadricObj *quad = gluNewQuadric();
  gluQuadricDrawStyle(quad, GLU_LINE);
  glColor3f(1.0, 0.0, 0.0);
  glLineWidth(2.0);
  gluCylinder(quad, 5, 5, 30, 20, 10);
  gluDeleteQuadric(quad);

  // Draw ground
  draw_ground();
  glPopMatrix();
}

void draw_perpective(float x, float y, float z, float u, float v, float n) {
  float ratio = static_cast<float>(width) / height;

  // Set projection mode
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, ratio, 1, 256);

  // Set model mode
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // Camera adjustment
  gluLookAt(x, y, z, 0.0, 0.0, 0.0, u, v, n);
  draw_scene();
}

void draw_side_views() {
  // V2
  glViewport(0, 0, width/2.0, height/2.0);
  draw_perpective(0.0, 40.0, 0.0, 1.0, 0.0, 0.0);
  // V3
  glViewport(0, height/2.0, width/2.0, height/2.0);
  draw_perpective(40.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  // V4
  glViewport(width/2.0, height/2.0, width/2.0, height/2.0);
  draw_perpective(0.0, 0.0, 40.0, 0.0, 1.0, 0.0);
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glColor3f(1.0, 1.0, 1.0);
  glLineWidth(1);

  // Set viewport
  glViewport(width/2.0, 0, width/2.0, height/2.0);

  // Set projection mode
  float ratio = static_cast<float>(width) / height;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, ratio, 1, 256);

  // Set model mode
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // Camera adjustment
  gluLookAt(0.0, 0.0, camera_z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  draw_scene();

  draw_side_views();

  glFlush();

  glutSwapBuffers();
}


void mouse_handler(int , int , int , int ) {
}

void mouse_motion_handler(int , int ) {
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
  glOrtho(0, w, h, 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  width = w;
  height = h;
}

void keyboard_handler(unsigned char key, int, int) {
  switch (key) {
    case 27: // ESC
      exit(0);
      break;
    case 'z':
      camera_z += 10;
      break;
    case 'Z':
      camera_z -= 10;
      break;
    case 'r':
      theta_z += 10;
      break;
    case 'R':
      theta_z -= 10;
      break;
    default:
      break;
  }
}

void special_key_handler(int key, int, int) {
  switch (key) {
    case GLUT_KEY_UP:
      theta_x += 10;
      if (theta_x > 360) theta_x = -360;
      break;
    case GLUT_KEY_DOWN:
      theta_x -= 10;
      if (theta_x < -360) theta_x = 360;
      break;
    case GLUT_KEY_RIGHT:
      theta_y -= 10;
      if (theta_y < -360) theta_y = 360;
      break;
    case GLUT_KEY_LEFT:
      theta_y += 10;
      if (theta_y > 360) theta_y = -360;
      break;
    default:
      break;
  }

  glutPostRedisplay();
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
  glutSpecialFunc(special_key_handler);

  //Enter the GLUT event loop
  glutMainLoop();

  return 0;
}

