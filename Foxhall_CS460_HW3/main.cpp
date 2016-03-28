#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#include <iostream>
#include <assert.h>
#include <math.h>

namespace {
  double camera_x = 0.0;
  double camera_y = 0.0;
  double camera_z = 40.0;
  double fairy_x = 0.0;
  double fairy_y = 0.0;
  double fairy_z = camera_z - 1;
  double up_x = 0.0;
  double up_y = 1.0;
  double up_z = 0.0;
  double lever_rot = 0.0;
  GLsizei width;
  GLsizei height;
  enum Scene { SCENE_CYLINDER, SCENE_LEVER };
  Scene current_scene = SCENE_CYLINDER;
}

void reset_camera() {
  camera_x = 0.0;
  camera_y = 0.0;
  camera_z = 40.0;
  fairy_x = 0.0;
  fairy_y = 0.0;
  fairy_z = camera_z - 1;
  up_x = 0.0;
  up_y = 1.0;
  up_z = 0.0;
  lever_rot = 0.0;
}

void next_scene() {
  reset_camera();
  if (current_scene == SCENE_CYLINDER) {
    current_scene = SCENE_LEVER;
  } else {
    current_scene = SCENE_CYLINDER;
  }
}

void init() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  //glLineWidth(2.0);
  glShadeModel(GL_FLAT);
  reset_camera();
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

void draw_cylinder() {
  glPushMatrix();
  //glRotatef(rx, 1, 0, 0);
  //glRotatef(ry, 0, 1, 0);
  //glRotatef(rz, 0, 0, 1);

  // Draw ground
  draw_ground();

  // Draw cylinder
  GLUquadricObj *quad = gluNewQuadric();
  gluQuadricDrawStyle(quad, GLU_LINE);
  glColor3f(1.0, 0.0, 0.0);
  glLineWidth(2.0);
  gluCylinder(quad, 5, 5, 30, 20, 10);
  gluDeleteQuadric(quad);

  glPopMatrix();
}

void draw_lever() {
  glPushMatrix();
  //glRotatef(rx, 1, 0, 0);
  //glRotatef(ry, 0, 1, 0);
  //glRotatef(rz, 0, 0, 1);

  // Draw ground
  draw_ground();

  GLUquadricObj *quad = gluNewQuadric();
  gluQuadricDrawStyle(quad, GLU_FILL);

  glPushMatrix();
  glRotated(lever_rot, 0, 1, 0); // rotation for b2
  // Position s1
  glRotated(90, 1, 0, 0);
  // Draw s1
  glColor3f(0.0, 0.0, 1.0);
  gluCylinder(quad, 1, 1, 20, 10, 10);
  // Position b2
  // Draw b2
  glColor3f(0.0, 1.0, 1.0);
  gluSphere(quad, 2, 10, 10);
  glPopMatrix();


  // left arm
  glPushMatrix();
  glRotated(-90+lever_rot, 0, 1, 0);
  glColor3f(0.0, 0.0, 1.0);
  gluCylinder(quad, 1, 1, 10, 10, 10); // s2
  glTranslated(0.0, 0.0, 10.0);
  glRotated(90, 0, 1, 0);
  glRotated(lever_rot+90, 1, 0, 0);
  glColor3f(0.0, 1.0, 1.0);
  gluSphere(quad, 1.5, 10, 10); // b1
  glColor3f(0.0, 0.0, 1.0);
  gluCylinder(quad, 0, 1, 10, 10, 10); // s2
  glTranslated(0, 0, 10);
  glColor3f(0.0, 1.0, 1.0);
  gluSphere(quad, 3, 10, 10); // b1
  glPopMatrix();

  // right arm
  glPushMatrix();
  glRotated(90+lever_rot, 0, 1, 0);
  glColor3f(0.0, 0.0, 1.0);
  gluCylinder(quad, 1, 1, 10, 10, 10); // s2
  glTranslated(0.0, 0.0, 10.0);
  glRotated(90, 0, 1, 0);
  glRotated(lever_rot+90, 1, 0, 0);
  glColor3f(0.0, 1.0, 1.0);
  gluSphere(quad, 1.5, 10, 10); // b1
  glColor3f(0.0, 0.0, 1.0);
  gluCylinder(quad, 0, 1, 10, 10, 10); // s2
  glTranslated(0, 0, 10);
  glColor3f(0.0, 1.0, 1.0);
  gluSphere(quad, 3, 10, 10); // b1
  glPopMatrix();

  gluDeleteQuadric(quad);
  glPopMatrix();
}

void draw_scene() {
  if (current_scene == SCENE_CYLINDER) {
    draw_cylinder();
  } else if (current_scene == SCENE_LEVER) {
    draw_lever();
  }
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
  gluLookAt(camera_x, camera_y, camera_z, fairy_x, fairy_y, fairy_z, up_x, up_y, up_z);
  draw_scene();

  draw_side_views();

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

  width = w;
  height = h;
}

void rotate(double& x, double& y, double theta) {
  double t = cos(theta)*x - sin(theta)*y;
  y = sin(theta)*x + cos(theta)*y;
  x = t;
}

void keyboard_handler(unsigned char key, int, int) {
  double delta_x;
  double delta_y;
  double delta_z;
  double mag;
  double look_angle;
  double rot_angle = 0.174533;
  switch (key) {
    case 27: // ESC
      exit(0);
      break;
    case 'w':
      delta_x = fairy_x - camera_x;
      delta_y = fairy_y - camera_y;
      delta_z = fairy_z - camera_z;
      mag = sqrt(delta_x*delta_x + delta_y*delta_y + delta_z*delta_z);
      fairy_x += 10*delta_x/mag;
      fairy_y += 10*delta_y/mag;
      fairy_z += 10*delta_z/mag;
      camera_x += 10*delta_x/mag;
      camera_y += 10*delta_y/mag;
      camera_z += 10*delta_z/mag;
      break;
    case 's':
      delta_x = fairy_x - camera_x;
      delta_y = fairy_y - camera_y;
      delta_z = fairy_z - camera_z;
      mag = sqrt(delta_x*delta_x + delta_y*delta_y + delta_z*delta_z);
      fairy_x += -10*delta_x/mag;
      fairy_y += -10*delta_y/mag;
      fairy_z += -10*delta_z/mag;
      camera_x += -10*delta_x/mag;
      camera_y += -10*delta_y/mag;
      camera_z += -10*delta_z/mag;
      break;
    case 'r':
      look_angle = atan(fairy_x/fairy_z);
      rotate(up_x, up_z, look_angle);
      rotate(up_x, up_y, rot_angle);
      rotate(up_x, up_z, -look_angle);
      break;
    case 'R':
      look_angle = atan(fairy_x/fairy_z);
      rotate(up_x, up_z, look_angle);
      rotate(up_x, up_y, -rot_angle);
      rotate(up_x, up_z, -look_angle);
      break;
    case 'n':
      next_scene();
      break;
    case 'g':
      lever_rot += 10;
      break;
    case 'G':
      lever_rot -= 10;
    default:
      break;
  }
}

void special_key_handler(int key, int, int) {
  double rot_angle = 0.174533;
  double look_angle;
  switch (key) {
    case GLUT_KEY_UP:
      fairy_y -= camera_y;
      fairy_z -= camera_z;
      look_angle = atan(fairy_x/fairy_z);
      rotate(fairy_x, fairy_z, look_angle);
      rotate(fairy_y, fairy_z, rot_angle);
      rotate(fairy_x, fairy_z, -look_angle);
      fairy_y += camera_y;
      fairy_z += camera_z;
      break;
    case GLUT_KEY_DOWN:
      fairy_y -= camera_y;
      fairy_z -= camera_z;
      look_angle = atan(fairy_x/fairy_z);
      rotate(fairy_x, fairy_z, look_angle);
      rotate(fairy_y, fairy_z, -rot_angle);
      rotate(fairy_x, fairy_z, -look_angle);
      fairy_y += camera_y;
      fairy_z += camera_z;
      break;
    case GLUT_KEY_RIGHT:
      fairy_x -= camera_x;
      fairy_z -= camera_z;
      rotate(fairy_x, fairy_z, rot_angle);
      fairy_y += camera_y;
      fairy_z += camera_z;
      break;
    case GLUT_KEY_LEFT:
      fairy_x -= camera_x;
      fairy_z -= camera_z;
      rotate(fairy_x, fairy_z, -rot_angle);
      fairy_y += camera_y;
      fairy_z += camera_z;
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
  glutKeyboardFunc(keyboard_handler);
  glutSpecialFunc(special_key_handler);

  //Enter the GLUT event loop
  glutMainLoop();

  return 0;
}

