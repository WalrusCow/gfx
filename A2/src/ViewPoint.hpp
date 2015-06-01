#pragma once

#include "algebra.hpp"
#include "Movable.hpp"

class ViewPoint : public Movable {
 public:
  ViewPoint();

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
