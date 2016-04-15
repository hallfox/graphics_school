#include "GeoObject.hpp"
#include <fstream>
#include <assert.h>
#include <OpenGL/gl.h>

GeoObject::GeoObject(std::string filename) {
  std::ifstream file(filename);
  std::string leader;
  double a, b, c;
  int x, y, z;

  while (!file.eof()) {
    file >> leader;
    if (leader == "v") {
      file >> a >> b >> c;
      verts_.push_back(Vertex(a, b, c));
    } else if (leader == "f") {
      file >> x >> y >> z;
      faces_.push_back(Face(x, y, z));
    } else {
      //assert(false);
    }

  }
}

void GeoObject::draw() {
  for (auto f: faces_) {
    const Vertex& v1 = verts_[std::get<0>(f) - 1];
    const Vertex& v2 = verts_[std::get<1>(f) - 1];
    const Vertex& v3 = verts_[std::get<2>(f) - 1];

    glBegin(GL_LINE_LOOP);
    glVertex3d(std::get<0>(v1), std::get<1>(v1), std::get<2>(v1));
    glVertex3d(std::get<0>(v2), std::get<1>(v2), std::get<2>(v2));
    glVertex3d(std::get<0>(v3), std::get<1>(v3), std::get<2>(v3));
    glEnd();
  }
}
