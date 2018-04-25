#ifndef PATCH_HPP_
#define PATCH_HPP_

#include <vector>
#include <OpenGL/gl.h>

class Patch {
public:
  struct CPoint {
    double x, y, z;
    friend std::ostream& operator<<(std::ostream&, const CPoint&);
    double& operator[](int);
    CPoint operator-(const CPoint&);
    CPoint& operator+=(const CPoint&);
    CPoint operator/(double);
    CPoint unit();
    CPoint(): x{0}, y{0}, z{0} {}
    CPoint(double x, double y, double z):
      x{x}, y{y}, z{z} {}
  };

  Patch();

  void draw(double, double, double);
  void shade(double, double, double, double, GLfloat);
  CPoint& operator[](int);
private:
  Patch::CPoint bezier_param(double, double);
  static double bernstein4(int, double);

  std::vector<CPoint> coords_;
};

#endif
