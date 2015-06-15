#include "primitive.hpp"

#include <iostream>
#include <QColor>

#include "Viewer.hpp"

Primitive::~Primitive() {}

Sphere::~Sphere() {}

void Sphere::walk_gl(Viewer* viewer, bool picking) const {
  std::cerr << "Sphere walking gl" << std::endl;
  viewer->drawSphere(viewer->getWalkMatrix(), QColor(1.0, 0, 0));
}
