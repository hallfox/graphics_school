#include <list>
#include <memory>
#include <functional>

using Point2d = std::pair<int, int>;
struct Point2dHash {
  inline std::size_t operator()(const Point2d& p) const {
    std::hash<int> hasher;
    return hasher(p.first) ^ hasher(p.second);
  }
};

class Drawing {
public:
  Drawing(const Point2d& start);
  Drawing(const std::list<Point2d>& vs): _verts{vs}, _is_finished{true} {}
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
  LineDrawing(const std::list<Point2d>& start): Drawing{start} {}
  void draw();
};

class LoopDrawing: public Drawing {
public:
  LoopDrawing(const Point2d& start):
    Drawing{start} {}
  LoopDrawing(const std::list<Point2d>& start):
    Drawing{start} {}
  void draw();
};

class BlobDrawing: public Drawing {
public:
  BlobDrawing(const Point2d& start);
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
  void clip(const std::list<Point2d>& clip_window);
  void fill();
 private:
  std::list<UniqDrawing> _drawings;
  bool _is_painting;
  Point2d _brush;
};

