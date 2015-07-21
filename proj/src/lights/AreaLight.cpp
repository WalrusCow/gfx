#include "AreaLight.hpp"

#include <cmath>

AreaLight::AreaLight(
    double rad, Colour&& c, Point3D&& posn, std::array<double, 3> falloff)
    : Light(std::move(c), std::move(posn), std::move(falloff)), radius(rad) {
}

std::vector<Point3D> AreaLight::getPoints(size_t n) const {
  std::vector<Point3D> points;
  // theta is to 2PI; phi is to PI.
  // For even sampling, we do twice as many in theta as phi.
  // Thus for i theta, j phi we have j = i/2.
  // Also, ij = n so i = sqrt(2n)
  double dtheta = (2 * M_PI) / std::sqrt((double)(2 * n));
  double dphi = dtheta / 2;
  for (double theta = 0; theta < 2 * M_PI; theta += dtheta) {
    for (double phi = 0; phi < M_PI; phi += dphi) {
      const double x = position[0] + radius * std::cos(theta) * std::sin(phi);
      const double y = position[1] + radius * std::sin(theta) * std::sin(phi);
      const double z = position[2] + radius * std::cos(phi);
      points.emplace_back(x, y, z);
    }
  }
  return points;
}
