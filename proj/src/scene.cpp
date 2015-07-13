#include "scene.hpp"

#include <cmath>
#include <iostream>

#include "xform.hpp"
#include "HitRecord.hpp"
#include "Ray.hpp"

SceneNode::SceneNode(const std::string& name) : m_name(name) {}

void SceneNode::rotate(char axis, double angle) {
  angle = 2*M_PI * angle / 360.0;
  if (axis == 'x' || axis == 'X')
    set_transform(trans * xRotationMatrix(angle));
  else if (axis == 'y' || axis == 'Y')
    set_transform(trans * yRotationMatrix(angle));
  else if (axis == 'z' || axis == 'Z')
    set_transform(trans * zRotationMatrix(angle));
}

void SceneNode::scale(const Vector3D& amount) {
  set_transform(trans * scaleMatrix(amount[0], amount[1], amount[2]));
}

void SceneNode::translate(const Vector3D& amount) {
  set_transform(trans * translationMatrix(amount[0], amount[1], amount[2]));
}

JointNode::JointNode(const std::string& name) : SceneNode(name) {
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
  : SceneNode(name), primitive(primitive) {
}
