#pragma once

class Material;
class Viewer;

class Primitive {
 public:
  virtual ~Primitive();
  virtual void draw(
      Viewer* viewer, Material* material, bool picking = false) const = 0;
};

class Sphere : public Primitive {
 public:
  virtual ~Sphere();
  void draw(
      Viewer* viewer, Material* material, bool picking = false) const override;
};
