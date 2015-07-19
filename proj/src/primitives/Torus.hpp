#pragma once

#include "Cube.hpp"
#include "Primitive.hpp"

// Base torus is lying flat in the XY plane.
class Torus : public Primitive {
 public:
  // Take as input the tube radius.
  Torus(double tubeRadius_);
  bool intersects(const Ray& ray,
                  HitRecord* hitRecord,
                  const Matrix4x4& inverseTransform) const override;
  Point3D getMinPoint(const Matrix4x4& inverseTransform) const override;
  Point3D getMaxPoint(const Matrix4x4& inverseTransform) const override;
  std::vector<Point3D>
  getBoundingBox(const Matrix4x4& inverseTransform) const override;

 private:
  const double tubeRadius;
  Cube boundingCube;
  Matrix4x4 cubeInverse;

  double solveIntersection(const Ray& localRay) const;
};
