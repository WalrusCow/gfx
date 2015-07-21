#pragma once

#include "Light.hpp"

// Area light. A sphere with radius r and given centre (posn)
class AreaLight : public Light {
 public:
  AreaLight(double rad,
            Colour&& c, Point3D&& posn, std::array<double, 3> falloff);

  std::vector<Point3D> getPoints(size_t n) const override;

 private:
  const double radius;
};
