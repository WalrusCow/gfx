#pragma once

#include <random>

#include "Light.hpp"

// Area light. A sphere with radius r and given centre (posn)
class AreaLight : public Light {
 public:
  AreaLight(double radius_, Vector3D&& normal_,
            Colour&& c, Point3D&& posn, std::array<double, 3> falloff);

  std::vector<Point3D> getPoints(size_t n) const override;

 private:
  // For random
  static thread_local std::default_random_engine generator;
  static thread_local std::uniform_real_distribution<double> distribution;

  const double radius;
  const Vector3D normal;

  double getRandom() const;
};
