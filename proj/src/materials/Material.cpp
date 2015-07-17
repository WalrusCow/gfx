#include "Material.hpp"

#include <algorithm>
#include <cmath>
#include "light.hpp"
#include "HitRecord.hpp"

Material::Material(const Colour& ks_, double shininess_)
    : ks(ks_), shininess(shininess_) {}

Colour Material::getColour(const HitRecord& hitRecord) const {
  return getKd(hitRecord);
}

Colour Material::lightColour(const Colour& colour,
                             const Vector3D& dir,
                             const Light& light,
                             const HitRecord& hitRecord) const {
  // Direction to the light (away from surface)
  auto lightDir = light.position - hitRecord.point;
  Colour lightColour = light.colour;

  auto r = lightDir.length();
  lightDir.normalize();

  auto ldotn = lightDir.dot(hitRecord.norm);

  // In direction away from surface
  auto refl = -1 * lightDir + 2 * ldotn * hitRecord.norm;

  auto specular = std::pow(std::max(0.0, refl.dot(-1 * dir)), shininess);
  specular /= ldotn;

  auto phi = colour + ks * specular;
  auto atten = light.falloff[0] + light.falloff[1]*r + light.falloff[2]*r*r;
  atten = ldotn / atten;

  return phi * lightColour * atten;
}
