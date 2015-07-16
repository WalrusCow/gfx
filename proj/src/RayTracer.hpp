#pragma once

#include <functional>
#include <list>
#include <memory>
#include <string>

#include "algebra.hpp"
#include "image.hpp"
#include "light.hpp"
#include "Model.hpp"
#include "PixelTransformer.hpp"
#include "Ray.hpp"
#include "scene.hpp"
#include "UniformGrid.hpp"
#include "ViewConfig.hpp"

class HitRecord;

class RayTracer {
 public:
  struct Options {
    uint32_t sampleRateX = 1;
    uint32_t sampleRateY = 1;
    uint32_t threadCount = 4;
    bool phongInterpolation = false;
    bool uniformGrid = false;
    uint32_t uniformGridSizeFactor = 8;
  };

  RayTracer(SceneNode* root_,
            uint32_t width_, uint32_t height_,
            ViewConfig viewConfig_,
            Colour ambient_,
            std::list<Light*> lights_,
            const Options& options_);

  void render(const std::string& filename);

 private:
  uint32_t imageWidth, imageHeight;
  ViewConfig viewConfig;
  Colour ambientColour;
  std::list<Light*> lights;
  Image image;
  Options options;
  PixelTransformer pixelTransformer;

  std::list<Model> models;

  // Used to create a bounding box for the scene
  Point3D minPoint;
  Point3D maxPoint;

  std::unique_ptr<UniformGrid> uniformGrid = nullptr;

  // Account for supersampling
  uint32_t rayHeight() const { return imageHeight * options.sampleRateY; }
  uint32_t rayWidth() const { return imageWidth * options.sampleRateX; }

  void threadWork(uint32_t id);
  void extractModels(SceneNode* root);
  void extractModels(SceneNode* root, const Matrix4x4& inverse);

  Colour rayColour(const Ray& ray, uint32_t x, uint32_t y);
  Colour backgroundColour(uint32_t x, uint32_t y);

  void writePixel(uint32_t x, uint32_t y, const Colour& colour);
  // Get the intersection of ray with models. Uses a particular implementation.
  bool getIntersection(const Ray& ray, HitRecord* hitRecord);

  // Particular implementations of the intersection component
  bool uniformGridIntersection(const Ray& ray, HitRecord* hitRecord);
  bool basicIntersection(const Ray& ray, HitRecord* hitRecord);

  // Make dest extreme regarding data. Extremize the coords individually,
  // according to the given function
  void extremize(Point3D* dest, const Point3D& data,
                 std::function<double(double, double)> extreme);
};
