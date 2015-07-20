#include "Light.hpp"

Light::Light(
    Colour&& colour_, Point3D&& position_, std::array<double, 3> falloff_)
      : position(std::move(position_)),
        colour(std::move(colour_)),
        falloff(std::move(falloff_)) {}

Colour Light::getColour() const {
  return colour;
}

std::vector<Point3D> Light::getPoints(size_t n) const {
  (void) n; // Warnings
  return {position};
}

double Light::getFalloff(double dist) const {
  return falloff[0] + falloff[1]*dist + falloff[2]*dist*dist;
}
