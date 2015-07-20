#include "AreaLight.hpp"

#include <cmath>

// Random generator and distribution
thread_local std::default_random_engine AreaLight::generator;
thread_local std::uniform_real_distribution<double> AreaLight::distribution(0, 1);

AreaLight::AreaLight(
    double rad, Colour&& c, Point3D&& posn, std::array<double, 3> falloff)
    : Light(std::move(c), std::move(posn), std::move(falloff)), radius(rad) {
}

std::vector<Point3D> AreaLight::getPoints(size_t n) const {
  std::vector<Point3D> points;
  for (; n > 0; n--) {
    const double theta = getRandom() * 2 * M_PI;
    const double phi = getRandom() * M_PI;
    const double x = position[0] + radius * std::cos(theta) * std::sin(phi);
    const double y = position[1] + radius * std::sin(theta) * std::sin(phi);
    const double z = position[2] + radius * std::cos(phi);
    points.emplace_back(x, y, z);
  }
  return points;
}

double AreaLight::getRandom() const {
  return distribution(generator);
}
