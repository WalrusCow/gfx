#include "Material.hpp"

#include <algorithm>
#include <cmath>
#include "lights/Light.hpp"
#include "HitRecord.hpp"
#include "xform.hpp"

// Random generator and distribution
thread_local
std::default_random_engine Material::generator;
thread_local
std::uniform_real_distribution<double> Material::distribution(0, 1);

Material::Material(const Colour& ks_, double shininess_,
                   double alpha_, double idx_)
                   : ks(ks_), shininess(shininess_),
                   alpha(alpha_), refractionIndex(idx_) {}

Colour Material::getColour(const HitRecord& hitRecord) const {
  return getKd(hitRecord);
}

Colour Material::lightColour(const Colour& colour,
                             const Vector3D& dir,
                             const Point3D& lightPoint,
                             const Light& light,
                             const HitRecord& hitRecord) const {
  // Direction to the light (away from surface)
  auto lightDir = lightPoint - hitRecord.point;
  Colour lightColour = light.getColour();

  auto r = lightDir.length();
  lightDir.normalize();

  auto ldotn = lightDir.dot(hitRecord.norm);

  // In direction away from surface
  auto refl = -1 * lightDir + 2 * ldotn * hitRecord.norm;

  auto specular = std::pow(std::max(0.0, refl.dot(-1 * dir)), shininess);
  specular /= ldotn;

  auto phi = colour + ks * specular;
  auto atten = light.getFalloff(r);
  atten = ldotn / atten;

  return phi * lightColour * atten;
}

bool Material::isSpecular() const {
  return ks.R() > 0 || ks.G() > 0 || ks.B() > 0;
}

bool Material::isTransparent() const {
  return alpha < 1;
}

Colour Material::specularColour() const {
  return ks;
};

double Material::getAlpha() const {
  return alpha;
}

double Material::getRefractionIndex() const {
  return refractionIndex;
}

std::vector<Vector3D> Material::getReflectedRays(
    const Vector3D& dir, const Vector3D& norm, size_t numRays) const {
  // Reflected ray
  auto rr = reflect(dir, norm);
  if (numRays <= 1) {
    // No glossy reflection
    return {rr};
  }

  auto exp = 1 / (shininess + 1);

  std::vector<Vector3D> reflectedRays;
  reflectedRays.resize(numRays);

  // Matrix to move to Z axis
  auto toZ = toZAxis(rr);
  auto fromZ = toZ.invert();

  auto rrZ = toZ * rr;

  // Give several perturbations of the reflected ray
  for (size_t i = 0; i < numRays; ++i) {
    auto alpha = std::pow(std::acos(getRandom()), exp);
    auto beta = 2 * M_PI * getRandom();
    // We want to perturb up by alpha and around by beta
    // We will be on the z axis, so that is around X to go up
    // and around Z to go around
    // P is perturbation matrix
    auto P = fromZ * xRotationMatrix(alpha) * zRotationMatrix(beta);
    reflectedRays.emplace_back(P * rrZ);
  }
  return reflectedRays;
}

double Material::getRandom() const {
  return distribution(generator);
}
