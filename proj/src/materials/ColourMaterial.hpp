#pragma once

#include "Material.hpp"

class ColourMaterial : public Material {
 public:
  ColourMaterial(
      const Colour& kd, const Colour& ks, double shininess, double alpha_);

 protected:
  Colour getKd(const HitRecord& hitRecord) const override;

 private:
  Colour kd;
};
