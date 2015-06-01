#pragma once

#include "algebra.hpp"
#include "Movable.hpp"

class ViewPoint : public Movable {
 public:
  ViewPoint();

  Matrix4x4 getViewMatrix() const;
};
