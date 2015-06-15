#pragma once

#include <QColor>

class Material {
 public:
  virtual ~Material();
  virtual void apply_gl() const = 0;

 protected:
  Material() {}
};

class PhongMaterial : public Material {
 public:
  PhongMaterial(const QColor& kd, const QColor& ks, double shininess);
  virtual ~PhongMaterial();

  virtual void apply_gl() const;

 private:
  QColor m_kd;
  QColor m_ks;

  double m_shininess;
};
