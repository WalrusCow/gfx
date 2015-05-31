#include "Model.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "xform.hpp"

namespace {

bool isZero(double a) {
  const double epsilon = 1e-5;
  return std::abs(a) < epsilon;
}

constexpr double hypot(double a, double b, double c) {
  return std::sqrt(a*a + b*b + c*c);
}

Matrix4x4 alignToZAxis(const Matrix4x4& startMatrix, const Vector3D& axis) {
  Matrix4x4 result = startMatrix;

  // We need to rotate so the axis is on the world z axis
  // .. but only if the axis isn't already on the world z axis!
  if (!isZero(axis[0]) || !isZero(axis[1])) {
    // Rotate about world y axis to get to world z axis
    double yHyp = hypot(axis[0], axis[1], axis[2]);
    double yAngle = std::acos(axis[2] / yHyp);
    result = yRotationMatrix(-yAngle) * result * yRotationMatrix(yAngle);

    // Rotate about world z axis to get to xz plane
    double zHyp = std::hypot(axis[0], axis[1]);
    double zAngle = std::acos(axis[0] / zHyp);
    result = zRotationMatrix(-zAngle) * result * zRotationMatrix(zAngle);
  }

  return result;
}

}

void Model::rotate(double rad, const Vector3D& axis) {
  // TODO: Do we have to make sure to align our z to *positive* world z?

  // Note that we are doing this inside-out (in opposite order) in order to
  // apply the inverse operation at the same time as the operation.
  Matrix4x4 xform = alignToZAxis(zRotationMatrix(rad), axis);

  // TODO: We could maintain this. It only changes on "translate"
  Point3D centre = modelMatrix * origin;
  xform = translationMatrix(centre[0], centre[1], centre[2]) * xform *
          translationMatrix(-centre[0], -centre[1], -centre[2]);

  modelMatrix = xform * modelMatrix;
}

void Model::rotateX(double rad) {
  rotate(rad, xAxis);
  // Maintain axes
  auto mat = xRotationMatrix(rad);
  yAxis = mat * yAxis;
  zAxis = mat * zAxis;
}

void Model::rotateY(double rad) {
  rotate(rad, yAxis);
  // Maintain axes
  auto mat = yRotationMatrix(rad);
  xAxis = mat * yAxis;
  zAxis = mat * zAxis;
}

void Model::rotateZ(double rad) {
  rotate(rad, zAxis);
  // Maintain axes
  auto mat = zRotationMatrix(rad);
  xAxis = mat * xAxis;
  yAxis = mat * yAxis;
}

void Model::translate(double x, double y, double z) {
  // Simply move relative to our local axes
  auto v1 = x * xAxis;
  auto v2 = y * yAxis;
  auto v3 = z * zAxis;

  auto xform =  translationMatrix(v1[0] + v2[0] + v3[0],
                                  v1[1] + v2[1] + v3[1],
                                  v1[2] + v2[2] + v3[2]);
  modelMatrix = xform * modelMatrix;
}

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

std::vector<Line> Model::getLines() const {
  std::vector<Line> res;
  res.reserve(lines.size());

  // Multiply each point
  std::transform(lines.begin(), lines.end(), res.begin(), [&] (const Line& l) {
    return Line(modelMatrix * l.start, modelMatrix * l.end);
  });

  return res;
}
