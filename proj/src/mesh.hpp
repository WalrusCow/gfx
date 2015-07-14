#pragma once

#include <vector>
#include <iosfwd>
#include "Primitive.hpp"
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

  struct Face {
    Face(const std::vector<int>& vertices_, const Vector3D& normal_)
        : vertices(vertices_), normal(normal_) {}
    std::vector<int> vertices;
    Vector3D normal;
  };

private:
  const std::vector<Point3D> m_verts;
  const std::vector<Face> m_faces;
  Cube boundingCube;
  Matrix4x4 boundingCubeInverse;

  friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
  bool faceIntersection(
      const Ray& ray, HitRecord* hitRecord, const Face& face);
  // Take as input a ray transformed into our model space
  bool withinBounds(const Ray& ray, HitRecord* hitRecord);

  static std::vector<Face> getFaces(
      const std::vector<Point3D>& verts,
      const std::vector<std::vector<int>>& faces);
};
