#include "scene.hpp"
#include <iostream>

#include "Viewer.hpp"

int SceneNode::nextId = 1;

SceneNode::SceneNode(const std::string& name) : m_name(name) {
  id = nextId;
  nextId += 1;
}

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
  if (id == this->id && parent && parent->parent && parent-parent->is_joint()) {
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
      r = id & 0x000000ff;
      g = (id & 0x0000ff00) >> 8;
      b = (id & 0x00ff0000) >> 16;
    }
    std::cerr <<m_name<< " unique color "<<r<<','<<g<<','<<b<<" from id " << id<<std::endl;
    viewer->setDiffuseColour({r, g, b});
  }
  m_primitive->draw(viewer, picking);

  for (auto& child : children) {
    child->walk_gl(viewer, picking);
  }
  viewer->popWalkMatrix();
}
