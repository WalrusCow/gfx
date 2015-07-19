#include "ColourMaterial.hpp"

ColourMaterial::ColourMaterial(
    const Colour& kd_, const Colour& ks_, double shininess_)
    : Material(ks_, shininess_), kd(kd_) {}


Colour ColourMaterial::getKd(const HitRecord& hitRecord) const {
  (void) hitRecord; // Remove warnings
  return kd;
}