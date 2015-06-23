#include "material.hpp"

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
  dir.normalize();
  // Direction *to* the light
  auto lightDirection = light.position - pt;
  Colour lightColour = light.colour;
  lightDirection.normalize();
  auto reflection = lightDirection + -2 * lightDirection.dot(norm) * norm;

  // Note: dir is backwards viewerDirection
  Vector3D cv(0, 0, 0);
  Colour diffuseIllumination = std::max(0.0, lightDirection.dot(norm)) * m_kd;
  Colour specularIllumination = std::pow(std::max(0.0, reflection.dot(dir)),
                                         m_shininess) * m_ks;
  return lightColour * diffuseIllumination + lightColour * specularIllumination;
}
