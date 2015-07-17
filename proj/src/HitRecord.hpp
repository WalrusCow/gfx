#pragma once

#include "algebra.hpp"
#include "materials/Material.hpp"

class HitRecord {
 public:
  HitRecord() {}
  HitRecord(const Vector3D& norm,
            const Point3D& point,
            const Material* material,
            double t) :
      norm(norm), point(point), material(material), t(t){}

  HitRecord(const HitRecord& o) :
      norm(o.norm), point(o.point), material(o.material), t(o.t) {}

  HitRecord& operator=(const HitRecord& o) {
    norm = o.norm;
    material = o.material;
    point = o.point;
    t = o.t;
    return *this;
  }

  // Try to update hitrecord with new hit data.
  // Return whether or not we did update (i.e. is the new t better?)
  bool update(const Vector3D& norm, const Point3D& point, double t);

  // What the normal vector was at the point hit
  Vector3D norm;
  // Where the hit occurred
  Point3D point;
  // What material was hit
  const Material* material;
  // t for Ray where we hit
  double t = -1;
};
