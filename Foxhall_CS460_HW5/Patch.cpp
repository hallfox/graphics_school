#include "Patch.hpp"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#include <math.h>

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
  return binomial3(i) * pow(u, i) * pow(1-u, 3-i);
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
  glColor3f(0.7, 0.0, 1.0);
  for (double u = 0.0; u < 1.0; u += 0.05) {
    for (double v = 0.0; v < 1.0; v += 0.05) {
      auto p = bezier_param(u, v);
      glBegin(GL_POINTS);
      glVertex3f(p.x, p.y, p.z);
      glEnd();
    }
  }
}

Patch::CPoint& Patch::operator[](int i) {
  return coords_[i];
}
