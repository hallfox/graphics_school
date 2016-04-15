#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <map>
#include <tuple>
#include <memory>

struct Bitmap {
  using Data = std::unique_ptr<unsigned char[]>;
  int width, height;
  Data data;

  Bitmap() = default;
  Bitmap(int w, int h): width{w}, height{h}, data{new unsigned char[3*w*h]} {}
};

Bitmap load_bmp(const char *);

using Vertex = std::pair<double, double>;
using BaryVertex = std::tuple<double, double, double>;
// RGB
using Color = std::tuple<unsigned char, unsigned char, unsigned char>;
class Triangle {
  public:
    friend std::ostream& operator<<(std::ostream& os, const Triangle&);
    Triangle() = default;
    Triangle(Vertex v1, Vertex v2, Vertex v3):
      v1{v1}, v2{v2}, v3{v3}, texture_{}
    {}
    bool contains_vert(const Vertex&);
    BaryVertex get_coords(const Vertex&);
    Vertex barycentric_to_cart(const BaryVertex&);
    void insert(const Vertex&, const Color&);
    void draw();

    Vertex v1, v2, v3;
    std::map<BaryVertex, Color> texture_;
};

std::ostream& operator<<(std::ostream&, Vertex);

#endif
