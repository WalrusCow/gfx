#include "scene.hpp"

#include <iostream>

#include "xform.hpp"

SceneNode::SceneNode(const std::string& name) : m_name(name) {}

void SceneNode::rotate(char axis, double angle) {
  if (axis == 'x')
    set_transform(m_trans * xRotationMatrix(angle));
  else if (axis == 'y')
    set_transform(m_trans * yRotationMatrix(angle));
  else if (axis == 'z')
    set_transform(m_trans * zRotationMatrix(angle));
}

void SceneNode::scale(const Vector3D& amount) {
  set_transform(m_trans * scaleMatrix(amount[0], amount[1], amount[2]));
}

void SceneNode::translate(const Vector3D& amount) {
  set_transform(m_trans * translationMatrix(amount[0], amount[1], amount[2]));
}

bool SceneNode::is_joint() const {
  return false;
}

JointNode::JointNode(const std::string& name) : SceneNode(name) {
}

bool JointNode::is_joint() const {
  return true;
}

void JointNode::set_joint_x(double min, double init, double max) {
  jointX.min = min;
  jointX.init = init;
  jointX.max = max;
}

void JointNode::set_joint_y(double min, double init, double max) {
  jointY.min = min;
  jointY.init = init;
  jointY.max = max;
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name), m_primitive(primitive) {
}
