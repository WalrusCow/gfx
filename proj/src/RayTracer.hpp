#pragma once

#include <list>
#include <string>
#include "algebra.hpp"

class Light;
class Ray;
class SceneNode;
class ViewConfig;

class RayTracer {
 public:
  void render(SceneNode* root,
              const std::string& filename,
              int width, int height,
              const ViewConfig& viewConfig,
              const Colour& ambient,
              const std::list<Light*>& lights);

 private:
  Colour rayColour(const Ray& ray,
                   const std::list<Light*>& lights,
                   int x, int y, int w, int h,
                   const Colour& ambientColour,
                   SceneNode* scene);

  Colour backgroundColour(int x, int y, int width, int height);
};

