#pragma once

#include "algebra.hpp"

class ViewConfig {
 public:
  ViewConfig(const Point3D& position, const Vector3D& viewDirection,
             const Vector3D& up, double fov) :
    eye(position), view(viewDirection), up(up), fov(fov) {}
  Point3D eye;
  Vector3D view;
  Vector3D up;
  double fov;
};
