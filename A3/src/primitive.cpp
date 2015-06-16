#include "primitive.hpp"

#include <iostream>
#include <QColor>

#include "material.hpp"
#include "Viewer.hpp"

Primitive::~Primitive() {}

Sphere::~Sphere() {}

void Sphere::draw(Viewer* viewer, bool picking) const {
  viewer->drawSphere(viewer->getWalkMatrix(), picking);
}
