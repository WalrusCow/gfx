#include "a4.hpp"

#include <cmath>

#include "image.hpp"
#include "light.hpp"
#include "HitRecord.hpp"
#include "PixelTransformer.hpp"
#include "Ray.hpp"
#include "ViewConfig.hpp"

Colour rayColour(const Ray& ray,
                 const std::list<Light*>& lights,
                 int x, int y,
                 int width, int height,
                 SceneNode* root) {
  HitRecord hitRecord;
  if (!root->intersects(ray, hitRecord)) {
    // No intersection - use background colour
    return backgroundColour(x, y, width, height);
  }
  return Colour(0,0,0);
}

Colour backgroundColour(int x, int y, int width, int height) {
  // Let's try a simple gradient between two colours
  const Colour top(1.0, 0.0, 0.0);
  const Colour bottom(0.0, 0.0, 0.0);

  // What percent we are
  double yp = y / (double) height;

  return top + yp * (bottom + (-1 * top));
}

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

  Image img(width, height, 3);

  PixelTransformer pixelTransformer(width, height, viewConfig);

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      // Get world coordinates of pixel (x, y)
      auto worldPx = pixelTransformer.transform(x, y);

      Ray ray(viewConfig.eye, worldPx);

      // Now check the intersection with every object lmao
      auto pixelColour = rayColour(ray, lights, x, y, width,height,root);
      img(x, y, 0) = pixelColour.R();
      img(x, y, 1) = pixelColour.G();
      img(x, y, 2) = pixelColour.B();
    }
  }

  // For now, just make a sample image.
  img.savePng(filename);

}
