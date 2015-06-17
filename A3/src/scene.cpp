#include "scene.hpp"
#include <iostream>

#include "Viewer.hpp"

int SceneNode::nextId = 1;

SceneNode::SceneNode(const std::string& name) : m_name(name) {
  id = nextId;
  nextId += 1;
}

SceneNode::~SceneNode() {}

void SceneNode::buildJointMap(std::unordered_map<int, JointNode*>* map) {
  for (auto& child : children) {
    if (child->is_joint()) {
      map->insert({child->id, dynamic_cast<JointNode*>(child.get())});
    }
    child->buildJointMap(map);
  }
}

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
  set_transform(m_trans * xform);
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

int SceneNode::getJointForId(int id) {
  if (id == this->id && parent && parent->parent && parent->parent->is_joint()) {
    return parent->parent->id;
  }
  else {
    for (auto& child : children) {
      int i = child->getJointForId(id);
      if (i > 0) {
        return i;
      }
    }
  }
  return 0;
}

int SceneNode::getJointId() const {
  if (parent && parent->parent && parent-parent->is_joint()) {
    return parent->parent->id;
  }
  return 0;
}

JointNode::JointNode(const std::string& name) : SceneNode(name) {
}

JointNode::~JointNode() {
}

void JointNode::walk_gl(Viewer* viewer, bool picking) const {
  auto xRotation = viewer->getXRotationAngle(id);
  auto yRotation = viewer->getYRotationAngle(id);
  xRotation = std::max(xJoint.min, std::min(xJoint.max, xRotation));
  yRotation = std::max(yJoint.min, std::min(yJoint.max, yRotation));

  QMatrix4x4 xRotMat;
  xRotMat.rotate(xRotation, {1, 0, 0});

  QMatrix4x4 yRotMat;
  yRotMat.rotate(yRotation, {0, 1, 0});

  viewer->pushWalkMatrix(m_trans * xRotMat * yRotMat);
  for (auto& child : children) {
    child->walk_gl(viewer, picking);
  }
  viewer->popWalkMatrix();
}

bool JointNode::is_joint() const {
  return true;
}

void JointNode::set_joint_x(double min, double init, double max) {
  xJoint.min = min;
  xJoint.init = init;
  xJoint.max = max;
}

void JointNode::set_joint_y(double min, double init, double max) {
  yJoint.min = min;
  yJoint.init = init;
  yJoint.max = max;
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
    : SceneNode(name), m_primitive(primitive) {
}

GeometryNode::~GeometryNode() {
}

void GeometryNode::walk_gl(Viewer* viewer, bool picking) const {
  viewer->pushWalkMatrix(m_trans);
  if (!picking) {
    m_material->apply(viewer, getJointId());
  }
  else {
    // Geometry nodes are under a containing node which is under
    // a joint node (or not)
    int r, g, b;
    r = g = b = 0;
    if (parent && parent->parent && parent->parent->is_joint()) {
      // Use unique colour
      r = (id*4 & 0x000000ff);
      g = (id*4 & 0x0000ff00) >> 8;
      b = (id*4 & 0x00ff0000) >> 16;
    }
    viewer->setDiffuseColour({r, g, b});
  }
  m_primitive->draw(viewer, picking);

  for (auto& child : children) {
    child->walk_gl(viewer, picking);
  }
  viewer->popWalkMatrix();
}
