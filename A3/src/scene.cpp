#include "scene.hpp"
#include <iostream>

#include "Viewer.hpp"

SceneNode::SceneNode(const std::string& name) : m_name(name) {}

SceneNode::~SceneNode() {}

void SceneNode::walk_gl(Viewer* viewer, bool picking) const {
  viewer->pushWalkMatrix(m_trans);
  for (auto& child : children) {
    child->walk_gl(viewer, picking);
  }
  viewer->popWalkMatrix();
}

void SceneNode::rotate(char axis, double angle) {
  QMatrix4x4 xform;
  if (axis == 'x') {
    xform.rotate(angle, 1, 0, 0);
  }
  else if (axis == 'y') {
    xform.rotate(angle, 0, 1, 0);
  }
  else if (axis == 'z') {
    xform.rotate(angle, 0, 0, 1);
  }
  set_transform(xform * m_trans);
}

void SceneNode::scale(const QVector3D& amount) {
  QMatrix4x4 xform;
  xform.scale(amount);
  set_transform(m_trans * xform);
}

void SceneNode::translate(const QVector3D& amount) {
  QMatrix4x4 xform;
  xform.translate(amount);
  set_transform(m_trans * xform);
}

bool SceneNode::is_joint() const {
  return false;
}

JointNode::JointNode(const std::string& name) : SceneNode(name) {
}

JointNode::~JointNode() {
}

void JointNode::walk_gl(Viewer* viewer, bool picking) const {
  SceneNode::walk_gl(viewer, picking);
}

bool JointNode::is_joint() const {
  return true;
}

void JointNode::set_joint_x(double min, double init, double max) {
  m_joint_x.min = min;
  m_joint_x.init = init;
  m_joint_x.max = max;
}

void JointNode::set_joint_y(double min, double init, double max) {
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
    : SceneNode(name), m_primitive(primitive) {
}

GeometryNode::~GeometryNode() {
}

void GeometryNode::walk_gl(Viewer* viewer, bool picking) const {
  viewer->pushWalkMatrix(m_trans);
  m_primitive->draw(viewer, m_material, picking);
  for (auto& child : children) {
    child->walk_gl(viewer, picking);
  }
  viewer->popWalkMatrix();
}
