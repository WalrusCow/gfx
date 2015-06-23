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
    //std::cerr << "Two lines are " << p1-p0 << " and " << p2-p0 <<std::endl;
    //std::cerr << "norm " << norm << std::endl;
    auto rayNorm = ray.dir.dot(norm);

    //std::cerr << "Ray norm " << rayNorm << std::endl;
    // Parallel
    if (isZero(rayNorm)) return false;

    t = (p0 - ray.start).dot(norm) / rayNorm;
    // No intersection
    if (t < 0 || isZero(t)) {
      //std::cerr << "bad t " << t << std::endl;
      return false;
    }
  }

  // Intersection point
  auto planePt = ray.at(t);
  // Now check if planePt is "left" of everything
  for (auto i = 0; i < face.size(); ++i) {
    // Go over points in order
    const auto& p1 = m_verts[face[i]];
    const auto& p2 = m_verts[face[(i + 1) % face.size()]];
    // from p1 to p2
    const auto side = p2 - p1;
    //std::cerr << "Checking against line " <<side << std::endl;
    // cross from p1 to plane pt and dot against normal
    auto k = norm.dot(side.cross(planePt - p1));
    //std::cerr << "cross product " <<side.cross(planePt - p1) << std::endl;
    if (!isZero(k) && k < 0) {
      //std::cerr << "outside " << k << std::endl;
      // Zero means on the side; negative means opposite dir from norm
      return false;
    }
  }

  // We got it.
  norm.normalize();
  // Update if this is a better t value
  hitRecord->update(norm, planePt, t);
  // True that we did hit this face
  return true;
}

bool Mesh::intersects(const Ray& ray,
                HitRecord* hitRecord,
                const Matrix4x4& inverseTransform) {
  // New ray
  const auto a = inverseTransform * ray.start;
  const auto b = inverseTransform * ray.other;
  const Ray newRay(a, b);

  int intersections = 0;
  // For each polygon, check for polygon intersection lol...
  for (const auto& face : m_faces) {
    if (faceIntersection(newRay, hitRecord, face)) {
      // Yay
      intersections += 1;
    }

    if (intersections >= 2) {
      // We are convex
      break;
    }
  }
  return intersections > 0;
}

bool Mesh::fastIntersects(const Ray& ray,
                          const Matrix4x4& inverseTransform) {
  HitRecord r;
  intersects(ray, &r, inverseTransform);
  // lol epsilon here should be larger, I guess...
  return r.t >= 0.01;
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
