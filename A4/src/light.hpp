#pragma once

#include "algebra.hpp"
#include <iosfwd>

// Represents a simple point light.
struct Light {
  Light();

  Colour colour;
  Point3D position;
  double falloff[3];
};

std::ostream& operator<<(std::ostream& out, const Light& l);
