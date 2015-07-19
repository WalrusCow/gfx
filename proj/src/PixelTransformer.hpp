#pragma once

#include "algebra.hpp"

class ViewConfig;

// Encapsulate transforming a pixel to world coordinates
class PixelTransformer {
 public:
  PixelTransformer(int width, int height, const ViewConfig& viewConfig);
  Point3D transform(double x, double y) const;
 private:
  Matrix4x4 transformMatrix;
};
