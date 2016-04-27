#ifndef PATCH_HPP_
#define PATCH_HPP_

#include <vector>

class Patch {
  public:
    struct CPoint {
      double x, y, z;
      double& operator[](int);
      CPoint(): x{0}, y{0}, z{0} {}
      CPoint(double x, double y, double z):
        x{x}, y{y}, z{z} {}
    };

    Patch();

    void draw();
    CPoint& operator[](int);
  private:
    Patch::CPoint bezier_param(double, double);
    static double bernstein4(int, double);

    std::vector<CPoint> coords_;
};

#endif
