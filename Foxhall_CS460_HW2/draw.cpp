#include "draw.hpp"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <assert.h>

Drawing::Drawing(const Point2d& start):
  _verts{std::list<Point2d>()},
  _is_finished{false} {
  this->add_point(start);
}

void Drawing::add_point(Point2d pt) {
  assert(!_is_finished);
  _verts.push_back(pt);
}

void Drawing::finish() {
  assert(!_is_finished);
  _is_finished = true;
}

void LineDrawing::draw() {
  glBegin(GL_LINE_STRIP);
  if (_verts.size() > 1) {
    for (const auto& p: _verts) {
      glVertex2i(p.first, p.second);
    }
  }
  glEnd();
}

void LoopDrawing::draw() {
  if (_is_finished) {
    glBegin(GL_LINE_LOOP);
  } else {
    glBegin(GL_LINE_STRIP);
  }
  if (_verts.size() > 1) {
    for (const auto& p: _verts) {
      glVertex2i(p.first, p.second);
    }
  }
  glEnd();
}

Painter::Painter() {
  _is_painting = false;
  _drawings = std::list<std::unique_ptr<Drawing> >();
  _brush = std::make_pair(0, 0);
}

void Painter::start_drawing(const Point2d& pt) {
  if (!_is_painting) {
    _is_painting = true;
    _drawings.push_back(std::unique_ptr<Drawing>(new LoopDrawing(pt)));
  }
}

void Painter::stop_drawing() {
  assert(_is_painting);
  _is_painting = false;
  _drawings.back()->finish();
}

void Painter::paint() {
  for (auto& drawing: _drawings) {
    drawing->draw();
  }
  if (_is_painting) {
    auto last = _drawings.back()->get_points().back();
    glBegin(GL_LINES);
    glVertex2i(last.first, last.second);
    glVertex2i(_brush.first, _brush.second);
    glEnd();
  }
}

void Painter::undo() {
  assert(!_is_painting);
  _drawings.pop_back();
}

void Painter::add_point(const Point2d& pt) {
  _drawings.back()->add_point(pt);
}
