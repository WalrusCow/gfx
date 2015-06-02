// William McDonald 20418145 wmcdonal
#pragma once

#include <string>


#include "algebra.hpp"
#include "Movable.hpp"

class Model : public Movable {
 public:
  // A model is composed of a number of lines
  Model(std::string name, std::vector<Line3D> lines) :
    name(std::move(name)), lines(std::move(lines)) {}

  void scale(double x, double y, double z);

  // We want to expose the transformed lines
  std::vector<Line3D> getLines() const;

 private:
  std::string name;
  std::vector<Line3D> lines;
};
