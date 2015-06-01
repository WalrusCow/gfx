#pragma once

#include "algebra.hpp"

class Movable {
 public:
  void rotateX(double rad);
  void rotateY(double rad);
  void rotateZ(double rad);

  void translate(double x, double y, double z);

  Matrix4x4 alignToZAxis(const Matrix4x4& startMatrix, const Vector3D& axis);

 protected:
  // Composed model transformations
  Matrix4x4 modelMatrix;

  // Yay
  Point3D origin;

  Vector3D xAxis = {1, 0, 0};
  Vector3D yAxis = {0, 1, 0};
  Vector3D zAxis = {0, 0, 1};
};
