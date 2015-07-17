#pragma once

#include "algebra.hpp"

class Light;

class Material {
 public:
  virtual ~Material() = default;

  virtual Colour getColour(const Light& light,
                           const Point3D& pt,
                           const Vector3D& norm,
                           Vector3D dir) const = 0;
  virtual Colour getMainColour() const = 0;

 protected:
  Material() {}
};

class PhongMaterial : public Material {
 public:
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess);

  Colour getColour(const Light& light,
                   const Point3D& pt,
                   const Vector3D& norm,
                   Vector3D dir) const override;
  Colour getMainColour() const override {
    return m_kd;
  }

  Colour m_kd;
  Colour m_ks;

  double m_shininess;
};
