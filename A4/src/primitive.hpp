#pragma once

#include <vector>

#include "algebra.hpp"

class Ray;
class HitRecord;

class Primitive {
 public:
  virtual ~Primitive() = default;
  virtual bool intersects(const Ray& ray,
                          HitRecord* hitRecord,
                          const Matrix4x4& inverseTransform) = 0;
  virtual bool fastIntersects(const Ray& ray,
                              const Matrix4x4& inverseTransform) = 0;
};

class Sphere : public Primitive {
  bool intersects(const Ray& ray,
                  HitRecord* hitRecord,
                  const Matrix4x4& inverseTransform) override;
  bool fastIntersects(const Ray& ray,
                      const Matrix4x4& inverseTransform) override;
 private:
  double solveIntersection(const Point3D& p1, const Vector3D& dir);
};

class Cube : public Primitive {
  bool intersects(const Ray& ray,
                  HitRecord* hitRecord,
                  const Matrix4x4& inverseTransform) override;
  bool fastIntersects(const Ray& ray,
                      const Matrix4x4& inverseTransform) override;
 private:
  // "back" face
  const Point3D p0 = Point3D(0, 0, 0);
  const Point3D p1 = Point3D(1, 0, 0);
  const Point3D p2 = Point3D(1, 1, 0);
  const Point3D p3 = Point3D(0, 1, 0);

  // "front" face
  const Point3D p4 = Point3D(0, 0, 1);
  const Point3D p5 = Point3D(1, 0, 1);
  const Point3D p6 = Point3D(1, 1, 1);
  const Point3D p7 = Point3D(0, 1, 1);
  const std::vector<Point3D> f1 = {p0, p1, p2, p3};
  const std::vector<Point3D> f2 = {p1, p5, p6, p2};
  const std::vector<Point3D> f3 = {p4, p5, p6, p7};
  const std::vector<Point3D> f4 = {p0, p4, p7, p3};
  const std::vector<Point3D> f5 = {p7, p6, p2, p3};
  const std::vector<Point3D> f6 = {p4, p5, p1, p0};

  double solveIntersection(const Point3D& p1, const Vector3D& dir,bool debug);
};
