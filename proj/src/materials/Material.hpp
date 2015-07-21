#pragma once

#include <random>

#include "algebra.hpp"

class HitRecord;
class Light;

// It's a phong material kind of world
class Material {
 public:
  Material(const Colour& ks_, double shininess_, double alpha_, double idx_);
  virtual ~Material() = default;

  Colour getColour(const HitRecord& hitRecord) const;

  Colour lightColour(const Colour& colour, // Colour at point
                     const Vector3D& dir, // View direction
                     const Point3D& lightPosition, // Position on light
                     const Light& light, // Light to light with
                     const HitRecord& hitRecord) const;
  bool isSpecular() const;
  bool isTransparent() const;
  Colour specularColour() const;
  double getAlpha() const;
  double getRefractionIndex() const;

  std::vector<Vector3D> getReflectedRays(
      const Vector3D& dir, const Vector3D& norm, size_t numRays) const;

 protected:
  virtual Colour getKd(const HitRecord& hitRecord) const = 0;

 private:
  const Colour ks;
  const double shininess;
  const double alpha;
  const double refractionIndex;

  // For random
  static thread_local std::default_random_engine generator;
  static thread_local std::uniform_real_distribution<double> distribution;

  double getRandom() const;
};
