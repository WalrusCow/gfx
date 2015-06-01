#include "Movable.hpp"

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

Matrix4x4 Movable::alignToZAxis(const Matrix4x4& startMatrix,
                                const Vector3D& axis) {
  Matrix4x4 result = startMatrix;

  std::cerr << "rotating with respect to " << axis << std::endl;

  // We need to rotate so the axis is on the world z axis
  // .. but only if the axis isn't already on the world z axis!
  if (!isZero(axis[0]) || !isZero(axis[1])) {

    //double h = hypot(axis[0], axis[1], axis[2]);
    // Angle to xz plane is pi/2 - angle to normal=y
    // a.dot(b) == |a||b|cos(theta)
    // but a and b are unit vectors!
    //axis[1] == h*cos(theta)
    double angleToXZ = (M_PI / 2) - std::acos(axis[1]);

    auto newP = zRotationMatrix(angleToXZ) * axis;

    // Now after doing that we will need the angle to the YZ plane
    double angleToYZ = (M_PI / 2) - std::acos(newP[0]);

    // Rotate about world y axis to get to world z axis
    //double yHyp = hypot(axis[0], axis[1], axis[2]);
    //double yAngle = -1 * std::asin(axis[1] / yHyp);
    double yAngle = angleToYZ;
    auto yM = yRotationMatrix(yAngle);
    result = yM.invert() * result * yM;
    std::cerr << "y angle: " << yAngle << std::endl;

    // Rotate about world z axis to get to xz plane
    //double zHyp = hypot(axis[0], axis[1]);
    //double zAngle = std::asin(axis[1] / yHyp);
    double zAngle = angleToXZ;
    auto zM = zRotationMatrix(zAngle);
    result = zM.invert() * result * zM;
    std::cerr << "z angle: " << zAngle << std::endl;

    std::cerr << "Before rotation: " << axis << std::endl;
    std::cerr << "Should be on XZ plane: " << zM * axis << std::endl;
    std::cerr << "Should be on Z axis: " << yM * zM * axis << std::endl;
  }
  else {
    std::cerr << "nothing to do" << std::endl;
  }

  return result;
}

void Movable::rotate(double rad, const Vector3D& axis) {
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

void Movable::rotateX(double rad) {
  rotate(rad, xAxis);
  // Maintain axes
  auto mat = xRotationMatrix(rad);
  yAxis = mat * yAxis;
  zAxis = mat * zAxis;
}

void Movable::rotateY(double rad) {
  rotate(rad, yAxis);
  // Maintain axes
  auto mat = yRotationMatrix(rad);
  xAxis = mat * xAxis;
  zAxis = mat * zAxis;
}

void Movable::rotateZ(double rad) {
  rotate(rad, zAxis);
  // Maintain axes
  auto mat = zRotationMatrix(rad);
  xAxis = mat * xAxis;
  yAxis = mat * yAxis;
}

void Movable::translate(double x, double y, double z) {
  // Simply move relative to our local axes
  auto v1 = x * xAxis;
  auto v2 = y * yAxis;
  auto v3 = z * zAxis;

  auto xform =  translationMatrix(v1[0] + v2[0] + v3[0],
                                  v1[1] + v2[1] + v3[1],
                                  v1[2] + v2[2] + v3[2]);
  modelMatrix = xform * modelMatrix;
}
