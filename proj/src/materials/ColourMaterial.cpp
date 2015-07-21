#include "ColourMaterial.hpp"

ColourMaterial::ColourMaterial(const Colour& kd_, const Colour& ks_,
                               double shininess_, double alpha_, double idx_)
    : Material(ks_, shininess_, alpha_, idx_), kd(kd_) {}


Colour ColourMaterial::getKd(const HitRecord& hitRecord) const {
  (void) hitRecord; // Remove warnings
  return kd;
}
