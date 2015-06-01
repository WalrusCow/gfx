#pragma once

#include "algebra.hpp"

class Movable {
 public:
  void rotateX(double rad);
  void rotateY(double rad);
  void rotateZ(double rad);
  void translate(double x, double y, double z);

  virtual void reset();

 protected:
  // Composed model transformations
  Matrix4x4 modelMatrix;
};
