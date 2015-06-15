#pragma once

class Viewer;

class Primitive {
 public:
  virtual ~Primitive();
  virtual void walk_gl(Viewer* viewer, bool picking = false) const = 0;
};

class Sphere : public Primitive {
 public:
  virtual ~Sphere();
  void walk_gl(Viewer* viewer, bool picking = false) const override;
};
