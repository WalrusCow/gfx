#include "Model.hpp"

#include "HitRecord.hpp"

Model::Model(Primitive* primitive_,
             Material* material_,
             const Matrix4x4& xform_)
  : primitive(primitive_), material(material_), xform(xform_) { }

bool Model::intersects(const Ray& ray, HitRecord* hitRecord) const {
  if (primitive->intersects(ray, hitRecord, xform)) {
    hitRecord->material = material;
    return true;
  }
  return false;
}
