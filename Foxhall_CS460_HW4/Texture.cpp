#include "Texture.hpp"

#include <OpenGL/gl.h>

#include <iostream>
#include <fstream>

// Triangle
bool Triangle::contains_vert(const Vertex& v) {
  auto barys = this->get_coords(v);
  auto a = std::get<0>(barys);
  auto b = std::get<1>(barys);
  auto c = std::get<2>(barys);
  return 0 <= a && a <= 1 && 0 <= b && b <= 1 && 0 <= c && c <= 1;
}

inline BaryVertex Triangle::get_coords(const Vertex& v) {
  double a = ((v2.second-v3.second)*(v.first-v3.first)+(v3.first-v2.first)*(v.second-v3.second))/((v2.second-v3.second)*(v1.first-v3.first)+(v3.first-v2.first)*(v1.second-v3.second));
  double b = ((v3.second-v1.second)*(v.first-v3.first)+(v1.first-v3.first)*(v.second-v3.second))/((v2.second-v3.second)*(v1.first-v3.first)+(v3.first-v2.first)*(v1.second-v3.second));
  return BaryVertex(a, b, 1-a-b);
}

inline Vertex Triangle::barycentric_to_cart(const BaryVertex& bv) {
  auto a = std::get<0>(bv);
  auto b = std::get<1>(bv);
  auto c = std::get<2>(bv);
  auto x = v1.first*a + v2.first*b + v3.first*c;
  auto y = v1.second*a + v2.second*b + v3.second*c;
  return Vertex(x, y);
}

void Triangle::insert(const Vertex& v, const Color& c) {
  auto barys = this->get_coords(v);
  texture_.insert(std::make_pair(barys, c));
}

void Triangle::draw() {
  glBegin(GL_POINTS);
  for (auto it: texture_) {
    auto r = std::get<0>(it.second);
    auto g = std::get<1>(it.second);
    auto b = std::get<2>(it.second);
    auto cart = this->barycentric_to_cart(it.first);
    glColor3ub(r, g, b);
    glVertex2d(cart.first, cart.second);
    glVertex2d(cart.first+1, cart.second+1);
    glVertex2d(cart.first+2, cart.second+2);
    glVertex2d(cart.first+3, cart.second+3);
  }
  glEnd();
}

std::ostream& operator<<(std::ostream& os, Vertex bv) {
  return os << bv.first << "," << bv.second;
}

std::ostream& operator<<(std::ostream& os, const Triangle& t) {
  return os << "Triangle (" << t.v1 << "; " << t.v2 << "; " << t.v3 << ")";
}

// Bitmap loading utility
Bitmap load_bmp(const char *filename) {
  // Load bmp file, BGR
  std::ifstream file(filename, std::ios::binary | std::ios::in);
  int width, height;
  // Header ends at 54, height at 18 and width at 22
  file.seekg(18);
  file.read((char *)&width, 4);
  file.read((char *)&height, 4);

  Bitmap bmp{width, height};
  file.seekg(54);
  // Read in BGR pixels
  file.read((char *)bmp.data.get(), 3*width*height);

  return bmp;
}

