#include "Light.hpp"

Light::Light() : colour(0.0, 0.0, 0.0), position(0.0, 0.0, 0.0) {
  falloff[0] = 1.0;
  falloff[1] = 0.0;
  falloff[2] = 0.0;
}
