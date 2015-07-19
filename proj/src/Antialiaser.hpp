#pragma once

#include "algebra.hpp"

class Image;
class RayTracer;

class Antialiaser {
 public:
  Antialiaser(const RayTracer* rt_, const Image* image_, double dist=0.02)
      : rt(rt_), image(image_), maxDistance(dist) {}
  Colour antialias(unsigned x, unsigned y);

 private:
  const RayTracer* const rt;
  const Image* const image;
  const double maxDistance;

  bool shouldAntialias(const std::vector<Colour>& colours);
};
