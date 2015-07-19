#include "Antialiaser.hpp"

#include <vector>

#include "image.hpp"
#include "RayTracer.hpp"

namespace {

Colour getPixelColour(const Image& image, unsigned x, unsigned y) {
  return Colour(
    image(x, y, 0),
    image(x, y, 1),
    image(x, y, 2)
  );
}

// Some notion of distance between colours
double distance(const Colour& c1, const Colour& c2) {
  // Treat as a vector
  const Colour d = c1 - c2;
  return std::sqrt(d.R()*d.R() + d.G()*d.G() + d.B()*d.B());
}

Colour average(const std::vector<Colour>& colours) {
  Colour res(0);
  for (const auto& c : colours) {
    res = res + c;
  }
  return res / colours.size();
}

} // Anonymous

bool Antialiaser::shouldAntialias(const std::vector<Colour>& colours) {
  return false;
  for (size_t i = 0; i < colours.size(); ++i) {
    for (size_t j = i + 1; j < colours.size(); ++j) {
      // If any pair of colours too far apart then we should antialias
      if (distance(colours[i], colours[j]) > maxDistance) {
        return true;
      }
    }
  }
  // No pair is too far apart: no need to antialias
  return false;
}

Colour Antialiaser::antialias(unsigned x, unsigned y) {
  // Antialias it all.
  // First, determine if there is too large a difference
  std::vector<Colour> colours = {
    getPixelColour(*image, x, y),
    getPixelColour(*image, x + 1, y),
    getPixelColour(*image, x, y + 1),
    getPixelColour(*image, x + 1, y + 1),
  };
  if (!shouldAntialias(colours)) {
    return average(colours);
  }
  return Colour(0, 0, 0);
}
