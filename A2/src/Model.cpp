#include "Model.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "xform.hpp"

void Model::scale(double x, double y, double z) {
  modelMatrix *= scaleMatrix(x, y, z);
}

std::vector<Line3D> Model::getLines() const {
  std::vector<Line3D> res;
  res.reserve(lines.size());

  auto xform = [&] (const Line3D& l) {
    return Line3D(modelMatrix * l.start, modelMatrix * l.end);
  };

  // Multiply each point
  std::transform(lines.begin(), lines.end(), std::back_inserter(res), xform);
  return res;
}
