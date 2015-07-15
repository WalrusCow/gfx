#pragma once

#include <vector>

#include "algebra.hpp"

class Ray;
class HitRecord;

class Primitive {
 public:
  virtual ~Primitive() = default;
  virtual bool intersects(const Ray& ray,
                          HitRecord* hitRecord,
                          const Matrix4x4& inverseTransform) = 0;
  virtual Point3D getMinPoint(const Matrix4x4& inverseTransform) const = 0;
  virtual Point3D getMaxPoint(const Matrix4x4& inverseTransform) const = 0;
};

namespace primitives {

bool isValid(double t);
double mostValid(double t1, double t2);

}
