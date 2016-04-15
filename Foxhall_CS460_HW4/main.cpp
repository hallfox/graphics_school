#include "Texture.hpp"
#include "GeoObject.hpp"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#include <iostream>
#include <assert.h>
#include <math.h>
#include <memory>
#include <fstream>
#include <vector>

namespace {
  // Up, right, down, left
  Triangle flower_texture[4];
  int flower_height, flower_width;
  double rotation;
  GeoObject teapot;
  int scene;
  double zoom;
  Vertex center;
}

std::ostream& operator<<(std::ostream& os, const std::tuple<double, double, double> bv) {
  return os << std::get<0>(bv) << "," << std::get<1>(bv) << "," << std::get<2>(bv);
}

void make_flower_texture() {
  auto flower_bmp = load_bmp("flower.bmp");
  flower_width = flower_bmp.width;
  flower_height = flower_bmp.height;
  auto nw = Vertex{0, 0};
  auto ne = Vertex{flower_bmp.width, 0};
  auto se = Vertex{flower_bmp.width, flower_bmp.height};
  auto sw = Vertex{0, flower_bmp.height};
  auto center = Vertex{flower_bmp.width/2, flower_bmp.height/2};
  flower_texture[0] = Triangle{nw, ne, center};
  flower_texture[1] = Triangle{ne, se, center};
  flower_texture[2] = Triangle{se, sw, center};
  flower_texture[3] = Triangle{sw, nw, center};

  for (int i = 0; i < flower_bmp.width; i++) {
    for (int j = 0; j < flower_bmp.height; j++) {
      auto v = Vertex(i, j);
      int offset = j*flower_bmp.width + i;
      auto r = flower_bmp.data[3*offset+2];
      auto g = flower_bmp.data[3*offset+1];
      auto b = flower_bmp.data[3*offset];
      for (int k = 0; k < 4; k++) {
        //std::cout << flower_texture[k] << " Vertex " << v << " Barycentric coords "
          //<< flower_texture[k].get_coords(v) << std::endl;
        if (flower_texture[k].contains_vert(v)) {
          flower_texture[k].insert(v, std::make_tuple(r, g, b));
          break;
        }
      }
    }
  }
}

void init() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  //glLineWidth(2.0);
  glShadeModel(GL_FLAT);
  make_flower_texture();
  teapot = GeoObject("teapot.obj");
  rotation = 0.0;
  scene = 0;
  zoom = 0;
}

void draw_flower() {
  glPushMatrix();
  glRotated(rotation, 0.0, 0.0, 1.0);
  glScalef(0.036, 0.04, 0.04);
  glTranslatef(-flower_width/2.0, -flower_height/2.0, 0.0);
  for (int i = 0; i < 4; i++) {
    flower_texture[i].draw();
  }
  glPopMatrix();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Set projection mode
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, 1, 1, 500000);

  // Set model mode
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // Camera adjustment
  gluLookAt(0, 0, 20, 0, 0, 0, 0, 1, 0);

  if (scene == 0) {
    draw_flower();
  } else if (scene == 1) {
    glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glRotated(center.first, 0.0, 1.0, 0.0);
    glRotated(center.second, 1.0, 0.0, 0.0);
    glTranslatef(0.0, -5.0, 0.0);
    glScalef(3.0+zoom, 3.0+zoom, 3.0+zoom);
    teapot.draw();
    glPopMatrix();
  }

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
    case 'r':
      rotation += 30.0;
      break;
    case 'R':
      rotation -= 30.0;
      break;
    case 'n':
      scene = (scene + 1) % 2;
      rotation = 0;
      center = Vertex(0, 0);
      break;
    case 'z':
      if (scene == 1) {
        zoom += 0.05;
      }
      break;
    case 'Z':
      if (scene == 1) {
        zoom -= 0.05;
      }
      break;
    default:
      break;
  }
}

inline void rotate(Vertex &v) {
  int new_x = cos(rotation)*v.first - sin(rotation)*v.second;
  int new_y = sin(rotation)*v.first + cos(rotation)*v.second;
  v = Vertex(new_x, new_y);
}

void mouse_motion_handler(int x, int y) {
  int win_w = glutGet(GLUT_WINDOW_WIDTH);
  int win_h = glutGet(GLUT_WINDOW_HEIGHT);
  if (scene == 0) {
    auto new_center = Vertex(x-win_w/2+flower_width/2, -y+win_h/2+flower_height/2);
    // rotate with flower
    rotate(new_center);
    for (int i = 0; i < 4; i++) {
      flower_texture[i].v3 = new_center;
    }
  } else if (scene == 1) {
    center = Vertex(x-win_w/2, -y+win_h/2);
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
  glutMotionFunc(mouse_motion_handler);

  //Enter the GLUT event loop
  glutMainLoop();

  return 0;
}

