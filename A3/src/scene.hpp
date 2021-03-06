#pragma once

#include <list>
#include <memory>
#include <unordered_map>
#include <QVector3D>
#include <QMatrix4x4>

#include "primitive.hpp"
#include "material.hpp"

class Viewer;
class JointNode;

class SceneNode {
 public:
  SceneNode(const std::string& name);
  virtual ~SceneNode();

  virtual void walk_gl(Viewer* viewer, bool picking = false) const;

  const QMatrix4x4& get_transform() const { return m_trans; }
  const QMatrix4x4& get_inverse() const { return m_invtrans; }

  void set_transform(const QMatrix4x4& m) {
    m_trans = m;
    m_invtrans = m.inverted();
  }

  void set_transform(const QMatrix4x4& m, const QMatrix4x4& i) {
    m_trans = m;
    m_invtrans = i;
  }

  void add_child(SceneNode* child) {
    child->setParent(this);
    children.push_back(std::unique_ptr<SceneNode>(child));
  }

  void remove_child(SceneNode* child) {
    children.remove_if([child] (const std::unique_ptr<SceneNode>& elem) {
      return elem.get() == child;
    });
  }

  int getJointForId(int id);
  int getJointId() const;

  // Callbacks to be implemented.
  // These will be called from Lua.
  void rotate(char axis, double angle);
  void scale(const QVector3D& amount);
  void translate(const QVector3D& amount);

  void setParent(SceneNode* parent) { this->parent = parent; }
  void buildJointMap(std::unordered_map<int, JointNode*>* map);

  // Returns true if and only if this node is a JointNode
  virtual bool is_joint() const;

  SceneNode* parent = nullptr;
  int id;

 protected:
  // Useful for picking
  std::string m_name;

  // Transformations
  QMatrix4x4 m_trans;
  QMatrix4x4 m_invtrans;

  // Hierarchy
  std::list<std::unique_ptr<SceneNode>> children;

 private:
  static int nextId;
};

class JointNode : public SceneNode {
 public:
  JointNode(const std::string& name);
  virtual ~JointNode();

  void walk_gl(Viewer* viewer, bool picking = false) const override;

  virtual bool is_joint() const;

  void set_joint_x(double min, double init, double max);
  void set_joint_y(double min, double init, double max);

  struct JointRange {
    double min, init, max;
  };

  JointRange xJoint, yJoint;
};

class GeometryNode : public SceneNode {
 public:
  GeometryNode(const std::string& name, Primitive* primitive);
  virtual ~GeometryNode();

  void walk_gl(Viewer* viewer, bool picking = false) const override;

  const Material* get_material() const;
  Material* get_material();

  void set_material(Material* material) {
    m_material = material;
  }

 protected:
  Material* m_material;
  Primitive* m_primitive;
};
