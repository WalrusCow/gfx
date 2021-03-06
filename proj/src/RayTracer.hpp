#pragma once

#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "algebra.hpp"
#include "image.hpp"
#include "lights/Light.hpp"
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
    double aaTolerance = 0.2;
    int aaDepth = 0;
    size_t shadowSamples = 1;
    size_t recursiveDepthLimit = 2;
    size_t glossyReflection = 1;
  };

  RayTracer(SceneNode* root_,
            uint32_t width_, uint32_t height_,
            ViewConfig viewConfig_,
            Colour ambient_,
            std::list<Light*> lights_,
            const Options& options_);

  Colour pixelColour(double x, double y) const;
  void render(const std::string& filename);

 private:
  uint32_t imageWidth, imageHeight;
  ViewConfig viewConfig;
  Colour ambientColour;
  std::list<Light*> lights;
  Image tempImage;
  Image finalImage;
  Options options;
  PixelTransformer pixelTransformer;

  std::list<Model> models;

  // Used to create a bounding box for the scene
  Point3D minPoint;
  Point3D maxPoint;

  std::unique_ptr<UniformGrid> uniformGrid = nullptr;

  // Account for supersampling
  uint32_t rayHeight() const { return (imageHeight + 1) * options.sampleRateY; }
  uint32_t rayWidth() const { return (imageWidth + 1) * options.sampleRateX; }

  void threadWork(uint32_t id);
  void extractModels(SceneNode* root);
  void extractModels(SceneNode* root, const Matrix4x4& inverse);

  Colour rayColour(const Ray& ray, double x, double y,
                   size_t depth = 0,
                   const Colour& rc=Colour(1, 1, 1),
                   double refractionIndex = 1) const;
  Colour backgroundColour(double x, double y) const;

  void writePixel(uint32_t x, uint32_t y, const Colour& colour);
  // Get the intersection of ray with models. Uses a particular implementation.
  bool getIntersection(const Ray& ray, HitRecord* hitRecord) const;

  // Particular implementations of the intersection component
  bool uniformGridIntersection(const Ray& ray, HitRecord* hitRecord) const;
  bool basicIntersection(const Ray& ray, HitRecord* hitRecord) const;

  // Make dest extreme regarding data. Extremize the coords individually,
  // according to the given function
  void extremize(Point3D* dest, const Point3D& data,
                 std::function<double(double, double)> extreme) const;

  std::vector<double> threadPercents;
  std::mutex progressMutex;
  void showProgress(const std::string& msg, double percent) const;
  void showThreadProgress(uint32_t id, double percent);
  std::string getProgressBar(double percent, size_t len) const;
};
