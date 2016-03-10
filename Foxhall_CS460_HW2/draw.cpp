#include "draw.hpp"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <assert.h>
#include <queue>
#include <unordered_set>
#include <iostream>

Drawing::Drawing(const Point2d& start):
  _verts{std::list<Point2d>()},
  _is_finished{false} {
  this->add_point(start);
}

void Drawing::add_point(const Point2d& pt) {
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

BlobDrawing::BlobDrawing(const Point2d& start):
  Drawing{start} {
  unsigned char pixel[4], base_color[4];
  int width = glutGet(GLUT_WINDOW_WIDTH);
  int height = glutGet(GLUT_WINDOW_HEIGHT);
  std::queue<Point2d> to_visit;
  std::unordered_set<Point2d, Point2dHash> seen;
  const std::list<Point2d> neighbors {std::make_pair(1,0), std::make_pair(0,1), std::make_pair(-1,0), std::make_pair(0,-1)};
  glReadPixels(start.first, start.second, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &base_color);

  to_visit.push(start);
  while (!to_visit.empty()) {
    auto pt = to_visit.front();
    to_visit.pop();
    _verts.push_back(pt);

    // Visit neighbors
    for (auto& n: neighbors) {
      auto n_x = pt.first + n.first, n_y = pt.second + n.second;
      auto np = std::make_pair(n_x, n_y);
      if (seen.count(np) == 0 && !(n_x < 0 || n_x > width || n_y < 0 || n_y > height)) {
        glReadPixels(n_x, n_y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
        if (pixel[0] == base_color[0] && pixel[1] == base_color[1] && pixel[2] == base_color[2] && pixel[3] == base_color[3]) {
          seen.insert(np);
          to_visit.push(np);
          // std::cout << "Visited " << pt.first << ", " << pt.second << std::endl;
        }
      }
    }
  }
  _is_finished = true;
}

void BlobDrawing::draw() {
  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_POINTS);

  if (_verts.size() > 1) {
    for (const auto& p: _verts) {
      glVertex2i(p.first, p.second);
    }
  }

  glEnd();
  glColor3f(0.0, 0.0, 0.0);
}

Painter::Painter() {
  _is_painting = false;
  _drawings = std::list<std::unique_ptr<Drawing> >();
  _brush = std::make_pair(0, 0);
}

void Painter::start_drawing(const Point2d& pt) {
  assert(!_is_painting);
  _is_painting = true;
  _drawings.push_back(std::unique_ptr<Drawing>(new LoopDrawing(pt)));
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
  assert(_is_painting);
  _drawings.back()->add_point(pt);
}

inline bool inside_boundary(Point2d a, Point2d b, Point2d x) {
  return (b.first - a.first)*(x.second - a.second) - (x.first - a.first)*(b.second - a.second) >= 0;
}

Point2d intersect(const std::pair<Point2d, Point2d>& v1, const std::pair<Point2d, Point2d>& v2) {
  int a1 = (v1.first.first*v1.second.second - v1.first.second*v1.second.first)*(v2.first.first - v2.second.first) -
    (v1.first.first - v1.second.first)*(v2.first.first*v2.second.second - v2.first.second*v2.second.first);

  int a2 = (v1.first.first*v1.second.second - v1.first.second*v1.second.first)*(v2.first.second - v2.second.second) -
    (v1.first.second - v1.second.second)*(v2.first.first*v2.second.second - v2.first.second*v2.second.first);

  int b = (v1.first.first - v1.second.first)*(v2.first.second - v2.second.second) -
    (v1.first.second - v1.second.second)*(v2.first.first - v2.second.first);

  assert(b != 0);
  return std::make_pair(a1/b, a2/b);
}

void Painter::clip(const std::list<Point2d>& clip_window) {
  std::list<Point2d> output_verts;

  for (auto& pic: _drawings) {
    output_verts = pic->get_points();

    for (auto it = clip_window.cbegin(); it != clip_window.cend();) {
      std::pair<Point2d, Point2d> clip_edge;
      if (it == --clip_window.cend()) {
        clip_edge = std::make_pair(*it, *clip_window.cbegin());
        it++;
      } else {
        clip_edge = std::make_pair(*it, *(++it));
      }
      auto input_verts = output_verts;
      output_verts.clear();
      auto last = input_verts.back();
      // Clipping assumes vertices are drawn counterclockwise
      for (auto& p: input_verts) {
        if (inside_boundary(clip_edge.first, clip_edge.second, p)) {
          if (!inside_boundary(clip_edge.first, clip_edge.second, last)) {
            // outside -> inside
            output_verts.push_back(intersect(std::make_pair(p, last), clip_edge));
          }
          // inside -> inside or outside -> inside
          output_verts.push_back(p);
        } else if (inside_boundary(clip_edge.first, clip_edge.second, last)) {
          // inside -> outside
          output_verts.push_back(intersect(std::make_pair(p,last), clip_edge));
        }
        last = p;
      }
    }
    pic = UniqDrawing(new LoopDrawing(output_verts));
  }

}

void Painter::fill(const Point2d& p) {
  if (p == Point2d(-1, -1)) {
    _drawings.push_back(UniqDrawing(new BlobDrawing(get_brush())));
  } else {
    _drawings.push_back(UniqDrawing(new BlobDrawing(p)));
  }
}

void Painter::delete_drawings() {
  assert(!_is_painting);
  _drawings.clear();
}
