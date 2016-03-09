#include <list>
#include <memory>

using Point2d = std::pair<int, int>;

class Drawing {
public:
  Drawing(const Point2d& start);
  virtual ~Drawing() = default;
  void add_point(Point2d pt);
  virtual void draw() = 0;
  std::list<Point2d>& get_points() { return _verts; }
  void finish();
protected:
  std::list<Point2d> _verts;
  bool _is_finished;
};

class LineDrawing: public Drawing {
 public:
  LineDrawing(const Point2d& start): Drawing{start} {}
  void draw();
};

class LoopDrawing: public Drawing {
public:
  LoopDrawing(const Point2d& start): Drawing{start} {}
  void draw();
};

using UniqDrawing = std::unique_ptr<Drawing>;
class Painter {
 public:
  Painter();
  void start_drawing(const Point2d& start);
  void stop_drawing();
  bool is_painting() { return _is_painting; }
  Point2d get_brush() { return _brush; }
  void move_brush(const Point2d& pt) { _brush = pt; }
  void paint();
  void undo();
  void add_point(const Point2d& pt);
 private:
  std::list<UniqDrawing> _drawings;
  bool _is_painting;
  Point2d _brush;
};

