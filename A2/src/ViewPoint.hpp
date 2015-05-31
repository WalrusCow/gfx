#pragma once

#include "algebra.hpp"

class ViewPoint {
 public:
  void translate(double x, double y, double z);
  void rotateX(double rad);
  void rotateY(double rad);
  void rotateZ(double rad);

  // TODO: Clipping
  void setFOV(double rad);
  void setNearPlane(double z);
  void setFarPlane(double z);

 private:
  // Where we initially look from
  Point3D viewOrigin = {0, 0, 3};

  Vector3D yAxis = {0, 1, 0};
  Vector3D zAxis = {0, 0, -1};
  Vector3D xAxis = zAxis.cross(yAxis);
};
