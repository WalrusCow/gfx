// William McDonald 20418145 wmcdonal
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

void Movable::rotateX(double rad) {
  // Maintain axes
  auto mat = xRotationMatrix(rad);
  modelMatrix *= mat;
}

void Movable::rotateY(double rad) {
  // Maintain axes
  auto mat = yRotationMatrix(rad);
  modelMatrix *= mat;
}

void Movable::rotateZ(double rad) {
  // Maintain axes
  auto mat = zRotationMatrix(rad);
  modelMatrix *= mat;
}

void Movable::translate(double x, double y, double z) {
  auto mat = translationMatrix(x, y, z);
  modelMatrix *= mat;
}

void Movable::reset() {
  modelMatrix = Matrix4x4();
}
