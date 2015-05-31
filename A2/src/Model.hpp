#pragma once

#include <vector>

#include "algebra.hpp"
#include "Movable.hpp"

class Model : public Movable {
 public:
  // A model is composed of a number of lines
  Model(std::vector<Line3D> lines) : lines(std::move(lines)) {}

  void scale(double x, double y, double z);

  // We want to expose the transformed lines
  std::vector<Line3D> getLines() const;

 private:
  std::vector<Line3D> lines;
};
