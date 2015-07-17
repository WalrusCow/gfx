#include "Material.hpp"

#include <algorithm>
#include <cmath>
#include "light.hpp"

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess)
  : m_kd(kd), m_ks(ks), m_shininess(shininess) {
}

Colour PhongMaterial::getColour(const Light& light,
                                const Point3D& pt,
                                const Vector3D& norm,
                                Vector3D dir) const {
  // Direction to the light (away from surface)
  auto lightDir = light.position - pt;
  Colour lightColour = light.colour;

  auto r = lightDir.length();
  lightDir.normalize();

  auto ldotn = lightDir.dot(norm);

  // In direction away from surface
  auto refl = -1 * lightDir + 2 * ldotn * norm;

  auto specular = std::pow(std::max(0.0, refl.dot(-1 * dir)), m_shininess);
  specular /= ldotn;

  auto phi = m_kd + m_ks * specular;
  auto atten = light.falloff[0] + light.falloff[1]*r + light.falloff[2]*r*r;
  atten = ldotn / atten;

  return phi * lightColour * atten;
}
