// William McDonald 20418145 wmcdonal
#include "Movable.hpp"

#include <cmath>

#include "xform.hpp"

void Movable::rotateX(double rad) {
  auto mat = xRotationMatrix(rad);
  modelMatrix *= mat;
}

void Movable::rotateY(double rad) {
  auto mat = yRotationMatrix(rad);
  modelMatrix *= mat;
}

void Movable::rotateZ(double rad) {
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
