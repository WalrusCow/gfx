#include "primitive.hpp"

#include <iostream>
#include <QColor>

#include "material.hpp"
#include "Viewer.hpp"

Primitive::~Primitive() {}

Sphere::~Sphere() {}

void Sphere::draw(Viewer* viewer, Material* material, bool picking) const {
  viewer->drawSphere(viewer->getWalkMatrix(), material->getColour());
}
