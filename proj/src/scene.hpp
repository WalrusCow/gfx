#pragma once

#include <list>

#include "algebra.hpp"
#include "materials/Material.hpp"
#include "primitives/Primitive.hpp"

class SceneNode {
 public:
  SceneNode(const std::string& name);
  virtual ~SceneNode() = default;

  const Matrix4x4& get_transform() const { return trans; }
  const Matrix4x4& get_inverse() const { return inverseTrans; }

  void set_transform(const Matrix4x4& m) {
    trans = m;
    inverseTrans = m.invert();
  }

  void set_transform(const Matrix4x4& m, const Matrix4x4& i) {
    trans = m;
    inverseTrans = i;
  }

  void add_child(SceneNode* child) {
    children.push_back(child);
  }

  void remove_child(SceneNode* child) {
    children.remove(child);
  }

  // Callbacks to be implemented.
  // These will be called from Lua.
  void rotate(char axis, double angle);
  void scale(const Vector3D& amount);
  void translate(const Vector3D& amount);

  // Hierarchy
  typedef std::list<SceneNode*> ChildList;
  ChildList children;

  std::string m_name;

  // Transformations
  Matrix4x4 trans;
  Matrix4x4 inverseTrans;
};

class JointNode : public SceneNode {
 public:
  JointNode(const std::string& name);

  void set_joint_x(double min, double init, double max);
  void set_joint_y(double min, double init, double max);

  struct JointRange {
    double min, init, max;
  };

 protected:
  JointRange jointX, jointY;
};

class GeometryNode : public SceneNode {
 public:
  GeometryNode(const std::string& name, Primitive* primitive);

  const Material* get_material() const;
  Material* get_material();

  void set_material(Material* material) {
    this->material = material;
  }

  Material* material;
  Primitive* primitive;
};
