#include "FunctionMaterial.hpp"

#include "HitRecord.hpp"

FunctionMaterial::FunctionMaterial(
    const FunctionMaterial::ColourMap& mapFunction_,
    const Colour& ks_,
    double shininess_)
    : Material(ks_, shininess_), mapFunction(mapFunction_) {}

Colour FunctionMaterial::getKd(const HitRecord& hitRecord) {
  return mapFunction(hitRecord.xPercent, hitRecord.yPercent);
}
