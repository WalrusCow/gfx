#pragma once

#include <vector>

#include "algebra.hpp"
#include "Cube.hpp"
#include "Primitive.hpp"

class Ray;
class HitRecord;

// A polygonal mesh.
class Mesh : public Primitive {
  typedef std::vector<std::vector<std::vector<int>>> FaceInput;
 public:
  Mesh(std::vector<Point3D>&& verts,
       std::vector<Vector3D>&& normals,
       const FaceInput& faces);

  bool intersects(const Ray& ray,
                  HitRecord* hitRecord,
                  const Matrix4x4& inverseTransform) const override;
  Point3D getMinPoint(const Matrix4x4& inverseTransform) const override;
  Point3D getMaxPoint(const Matrix4x4& inverseTransform) const override;

  std::vector<Point3D> getBoundingBox(const Matrix4x4& inverseTransform)
      const override;

  static bool interpolateNormals;

 private:
  class FaceVertex {
   public:
    FaceVertex(const Mesh* parent_, int vertex)
        : parent(parent_), m_vertex(vertex) {}
    FaceVertex(const Mesh* parent_, int vertex, int normal)
        : parent(parent_), m_vertex(vertex), m_normal(normal) {}

    const Point3D& vertex() const;
    const Vector3D& normal() const;

    int vertexId() const;

   private:
    const Mesh* const parent;
    const int m_vertex;
    const int m_normal = -1;
  };

  struct Face {
    Face(std::vector<FaceVertex>&& vertices_, const Vector3D& normal_,
         Matrix4x4&& toZAxis_, Point3D&& rectStart_, Vector3D&& rectSize_)
        : vertices(std::move(vertices_)), normal(normal_),
          toZAxis(std::move(toZAxis_)), rectStart(std::move(rectStart_)),
          rectSize(std::move(rectSize_)) {}
    const std::vector<FaceVertex> vertices;
    const Vector3D normal;

    // Info for texture mapping. How to get a percentage of (x, y) in the
    // containing rectangle of this polygon.
    const Matrix4x4 toZAxis;
    // These are kept as 3D b/c of better support, but are really 2D
    const Point3D rectStart;
    const Vector3D rectSize;
  };

  const std::vector<Point3D> m_verts;
  const std::vector<Face> m_faces;
  const std::vector<Vector3D> m_normals;

  Cube boundingCube;
  Matrix4x4 boundingCubeInverse;

  bool faceIntersection(
      const Ray& ray, HitRecord* hitRecord, const Face& face) const;

  std::vector<Face> getFaces(const FaceInput& faces) const;
  std::vector<Vector3D> getNormals(std::vector<Vector3D>&& normals) const;

  Vector3D interpolatedNormal(const Face& face, const Point3D& pt) const;
  void getXYPercent(
      const Face& face, const Point3D& pt, double* xp, double* yp) const;
};
