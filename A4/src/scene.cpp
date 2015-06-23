#include "scene.hpp"

#include <iostream>

#include "xform.hpp"
#include "HitRecord.hpp"
#include "Ray.hpp"

SceneNode::SceneNode(const std::string& name) : m_name(name) {}

void SceneNode::rotate(char axis, double angle) {
  if (axis == 'x')
    set_transform(trans * xRotationMatrix(angle));
  else if (axis == 'y')
    set_transform(trans * yRotationMatrix(angle));
  else if (axis == 'z')
    set_transform(trans * zRotationMatrix(angle));
}

void SceneNode::scale(const Vector3D& amount) {
  std::cerr << "Scale: " << amount << std::endl;
  std::cerr << trans << std::endl;
  set_transform( scaleMatrix(amount[0], amount[1], amount[2]) * trans);
  std::cerr << trans << std::endl;
}

void SceneNode::translate(const Vector3D& amount) {
  std::cerr << "Translate: " << amount << std::endl;
  std::cerr << trans << std::endl;
  set_transform( translationMatrix(amount[0], amount[1], amount[2]) * trans);
  std::cerr << trans << std::endl;
}

bool SceneNode::intersects(
    const Ray& ray, HitRecord* hitRecord, const Matrix4x4& inverseTransform) {
  auto inverse = inverseTrans * inverseTransform;
  bool hit = false;
  for (auto child : children) {
    if (child->intersects(ray, hitRecord, inverse)) {
      hit = true;
    }
  }
  return hit;
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

bool GeometryNode::intersects(
    const Ray& ray, HitRecord* hitRecord, const Matrix4x4& inverseTransform) {
  // Check if our primitive intersects anything
  auto inverse = inverseTrans * inverseTransform;

  bool hit = false;
  if (primitive->intersects(ray, hitRecord, inverse)) {
    // It's a pointer
    hitRecord->material = material;
    hit = true;
  }

  return SceneNode::intersects(ray, hitRecord, inverseTransform) || hit;
}
