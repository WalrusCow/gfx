#pragma once

#include "algebra.hpp"

class Ray;
class HitRecord;

class Primitive {
 public:
  virtual ~Primitive() = default;
  virtual bool intersects(const Ray& ray,
                          HitRecord* hitRecord,
                          const Matrix4x4& inverseTransform) = 0;
};

class Sphere : public Primitive {
  bool intersects(const Ray& ray,
                  HitRecord* hitRecord,
                  const Matrix4x4& inverseTransform) override;
};

class Cube : public Primitive {
  bool intersects(const Ray& ray,
                  HitRecord* hitRecord,
                  const Matrix4x4& inverseTransform) override;
};

class NonhierSphere : public Primitive {
 public:
  NonhierSphere(const Point3D& pos, double radius)
    : m_pos(pos), m_radius(radius) {}
  bool intersects(const Ray& ray,
                  HitRecord* hitRecord,
                  const Matrix4x4& inverseTransform) override;

 private:
  Point3D m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
 public:
  NonhierBox(const Point3D& pos, double size) : m_pos(pos), m_size(size) {}
  bool intersects(const Ray& ray,
                  HitRecord* hitRecord,
                  const Matrix4x4& inverseTransform) override;

 private:
  Point3D m_pos;
  double m_size;
};
