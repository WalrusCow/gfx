#pragma once

#include "algebra.hpp"

// Represents a simple point light.
struct Light {
  Light();

  Colour colour;
  Point3D position;
  double falloff[3];
};
