#pragma once

#include "algebra.hpp"
#include "Movable.hpp"

class ViewPoint : public Movable {
 public:
  ViewPoint() {
    yAxis = {0, 1, 0};
    zAxis = {0, 0, 1};
    // Backwards for view
    xAxis = {-1, 0, 0};

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
    translate(0, 0, -3);
  }

  // Clip the line (modify it in place) and return if it is included
  // or not.
  bool clipLine(Line3D* line);

  // Turn a 3D point into a 2D version of where it should be

  Point2D projectPoint(const Point3D& point);

  Matrix4x4 getViewMatrix() const;

  // TODO: Clipping
  void changeFov(double rad);
  void translateNearPlane(double z);
  void translateFarPlane(double z);
};
