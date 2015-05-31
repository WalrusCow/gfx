#include "xform.hpp"

#include <cmath>

Matrix4x4 xRotationMatrix(double rad) {
  double c = std::cos(rad);
  double s = std::sin(rad);
  return Matrix4x4(
      Vector4D(1, 0, 0, 0),
      Vector4D(0, c, -s, 0),
      Vector4D(0, s, c, 0),
      Vector4D(0, 0, 0, 1)
  );
}

Matrix4x4 yRotationMatrix(double rad) {
  double c = std::cos(rad);
  double s = std::sin(rad);
  return Matrix4x4(
      Vector4D(c, 0, s, 0),
      Vector4D(0, 1, 0, 0),
      Vector4D(-s, 0, c, 0),
      Vector4D(0, 0, 0, 1)
  );
}

Matrix4x4 zRotationMatrix(double rad) {
  double c = std::cos(rad);
  double s = std::sin(rad);
  return Matrix4x4(
      Vector4D(c, -s, 0, 0),
      Vector4D(s, c, 0, 0),
      Vector4D(0, 0, 1, 0),
      Vector4D(0, 0, 0, 1)
  );
}

Matrix4x4 scaleMatrix(double x, double y, double z) {
  return Matrix4x4(
      Vector4D(x, 0, 0, 0),
      Vector4D(0, y, 0, 0),
      Vector4D(0, 0, z, 0),
      Vector4D(0, 0, 0, 1)
  );
}

Matrix4x4 translationMatrix(double x, double y, double z) {
  return Matrix4x4(
      Vector4D(1, 0, 0, x),
      Vector4D(0, 1, 0, y),
      Vector4D(0, 0, 1, z),
      Vector4D(0, 0, 0, 1)
  );
}
