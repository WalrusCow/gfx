#include "a4.hpp"

#include <cmath>

#include "image.hpp"
#include "PixelTransformer.hpp"
#include "ViewConfig.hpp"

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
               const std::list<Light*>& lights) {

  PixelTransformer pixelTransformer(width, height, viewConfig);

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      // Get world coordinates of pixel (x, y)
      auto worldPx = pixelTransformer.transform(x, y);
      std::cerr << worldPx << std::endl;
      //auto r = Ray(x, y, eye, view, up, fov)
    }
  }

  for (auto I = lights.begin(); I != lights.end(); ++I) {
    if (I != lights.begin()) std::cerr << ", ";
    std::cerr << **I;
  }
  std::cerr << "});" << std::endl;

  // For now, just make a sample image.

  Image img(width, height, 3);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < height; x++) {
      // Red: increasing from top to bottom
      img(x, y, 0) = (double)y / height;
      // Green: increasing from left to right
      img(x, y, 1) = (double)x / width;
      // Blue: in lower-left and upper-right corners
      img(x, y, 2) = ((y < height/2 && x < height/2)
                      || (y >= height/2 && x >= height/2)) ? 1.0 : 0.0;
    }
  }
  img.savePng(filename);

}
