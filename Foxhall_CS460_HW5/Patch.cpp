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

Patch::CPoint Patch::CPoint::operator-(const Patch::CPoint& o) {
  return Patch::CPoint(x-o.x, y-o.y, z-o.z);
}

Patch::CPoint Patch::CPoint::unit() {
  double len = sqrt(x*x + y*y + z*z);
  return Patch::CPoint(x/len, y/len, z/len);
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

inline Patch::CPoint normal(Patch::CPoint p1, Patch::CPoint p2, Patch::CPoint p3) {
  auto u = p2 - p1;
  auto v = p3 - p1;
  auto n = Patch::CPoint(u.y*v.z-u.z*v.y, u.z*v.x-u.x*v.z, u.x*v.y-u.y*v.x);
  return n.unit();
}

bool is_bfc(Patch::CPoint p1, Patch::CPoint p2, Patch::CPoint p3, double vx, double vy, double vz) {
  auto n = normal(p1, p2, p3);
  return n.x*vx + n.y*vy + n.z*vz >= 0;
}

void Patch::draw(double vx, double vy, double vz) {
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
  // std::cerr << pts.size() << std::endl;


  // Draw surfaces
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  for (int i = 0; i < 19; i++) {
    for (int j = 0; j < 19; j++) {
      auto p1 = pts[j + 20*i];
      auto p2 = pts[j+1 + 20*i];
      auto p3 = pts[j + 20*(i+1)];
      auto p4 = pts[j+1 + 20*(i+1)];
      glColor3f(0.0, 1.0, 0.0);
      glBegin(GL_TRIANGLE_STRIP);
      // Face one
      if (is_bfc(p1, p2, p3, vx, vy, vz)) {
        glVertex3f(p1.x, p1.y, p1.z);
        glVertex3f(p2.x, p2.y, p2.z);
        glVertex3f(p3.x, p3.y, p3.z);
      }

      // Face two
      if (is_bfc(p3, p2, p4, vx, vy, vz)) {
        glVertex3f(p4.x, p4.y, p4.z);
      }
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

inline void cpToGl(Patch::CPoint p, GLfloat vs[3]) {
  vs[0] = p.x; vs[1] = p.y; vs[2] = p.z;
}

template <int M, int N>
bool in_bounds(std::pair<int, int> c) {
  return c.first >= 0 && c.second >= 0 && c.first < M && c.second < N;
}

Patch::CPoint average_normal(const std::vector<Patch::CPoint> pts, int i, int j) {
  /*

   * - a - b
   | / | / |
   f - o - c
   | / | / |
   e - d - *

   neighbors: a(0, 1), b(1, 1), c(1, 0), d(0, -1), e(-1, -1), f(-1, 0)

  */

  std::vector<std::pair<int, int> > neighbors = {
    { j,   i-1}, // a
    { j+1, i-1}, // b
    { j+1, i},   // c
    { j,   i+1}, // d
    {j-1,  i+1}, // e
    {j-1,  i},   // f
  };
  std::vector<Patch::CPoint> norms;
  for (auto it = neighbors.begin()+1; it != neighbors.end(); it++) {
    auto v1 = *(it-1);
    auto v2 = *it;
    if (in_bounds<20, 20>(v1) && in_bounds<20, 20>(v2)) {
      norms.push_back(normal(pts[j+20*i],
                             pts[v1.first+20*v1.second],
                             pts[v2.first+20*v2.second]));
    }
  }
  Patch::CPoint avg_norm;
  for (auto& n: norms) {
    avg_norm += n;
  }
  return avg_norm / static_cast<double>(norms.size());
}

void Patch::shade(double vx, double vy, double vz, double shininess, GLfloat diffuse) {
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
  // std::cerr << pts.size() << std::endl;
  GLfloat a_reflection[] = { 0.0, 0.7, 0.0, 1.0 };
  GLfloat d_reflection[] = { 0.0, diffuse, 0.0, 1.0 };
  GLfloat s_reflection[] = { 1.0, 1.0, 1.0, 1.0 };
  glMaterialfv(GL_FRONT, GL_AMBIENT, a_reflection);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, d_reflection);
  glMaterialfv(GL_FRONT, GL_SPECULAR, s_reflection);
  glMaterialf(GL_FRONT, GL_SHININESS, shininess);


  // Draw surfaces
  glColor3f(0.0, 1.0, 0.0);
  for (int i = 0; i < 19; i++) {
    for (int j = 0; j < 19; j++) {
      auto p1 = pts[j + 20*i];
      auto p2 = pts[j+1 + 20*i];
      auto p3 = pts[j + 20*(i+1)];
      auto p4 = pts[j+1 + 20*(i+1)];
      Patch::CPoint n;

      glBegin(GL_TRIANGLES);
      // Face one
      if (is_bfc(p1, p2, p3, vx, vy, vz)) {
        n = average_normal(pts, i, j);
        // std::cerr << n << std::endl;
        glNormal3f(n.x, n.y, n.z);
        glVertex3f(p1.x, p1.y, p1.z);

        n = average_normal(pts, i, j+1);
        glNormal3f(n.x, n.y, n.z);
        glVertex3f(p2.x, p2.y, p2.z);

        n = average_normal(pts, i+1, j);
        glNormal3f(n.x, n.y, n.z);
        glVertex3f(p3.x, p3.y, p3.z);
      }

      // Face two
      if (is_bfc(p3, p2, p4, vx, vy, vz)) {
        n = average_normal(pts, i+1, j);
        glNormal3f(n.x, n.y, n.z);
        glVertex3f(p3.x, p3.y, p3.z);

        n = average_normal(pts, i, j+1);
        glNormal3f(n.x, n.y, n.z);
        glVertex3f(p2.x, p2.y, p2.z);

        n = average_normal(pts, i+1, j+1);
        glNormal3f(n.x, n.y, n.z);
        glVertex3f(p4.x, p4.y, p4.z);
      }
      glEnd();
    }
  }

  glPopMatrix();
}

Patch::CPoint& Patch::operator[](int i) {
  return coords_[i];
}

Patch::CPoint& Patch::CPoint::operator+=(const Patch::CPoint& pt) {
  x += pt.x;
  y += pt.y;
  z += pt.z;
  return *this;
}

Patch::CPoint Patch::CPoint::operator/(double d) {
  return CPoint(x/d, y/d, z/d);
}

std::ostream& operator<<(std::ostream& os, const Patch::CPoint& c) {
  return os << "(" << c.x << "," << c.y << "," << c.z << ")";
}
