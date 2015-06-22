#pragma once

#include "algebra.hpp"

class Primitive {
public:
  virtual ~Primitive() = default;
};

class Sphere : public Primitive {
};

class Cube : public Primitive {
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const Point3D& pos, double radius)
    : m_pos(pos), m_radius(radius) {}

private:
  Point3D m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const Point3D& pos, double size) : m_pos(pos), m_size(size) {}

private:
  Point3D m_pos;
  double m_size;
};
