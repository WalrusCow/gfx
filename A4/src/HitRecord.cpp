#include "HitRecord.hpp"

bool HitRecord::update(
    const Vector3D& newNorm, const Point3D& newPt, double newT) {
  if (newT < 0 || (t >= 0 && newT >= t)) {
    return false;
  }
  t = newT;
  norm = newNorm;
  point = newPt;
  return true;
}
