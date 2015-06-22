#pragma once

#include <string>
#include "algebra.hpp"
#include "scene.hpp"

class Light;
class Ray;
class ViewConfig;

void a4_render(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const ViewConfig& viewConfig,
               // Lighting parameters
               const Colour& ambient,
               const std::list<Light*>& lights);

Colour rayColour(const Ray& ray,
    const std::list<Light*>& lights,
    int x, int y, int w, int h,
    SceneNode* scene);

Colour backgroundColour(int, int, int, int);
