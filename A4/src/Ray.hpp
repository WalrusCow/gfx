#pragma once

#include "algebra.hpp"

class Ray {
 public:
  // A Ray is a line, defined by two points
  // Rays have no end, but they have a start and a direction
  Ray(const Point3D& start, const Point3D& other) :
    start(start), other(other), dir(other - start) {}
  Point3D start;
  Point3D other;
  Vector3D dir;
  Point3D at(double t) const {
    return start + t * dir;
  }
};
