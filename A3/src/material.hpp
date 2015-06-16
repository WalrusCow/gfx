#pragma once

#include <QColor>

class Viewer;

class Material {
 public:
  virtual ~Material() {};
  virtual void apply(Viewer* viewer) const = 0;

 protected:
  Material() {}
};

class PhongMaterial : public Material {
 public:
  PhongMaterial(const QColor& kd, const QColor& ks, double shininess);

  void apply(Viewer* viewer) const override;

 private:
  QColor m_kd;
  QColor m_ks;

  double m_shininess;
};
