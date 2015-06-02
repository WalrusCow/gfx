// William McDonald 20418145 wmcdonal
#include "ViewPoint.hpp"

ViewPoint::ViewPoint() {
  reset();
}

Matrix4x4 ViewPoint::getViewMatrix() const {
  return modelMatrix.invert();
}

void ViewPoint::reset() {
  Movable::reset();
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
  translate(0, 0, -5);
}
