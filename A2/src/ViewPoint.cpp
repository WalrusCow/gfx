#include "ViewPoint.hpp"

Matrix4x4 ViewPoint::getViewMatrix() const {
  // Do a change of frame from standard frame (world frame) to our frame
  // (which consists of the axes and the "origin" we have)
  Matrix4x4 viewMatrix;

  auto oSub = origin - (modelMatrix * origin);

  int row = 0;
  for (const auto& axis : {xAxis, yAxis, zAxis}) {
    int col = 0;
    for (; col < 3; ++col) {
      // Dot product with `col`th standard basis vector is `col`th entry
      viewMatrix[row][col] = axis[col];
    }
    // Last column consists of difference in origins dot axis in question
    viewMatrix[row++][col] = oSub.dot(axis);
  }
  return viewMatrix;
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
