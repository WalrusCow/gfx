#include "Model.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "xform.hpp"

void Model::scale(double x, double y, double z) {
  // Note that we are doing this inside-out (in opposite order) in order to
  // apply the inverse operation at the same time as the operation.

  // Note that it doesn't matter here if we align the z axis negatively,
  // because scale happens both positive and negative
  Matrix4x4 xform = alignToZAxis(scaleMatrix(x, y, z), zAxis);

  // TODO: We could maintain this. It only changes on "translate"
  Point3D centre = modelMatrix * origin;
  xform = translationMatrix(centre[0], centre[1], centre[2]) * xform *
          translationMatrix(-centre[0], -centre[1], -centre[2]);

  modelMatrix = xform * modelMatrix;
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

void Model::rotateX(double rad) {
  Movable::rotateX(rad);
  if (name == "Box") {
    std::cerr << "Box x axis: " << xAxis << std::endl;
  }
  if (name == "BoxGnomon") {
    std::cerr << "Box gnomon x: " << lines[0].end << std::endl;
    std::cerr << "Box gnomon x xform: " << modelMatrix * lines[0].end << std::endl;
    std::cerr << "Box gnomon x axis: " << xAxis << std::endl;
  }
}
