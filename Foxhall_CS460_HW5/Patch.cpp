#include "Patch.hpp"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#include <math.h>
#include <iostream>

double& Patch::CPoint::operator[](int i) {
  switch (i) {
    case 0:
      return x;
    case 1:
      return y;
    case 2:
      return z;
    default:
      throw std::out_of_range("Not a valid point");
  }
}

Patch::Patch() {
  coords_ = std::vector<CPoint>{16};
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      coords_[j+4*i] = CPoint{i*20.0, 0.0, j*20.0};
    }
  }
}

int binomial3(int k) {
  int vals[] = {1, 3, 3, 1};
  return vals[k];
}

double Patch::bernstein4(int i, double u) {
  return binomial3(i) * pow(u/20.0, i) * pow(1-u/20.0, 3-i);
}

inline Patch::CPoint Patch::bezier_param(double u, double v) {
  Patch::CPoint p;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      auto b = bernstein4(i, u)*bernstein4(j, v);
      p.x += b*coords_[j+4*i].x;
      p.y += b*coords_[j+4*i].y;
      p.z += b*coords_[j+4*i].z;
    }
  }
  return p;
}

void Patch::draw() {
  glPushMatrix();
  glTranslatef(-30, 0, -30);

  // Control points
  glColor3f(1.0, 1.0, 1.0);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      glPushMatrix();
      Patch::CPoint cp = coords_[j+4*i];
      glTranslatef(cp.x, cp.y, cp.z);
      glutSolidSphere(1, 10, 10);
      glPopMatrix();
    }
  }

  // Bezier surface points
  std::vector<Patch::CPoint> pts;
  for (int u = 0; u < 20; u++) {
    for (int v = 0; v < 20; v++) {
      pts.push_back(bezier_param(u, v));
    }
  }
  std::cerr << pts.size() << std::endl;

  // Draw surfaces
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  for (int i = 0; i < 19; i++) {
    for (int j = 0; j < 19; j++) {
      auto p1 = pts[j + 20*i];
      auto p2 = pts[j+1 + 20*i];
      auto p3 = pts[j + 20*(i+1)];
      auto p4 = pts[j+1 + 20*(i+1)];
      glColor3f(0.7, 0.0, 1.0);
      glBegin(GL_TRIANGLE_STRIP);
      glVertex3f(p1.x, p1.y, p1.z);
      glVertex3f(p2.x, p2.y, p2.z);
      glVertex3f(p4.x, p4.y, p4.z);
      glVertex3f(p3.x, p3.y, p3.z);
      glEnd();
    }
  }
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glColor3f(0.0, 0.5, 1.0);
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 20; j++) {
      auto p1 = pts[j + 20*i];
      glPushMatrix();
      glTranslatef(p1.x, p1.y, p1.z);
      glutSolidSphere(0.2, 10, 10);
      glPopMatrix();
    }
  }

  glPopMatrix();
}

Patch::CPoint& Patch::operator[](int i) {
  return coords_[i];
}
