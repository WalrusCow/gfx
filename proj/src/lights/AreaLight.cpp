#include "AreaLight.hpp"

// Random generator and distribution
thread_local std::default_random_engine AreaLight::generator;
thread_local std::uniform_real_distribution<double> AreaLight::distribution(0, 1);

AreaLight::AreaLight(double radius_, Vector3D&& normal_,
                     Colour&& c, Point3D&& posn, std::array<double, 3> falloff)
    : Light(std::move(c), std::move(posn), std::move(falloff)),
      radius(radius_), normal(std::move(normal_)) {
}

std::vector<Point3D> AreaLight::getPoints(size_t n) const {
  // TODO: Generate N random points
  return {position};
}

double AreaLight::getRandom() const {
  return distribution(generator);
}
