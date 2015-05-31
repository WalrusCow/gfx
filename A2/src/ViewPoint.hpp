#pragma once

#include "algebra.hpp"
#include "Movable.hpp"

class ViewPoint : public Movable {
 public:
  ViewPoint() {
    // Where we are facing: Inwards
    yAxis = {0, 1, 0};
    zAxis = {0, 0, -1};
    xAxis = zAxis.cross(yAxis);

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
  void setFOV(double rad);
  void setNearPlane(double z);
  void setFarPlane(double z);
};
