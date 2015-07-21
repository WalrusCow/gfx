#pragma once

#include <string>
#include <vector>

#include "Material.hpp"

class TextureMaterial : public Material {
 public:
  TextureMaterial(const std::string& filename,
                  const Colour& ks,
                  double shininess_,
                  double alpha_);
 protected:
  Colour getKd(const HitRecord& hitRecord) const override;

 private:
  struct Texture {
    std::vector<Colour> data;
    unsigned width;
    unsigned height;
  };
  Texture texture;
  Texture readTexture(const std::string& filename) const;
};
