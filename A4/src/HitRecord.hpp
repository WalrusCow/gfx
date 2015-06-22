#pragma once

#include "algebra.hpp"
#include "material.hpp"

class HitRecord {
 public:
  HitRecord() {}
  HitRecord(const Vector3D& norm,
            const Point3D& point,
            const Material* material) :
      norm(norm), point(point), material(material) {}

  HitRecord(const HitRecord& other) :
      norm(other.norm), point(other.point), material(other.material) {}

  HitRecord& operator=(const HitRecord& other) {
    norm = other.norm;
    material = other.material;
    point = other.point;
    return *this;
  }

  // What the normal vector was at the point hit
  Vector3D norm;
  // Where the hit occurred
  Point3D point;
  // What material was hit
  const Material* material;
};
