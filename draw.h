#include <list>

using Point2d = std::pair<int, int>;

class LineDrawing {
 public:
  LineDrawing(Point2d start);
  void add_point(Point2d pt);
  std::list<Point2d>& get_points() { return _verts; }
 private:
  std::list<Point2d> _verts;
};

class Painter {
 public:
  Painter();
  void start_drawing(Point2d start);
  void stop_drawing() { _is_drawing = false; };
  LineDrawing& get_current_drawing() { return _drawings.back(); } // TODO Account for what happens when painter is not drawing
  std::list<LineDrawing>& get_drawings() { return _drawings; }
  bool is_drawing() { return _is_drawing; }
  Point2d get_brush() { return _brush; }
  void set_brush(Point2d pt) { _brush = pt; }
 private:
  std::list<LineDrawing> _drawings;
  bool _is_drawing;
  Point2d _brush;
};
