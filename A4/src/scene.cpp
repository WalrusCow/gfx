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
  set_transform(trans * scaleMatrix(amount[0], amount[1], amount[2]));
}

void SceneNode::translate(const Vector3D& amount) {
  set_transform(trans * translationMatrix(amount[0], amount[1], amount[2]));
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
  //std::cerr << "Geometry node intersecting"<< std::endl;

  bool hit = false;
  if (primitive->intersects(ray, hitRecord, inverse)) {
    // It's a pointer
    std::cerr << "My material is " << material << std::endl;
    hitRecord->material = material;
    std::cerr << "INTERSECTION!" << std::endl;
    hit = true;
  }

  return SceneNode::intersects(ray, hitRecord, inverseTransform) || hit;
}
