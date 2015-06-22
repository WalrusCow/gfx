#pragma once

#include <string>
#include "algebra.hpp"
#include "scene.hpp"
#include "light.hpp"

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
