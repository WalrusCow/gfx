#include "RayTracer.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <thread>

#include "Antialiaser.hpp"
#include "image.hpp"
#include "scene.hpp"
#include "HitRecord.hpp"
#include "PixelTransformer.hpp"
#include "primitives/Mesh.hpp"
#include "Ray.hpp"
#include "ViewConfig.hpp"

RayTracer::RayTracer(SceneNode* root,
                     uint32_t width_, uint32_t height_,
                     ViewConfig viewConfig_,
                     Colour ambient_,
                     std::list<Light*> lights_,
                     const RayTracer::Options& options_) :
    imageWidth(width_), imageHeight(height_),
    viewConfig(std::move(viewConfig_)),
    ambientColour(std::move(ambient_)),
    lights(std::move(lights_)),
    tempImage(width_ + 1, height_ + 1, 3),
    finalImage(width_, height_, 3),
    options(options_),
    pixelTransformer(rayWidth(), rayHeight(), viewConfig_) {

  // This is such a horrible hack
  Mesh::interpolateNormals = options_.phongInterpolation;

  // Initialize image data to black
  for (uint32_t y = 0; y < imageHeight + 1; ++y) {
    for (uint32_t x = 0; x < imageWidth + 1; ++x) {
      for (auto i = 0; i < 3; ++i)
        tempImage(x, y, i) = 0;
    }
  }

  minPoint = Point3D(1e20, 1e20, 1e20);
  maxPoint = Point3D(-1e20, -1e20, -1e20);
  extractModels(root);

  // Maybe one day, if we decide to use a model for area lights
  //for (Light* light : lights) {
  //  extremize(&minPoint, light->position,
  //            [] (double a, double b) { return std::min(a,b); });
  //  extremize(&maxPoint, light->position,
  //            [] (double a, double b) { return std::max(a,b); });
  //}

  if (options.uniformGrid) {
    uniformGrid = std::make_unique<UniformGrid>(
        models, minPoint, maxPoint, options.uniformGridSizeFactor);
  }
}

Colour RayTracer::rayColour(const Ray& ray, double x, double y) const {
  HitRecord hitRecord;
  if (!getIntersection(ray, &hitRecord)) {
    // No intersection - use background colour
    return backgroundColour(x, y);
  }

  Colour colour(0, 0, 0);

  // Unit direction
  Vector3D direction = ray.dir;
  direction.normalize();

  const Material* material = hitRecord.material;
  Colour materialColour(material->getColour(hitRecord));

  for (const auto light : lights) {
    const auto lightPoints = light->getPoints(options.shadowSamples);
    for (const auto& lightPoint : lightPoints) {
      Ray shadowRay(hitRecord.point, lightPoint);
      HitRecord r;
      if (!getIntersection(shadowRay, &r)) {
        // Only add from light source if nothing is hit first
        auto litColour = material->lightColour(
            materialColour, direction, lightPoint, *light, hitRecord);
        colour = colour + (litColour / lightPoints.size());
      }
    }
  }

  return colour + materialColour * ambientColour;
}

Colour RayTracer::backgroundColour(double x, double y) const {
  (void) x;
  // Let's try a simple gradient between two colours
  const Colour top(0.6, 1, 0.9);
  const Colour bottom(1.0, 0.596, 0.9215);

  // What percent we are
  double yp = y / (double) rayHeight();

  // Upside down, because png starts top left, I guess
  return bottom + yp * (top + (-1 * bottom));
}

void RayTracer::extractModels(SceneNode* root) {
  extractModels(root, Matrix4x4());
}

void RayTracer::extractModels(SceneNode* root, const Matrix4x4& inverse) {
  auto inv = root->inverseTrans * inverse;
  for (const auto& child : root->children) {
    extractModels(child, inv);
  }

  if (auto* node = dynamic_cast<GeometryNode*>(root)) {
    models.emplace_back(node->primitive, node->material, inv);

    // While we're at it, let's also get the extreme points of the scene
    extremize(&minPoint, node->primitive->getMinPoint(inv),
              [] (double a, double b) { return std::min(a,b); });
    extremize(&maxPoint, node->primitive->getMaxPoint(inv),
              [] (double a, double b) { return std::max(a,b); });
  }
}

void RayTracer::writePixel(uint32_t x, uint32_t y, const Colour& colour) {
  auto xPx = x / options.sampleRateX;
  auto yPx = y / options.sampleRateY;

  auto pixelColour = colour / (options.sampleRateX * options.sampleRateY);
  tempImage(xPx, yPx, 0) += pixelColour.R();
  tempImage(xPx, yPx, 1) += pixelColour.G();
  tempImage(xPx, yPx, 2) += pixelColour.B();
}

void RayTracer::render(const std::string& filename) {
  // Threading
  std::list<std::thread> threads;
  for (uint32_t id = 0; id < options.threadCount; ++id) {
    threads.emplace_back(&RayTracer::threadWork, this, id);
  }

  // Wait for them all to finish
  for (auto& thread : threads) {
    thread.join();
  }

  // Now we have the temporary image, we need to get the real deal.
  // Adaptive anti-aliasing techniques up in this.
  Antialiaser antialiaser(this, &tempImage,
                          options.aaTolerance, options.aaDepth);

  for (unsigned y = 0; y < imageHeight; ++y) {
    for (unsigned x = 0; x < imageHeight; ++x) {
      Colour finalColour = antialiaser.antialias(x, y);

      finalImage(x, y, 0) = finalColour.R();
      finalImage(x, y, 1) = finalColour.G();
      finalImage(x, y, 2) = finalColour.B();
    }
  }

  finalImage.savePng(filename);
}

Colour RayTracer::pixelColour(double x, double y) const {
  // Invert y
  y = rayHeight() - 1 - y;
  auto worldCoords = pixelTransformer.transform(x, y);

  Ray ray(viewConfig.eye, worldCoords);
  return rayColour(ray, x, y);
}

void RayTracer::threadWork(uint32_t id) {
  const auto threadCount = options.threadCount;
  const auto w = rayWidth();
  const auto h = rayHeight();
  for (uint32_t y = 0; y < h; ++y) {
    for (uint32_t x = (y + id) % options.threadCount; x < w; x += threadCount) {
      writePixel(x, y, pixelColour(x, y));
    }
  }
}

bool RayTracer::getIntersection(const Ray& ray, HitRecord* hitRecord) const {
  if (options.uniformGrid) {
    return uniformGridIntersection(ray, hitRecord);
  }
  return basicIntersection(ray, hitRecord);
}

bool
RayTracer::uniformGridIntersection(const Ray& ray, HitRecord* hitRecord) const {
  bool hitModel = false;
  for (const auto* model : uniformGrid->getModels(ray)) {
    if (model->intersects(ray, hitRecord)) {
      hitModel = true;
    }
  }
  return hitModel;
}

bool RayTracer::basicIntersection(const Ray& ray, HitRecord* hitRecord) const {
  bool hitModel = false;
  for (const auto& model : models) {
    if (model.intersects(ray, hitRecord)) {
      hitModel = true;
    }
  }
  return hitModel;
}

void RayTracer::extremize(Point3D* dest, const Point3D& data,
                          std::function<double(double, double)> extreme) const {
  (*dest)[0] = extreme((*dest)[0], data[0]);
  (*dest)[1] = extreme((*dest)[1], data[1]);
  (*dest)[2] = extreme((*dest)[2], data[2]);
}
