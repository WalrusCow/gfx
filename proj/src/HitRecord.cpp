#include "HitRecord.hpp"

bool HitRecord::update(const Vector3D& n, const Point3D& pt, double newT,
                       double xp, double yp) {
  if (newT < 0 || (t >= 0 && newT >= t)) {
    return false;
  }
  t = newT;
  norm = n;
  point = pt;
  xPercent = xp;
  yPercent = yp;
  return true;
}
