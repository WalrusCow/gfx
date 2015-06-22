#include "mesh.hpp"

#include <iostream>

#include "HitRecord.hpp"
#include "Ray.hpp"

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
    : m_verts(verts), m_faces(faces) {
}

bool Mesh::intersects(const Ray& ray,
                HitRecord* hitRecord,
                const Matrix4x4& inverseTransform) {
  return false;
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
