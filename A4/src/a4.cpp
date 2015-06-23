#include "a4.hpp"

#include <cmath>
#include <cstdlib>

#include "image.hpp"
#include "light.hpp"
#include "material.hpp"
#include "HitRecord.hpp"
#include "PixelTransformer.hpp"
#include "Ray.hpp"
#include "ViewConfig.hpp"

Colour rayColour(const Ray& ray,
                 const std::list<Light*>& lights,
                 int x, int y,
                 int width, int height,
                 const Colour& ambientColour,
                 SceneNode* root) {

  HitRecord hitRecord;
  if (!root->intersects(ray, &hitRecord)) {
    // No intersection - use background colour
    return backgroundColour(x, y, width, height);
  }

  Colour colour(0, 0, 0);

  // Unit direction
  Vector3D direction = ray.dir;
  direction.normalize();

  for (const auto light : lights) {
    // Direction to the light
    auto lightDir = light->position - hitRecord.point;
    HitRecord shadowRecord;
    Ray shadowRay(hitRecord.point, light->position);
    if (!root->intersects(shadowRay, &shadowRecord)) {
      // Only add from light source if nothing is hit first
      colour = colour + hitRecord.material->getColour(
          *light, hitRecord.point, hitRecord.norm, ray.dir);
    }
  }

  return colour + colour * ambientColour;
}

Colour backgroundColour(int x, int y, int width, int height) {
  (void) x;
  // Let's try a simple gradient between two colours
  const Colour top(0.6, 1, 0.9);
  const Colour bottom(1.0, 0.596, 0.9215);

  // What percent we are
  double yp = y / (double) height;

  // Upside down, because png starts top left, I guess
  return bottom + yp * (top + (-1 * bottom));
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
      auto pixelColour = rayColour(
          ray, lights, x, y, width, height, ambient, root);
      img(x, height-1-y, 0) = pixelColour.R();
      img(x, height-1-y, 1) = pixelColour.G();
      img(x, height-1-y, 2) = pixelColour.B();
    }
  }

  // For now, just make a sample image.
  img.savePng(filename);
}
