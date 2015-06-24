#include "mesh.hpp"

#include <iostream>

#include "HitRecord.hpp"
#include "Ray.hpp"

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector<std::vector<int>>& faces)
    : m_verts(verts), m_faces(faces) {
}

bool Mesh::faceIntersection(
    const Ray& ray, HitRecord* hitRecord, const Mesh::Face& face) {
  // Check if ray intersects this face...
  // First, get the point where the ray intersects
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
  norm.normalize();
  // Update if this is a better t value
  return hitRecord->update(norm, planePt, t);
}

bool Mesh::intersects(const Ray& ray,
                HitRecord* hitRecord,
                const Matrix4x4& inverseTransform) {
  // New ray
  const auto a = inverseTransform * ray.start;
  const auto b = inverseTransform * ray.other;
  const Ray newRay(a, b);

  // Note: This is not implemented correctly, since faceIntersection
  // checks if the face was intersected *and* was a better t value
  bool hit = false;
  // For each polygon, check for polygon intersection lol...
  for (const auto& face : m_faces) {
    if (faceIntersection(newRay, hitRecord, face)) {
      // haha, use real ray
      hitRecord->point = ray.at(hitRecord->t);
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
