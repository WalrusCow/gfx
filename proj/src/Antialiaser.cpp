#include "Antialiaser.hpp"

#include <cmath>

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

Colour Antialiaser::Pixel::colour() const {
  return average(colours);
}

bool Antialiaser::shouldAntialias(const Antialiaser::Pixel& pixel) const {
  for (size_t i = 0; i < pixel.colours.size(); ++i) {
    for (size_t j = i + 1; j < pixel.colours.size(); ++j) {
      // If any pair of colours too far apart then we should antialias
      if (distance(pixel.colours[i], pixel.colours[j]) > maxDistance) {
        return true;
      }
    }
  }
  // No pair is too far apart: no need to antialias
  return false;
}

Colour Antialiaser::antialias(unsigned x, unsigned y) const {
  // Antialias it all.
  // First, determine if there is too large a difference
  std::vector<Colour> colours = {
    getPixelColour(*image, x, y),
    getPixelColour(*image, x + 1, y),
    getPixelColour(*image, x, y + 1),
    getPixelColour(*image, x + 1, y + 1),
  };
  return getColour(Antialiaser::Pixel(x, y, std::move(colours)));
}

Colour
Antialiaser::getColour(const Antialiaser::Pixel& pixel, int depth) const {
  // Get colour for a pixel
  if (depth >= maxDepth || !shouldAntialias(pixel)) return pixel.colour();

  // Split into sub-pixels
  std::vector<Antialiaser::Pixel> subPixels;

  double dist = std::pow(0.5, depth);
  Colour top = rt->pixelColour(pixel.x + dist, pixel.y);
  Colour mid = rt->pixelColour(pixel.x + dist, pixel.y + dist);
  Colour left = rt->pixelColour(pixel.x, pixel.y + dist);
  Colour right = rt->pixelColour(pixel.x + 2*dist, pixel.y + dist);
  Colour bottom = rt->pixelColour(pixel.x + dist, pixel.y + 2*dist);

  // Top left
  subPixels.emplace_back(pixel.x, pixel.y, std::vector<Colour>(
                         {pixel.colours[0], top, left, mid}));
  // Top right
  subPixels.emplace_back(pixel.x + dist, pixel.y, std::vector<Colour>(
                         {top, pixel.colours[1], top, mid, right}));
  // Bottom left
  subPixels.emplace_back(pixel.x, pixel.y + dist, std::vector<Colour>(
                         {left, mid, pixel.colours[2], bottom}));
  // Bottom right
  subPixels.emplace_back(pixel.x + dist, pixel.y + dist, std::vector<Colour>(
                         {mid, right, bottom, pixel.colours[3]}));
  // Now we just need to average them all
  std::vector<Colour> colours;
  for (const auto& px : subPixels) {
    colours.emplace_back(getColour(px, depth + 1));
  }

  return average(colours);
}
