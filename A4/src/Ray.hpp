#pragma once

#include "algebra.hpp"

class Ray {
 public:
  // A Ray is a line, defined by two points
  // Rays have no end, but they have a start and a direction
  Ray(const Point3D& start, const Point3D& other) :
    start(start), dir(other - start) {}
  Point3D start;
  Vector3D dir;
};
