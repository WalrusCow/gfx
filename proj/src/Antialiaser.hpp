#pragma once

#include <vector>
#include <memory>

#include "algebra.hpp"

class Image;
class RayTracer;

class Antialiaser {
 public:
  Antialiaser(const RayTracer* rt_, const Image* image_, double dist=.02)
      : rt(rt_), image(image_), maxDistance(dist) {}
  Colour antialias(unsigned x, unsigned y) const;

 private:
  const RayTracer* const rt;
  const Image* const image;
  const double maxDistance;
  const int maxDepth = 0;

  struct Pixel {
    // A pixel consists of a top-left point, a distance and four colours, which
    // are at the corners.  0---1
    // See the diagram -->  |   |
    // of colour locations  2---3
    Pixel(double x_, double y_, std::vector<Colour>&& colours_)
        : x(x_), y(y_), colours(std::move(colours_)) {}
    const double x;
    const double y;
    const std::vector<Colour> colours;
    Colour colour() const;
  };

  bool shouldAntialias(const Pixel& pixel) const;
  Colour getColour(const Pixel& pixel, int depth = 0) const;
};
