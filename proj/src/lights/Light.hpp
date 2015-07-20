#pragma once

#include <array>

#include "algebra.hpp"

// Represents a simple point light.
class Light {
 public:
  Light(Colour&& colour_, Point3D&& position_, std::array<double, 3> falloff_);

  // Return at most n points to use for this light
  virtual std::vector<Point3D> getPoints(size_t n) const;
  Colour getColour() const;
  double getFalloff(double dist) const;

 protected:
  const Point3D position;

 private:
  const Colour colour;
  const std::array<double, 3> falloff;
};
