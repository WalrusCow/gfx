#pragma once

#include <vector>

#include "algebra.hpp"
#include "Movable.hpp"

class Line {
 public:
  Line(Point3D start, Point3D end) : start(start), end(end) {}
  Point3D start;
  Point3D end;
};

class Model : public Movable {
 public:
  // A model is composed of a number of lines
  Model(std::vector<Line> lines) : lines(std::move(lines)) {}

  void scale(double x, double y, double z);

  // We want to expose the transformed lines
  std::vector<Line> getLines() const;

 private:
  std::vector<Line> lines;
};
