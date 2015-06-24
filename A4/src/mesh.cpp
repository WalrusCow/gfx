#include "mesh.hpp"

#include <iostream>

#include "HitRecord.hpp"
#include "Ray.hpp"
#include "xform.hpp"

#define BOUNDING_BOX_RENDER false

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector<std::vector<int>>& faces)
    : m_verts(verts), m_faces(faces) {
  Point3D smallPoint(m_verts[0][0], m_verts[0][1], m_verts[0][2]);
  Point3D largePoint(m_verts[0][0], m_verts[0][1], m_verts[0][2]);

  // Get the minimum and maximum coordinates
  for (const auto& vert : m_verts) {
    for (auto i = 0; i < 3; ++i) {
      largePoint[i] = std::max(largePoint[i], vert[i]);
      smallPoint[i] = std::min(smallPoint[i], vert[i]);
    }
  }
  lowerBound = smallPoint;
  boundsRange = largePoint - smallPoint;
  // Enforce a minimum size (e.g. for planes)
  double MIN_SIZE = 0.2;
  for (auto i = 0; i < 3; ++i) {
    boundsRange[i] = std::max(boundsRange[i], MIN_SIZE);
  }
}

bool Mesh::faceIntersection(
    const Ray& ray, HitRecord* hitRecord, const Mesh::Face& face) {
  // Get a point on the plane
  Vector3D norm;
  double t;
  {
    const auto& p0 = m_verts[face[0]];
    const auto& p1 = m_verts[face[1]];
    const auto& p2 = m_verts[face[face.size()-1]];
    norm = (p1 - p0).cross(p2 - p0);
    auto rayNorm = ray.dir.dot(norm);

    // Parallel
    if (isZero(rayNorm)) return false;

    t = (p0 - ray.start).dot(norm) / rayNorm;
    // No intersection
    if (t < 0 || isZero(t)) {
      return false;
    }
  }

  // Intersection point
  auto planePt = ray.at(t);
  // Now check if planePt is "left" of everything
  for (size_t i = 0; i < face.size(); ++i) {
    // Go over points in order
    const auto& p1 = m_verts[face[i]];
    const auto& p2 = m_verts[face[(i + 1) % face.size()]];
    // from p1 to p2
    const auto side = p2 - p1;
    // cross from p1 to plane pt and dot against normal
    auto k = norm.dot(side.cross(planePt - p1));
    if (!isZero(k) && k < 0) {
      // Zero means on the side; negative means opposite dir from norm
      return false;
    }
  }

  // We got it.
  //norm.normalize();
  // Update if this is a better t value
  return hitRecord->update(norm, planePt, t);
}

bool Mesh::withinBounds(const Ray& ray, HitRecord* hitRecord) {
  // We need to transform the ray to our bounding box space now
  Matrix4x4 xlate = translationMatrix(
      lowerBound[0], lowerBound[1], lowerBound[2]).invert();
  Matrix4x4 scale = scaleMatrix(
      boundsRange[0], boundsRange[1], boundsRange[2]).invert();
  auto inverseTransform = xlate * scale;

  // This will update the hit record
  return boundingCube.intersects(ray, hitRecord, inverseTransform);
}

bool Mesh::intersects(const Ray& ray,
                HitRecord* hitRecord,
                const Matrix4x4& inverseTransform) {
  // New ray
  const auto a = inverseTransform * ray.start;
  const auto b = inverseTransform * ray.other;
  const Ray newRay(a, b);

  HitRecord boundingRecord;
  HitRecord* boundingPtr;
  if (BOUNDING_BOX_RENDER) {
    boundingPtr = hitRecord;
  }
  else {
    boundingPtr = &boundingRecord;
  }
  // First, check for intersection against our bounding box
  //if (!withinBounds(newRay, boundingPtr)) {
  //  return false;
  //}

  // Note: This is not implemented correctly, since faceIntersection
  // checks if the face was intersected *and* was a better t value
  bool hit = false;
  // For each polygon, check for polygon intersection lol...
  for (const auto& face : m_faces) {
    if (faceIntersection(newRay, hitRecord, face)) {
      // haha, use real ray
      hitRecord->point = ray.at(hitRecord->t);
      hitRecord->norm = inverseTransform.transpose() * hitRecord->norm;
      hitRecord->norm.normalize();
      hit = true;
    }

  }
  return hit;
}

bool Mesh::fastIntersects(const Ray& ray,
                          const Matrix4x4& inverseTransform) {
  HitRecord r;
  intersects(ray, &r, inverseTransform);
  return r.t > 0 && !isZero(r.t);
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh) {
  out << "mesh({";
  auto i = 0;
  for (const auto& vert : mesh.m_verts) {
    if (i++)
      out << ",\n      ";
    out << vert;
  }
  out << "},\n\n     {";

  i = 0;
  for (const auto& face : mesh.m_faces) {
    if (i++)
      out << ",\n      ";
    out << "[";

    auto j = 0;
    for (const auto& val : face) {
      if (j++)
        out << ", ";
      out << val;
    }
    out << "]";
  }

  out << "});" << std::endl;
  return out;
}
