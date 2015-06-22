#pragma once

#include <vector>
#include <iosfwd>
#include "primitive.hpp"
#include "algebra.hpp"

class Ray;
class HitRecord;

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh(const std::vector<Point3D>& verts,
       const std::vector<std::vector<int>>& faces);

  bool intersects(const Ray& ray,
                  HitRecord* hitRecord,
                  const Matrix4x4& inverseTransform) override;

  typedef std::vector<int> Face;

private:
  std::vector<Point3D> m_verts;
  std::vector<Face> m_faces;

  friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};
