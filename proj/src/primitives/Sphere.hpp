#pragma once

#include "Cube.hpp"
#include "Primitive.hpp"

class Sphere : public Primitive {
 public:
  bool intersects(const Ray& ray,
                  HitRecord* hitRecord,
                  const Matrix4x4& inverseTransform) const override;
  Point3D getMinPoint(const Matrix4x4& inverseTransform) const override;
  Point3D getMaxPoint(const Matrix4x4& inverseTransform) const override;
 private:
  double solveIntersection(const Point3D& p1, const Vector3D& dir) const;

  Cube boundingCube;
};
