#include "HitRecord.hpp"

bool HitRecord::update(
    const Vector3D& norm, const Point3D& point, double newT) {
  if (newT < 0 || (t >= 0 && newT >= t)) {
    return false;
  }
  this->t = t;
  this->norm = norm;
  this->point = point;
  return true;
}
