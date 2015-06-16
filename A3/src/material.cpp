#include "material.hpp"
#include "Viewer.hpp"

PhongMaterial::PhongMaterial(
    const QColor& kd, const QColor& ks, double shininess)
    : m_kd(kd), m_ks(ks), m_shininess(shininess) {}

void PhongMaterial::apply(Viewer* viewer) const {
  // Perform OpenGL calls necessary to set up this material.
  viewer->setDiffuseColour(m_kd);
  viewer->setSpecularColour(m_ks);
  viewer->setShininess(m_shininess);
}
