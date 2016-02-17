#include "draw.h"

LineDrawing::LineDrawing(Point2d start) {
  _verts = std::list<Point2d>();
  this->add_point(start);
}

void LineDrawing::add_point(Point2d pt) {
  _verts.push_back(pt);
}

Painter::Painter() {
  _is_drawing = false;
  _drawings = std::list<std::unique_ptr<LineDrawing> >();
}

void Painter::start_drawing(Point2d pt) {
  if (!_is_drawing) {
    _is_drawing = true;
    _drawings.push_back(std::unique_ptr<LineDrawing>(new LineDrawing(pt)));
  }
  // TODO Figure out what happens if a Painter tries to start
  // drawing when they already are
}
