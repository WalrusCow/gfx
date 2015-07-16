#pragma once

#include "algebra.hpp"
#include "material.hpp"
#include "Ray.hpp"
#include "primitives/Primitive.hpp"

class HitRecord;

class Model {
 public:
  Model(Primitive* primitive_,
        Material* material_,
        const Matrix4x4& xform_);

  bool intersects(const Ray& ray, HitRecord* hitRecord) const;

 private:
  const Primitive* primitive;
  const Material* material;
  const Matrix4x4 xform;
};
