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
  _drawings = std::list<LineDrawing>();
  // FIXME Brensenham draws line from mouse click to here if mouse hasn't been moved
  _brush = std::make_pair(0, 0);
}

void Painter::start_drawing(Point2d pt) {
  if (!_is_drawing) {
    _is_drawing = true;
    _drawings.push_back(LineDrawing(pt));
  }
  // TODO Figure out what happens if a Painter tries to start
  // drawing when they already are
}
