#include "ViewPoint.hpp"

ViewPoint::ViewPoint() {
  Matrix4x4 reflMatrix = {
    {-1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
  };

  // First reflect x
  modelMatrix = reflMatrix * modelMatrix;
  rotateY(M_PI);
  // Back up to get a better view
  translate(0, 0, -3);
}

Matrix4x4 ViewPoint::getViewMatrix() const {
  return modelMatrix.invert();
}

void ViewPoint::changeFov(double rad) {
  // TODO
}

void ViewPoint::translateNearPlane(double z) {
  // TODO
}

void ViewPoint::translateFarPlane(double z) {
  // TODO
}
