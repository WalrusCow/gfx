#pragma once

#include "algebra.hpp"

class ViewConfig;

// Encapsulate transforming a pixel to world coordinates
class PixelTransformer {
 public:
  PixelTransformer(int width, int height, const ViewConfig& viewConfig);
  Point3D transform(int x, int y) const;
 private:
  Matrix4x4 transformMatrix;
};
