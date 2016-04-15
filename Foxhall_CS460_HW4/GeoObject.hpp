#ifndef GEO_OBJECT_H_
#define GEO_OBJECT_H_

#include <vector>

class GeoObject {
  using Vertex = std::tuple<double, double, double>;
  using Face = std::tuple<int, int, int>;
  public:
    GeoObject() = default;
    GeoObject(std::string);
    void draw();
  private:
    std::vector<Vertex> verts_;
    std::vector<Face> faces_;
};

#endif
