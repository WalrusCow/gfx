#include "Model.hpp"

#include <cmath>

#include "xform.hpp"

namespace {

bool isZero(double a) {
  const double epsilon = 1e-5;
  return std::abs(a) < epsilon;
}

constexpr double hypot(double a, double b, double c) {
  return std::sqrt(a*a + b*b + c*c);
}

}

void Model::rotate(double rad, const Vector3D& axis) {
  // First, we need to translate to the origin

  // Note that we are doing this inside-out (in opposite order) in order to
  // apply the inverse operation at the same time as the operation.
  Matrix4x4 xform = zRotationMatrix(rad);

  // We need to rotate so the axis is on the world z axis
  // .. but only if the axis isn't already on the world z axis!
  if (!isZero(axis[0]) || !isZero(axis[1])) {
    // Rotate about world y axis to get to world z axis
    double yHyp = hypot(xAxis[0], xAxis[1], xAxis[2]);
    double yAngle = std::acos(xAxis[2] / yHyp);
    xform = yRotationMatrix(-yAngle) * xform * yRotationMatrix(yAngle);

    // Rotate about world z axis to get to xz plane
    double zHyp = std::hypot(xAxis[0], xAxis[1]);
    double zAngle = std::acos(xAxis[0] / zHyp);
    xform = zRotationMatrix(-zAngle) * xform * zRotationMatrix(zAngle);
  }

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
  // In order to translate about our own shit, we have to translate
  // back to the origin, then rotate to the original axes, then slide,
  // then rotate back, then translate back to where we were originally

  //auto xform = translationMatrix(x, y, z);

  // Rotate to original axes position
  //xform = 

}
