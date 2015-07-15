#include "mesh.hpp"

#include <iostream>

#include "HitRecord.hpp"
#include "Ray.hpp"
#include "xform.hpp"

bool Mesh::interpolateNormals = true;

Mesh::Mesh(std::vector<Point3D>&& verts,
           std::vector<Vector3D>&& normals,
           const FaceInput& faces)
     : m_verts(std::move(verts)),
       m_normals(std::move(normals)), m_faces(getFaces(faces)) {

  if (normals.size() == 0) {
    // TODO
    // interpolate for each vertex (totally rounded)
  }

  Point3D smallPoint(m_verts[0][0], m_verts[0][1], m_verts[0][2]);
  Point3D largePoint(smallPoint);

  // Get the minimum and maximum coordinates
  for (const auto& vert : m_verts) {
    for (auto i = 0; i < 3; ++i) {
      largePoint[i] = std::max(largePoint[i], vert[i]);
      smallPoint[i] = std::min(smallPoint[i], vert[i]);
    }
  }
  Vector3D boundsRange = largePoint - smallPoint;
  // Enforce a minimum size (e.g. for planes)
  // TODO: Epsilon?
  double MIN_SIZE = 0.2;
  for (auto i = 0; i < 3; ++i) {
    boundsRange[i] = std::max(boundsRange[i], MIN_SIZE);
  }

  // Translate to put lower corner at origin
  Matrix4x4 xlate = translationMatrix(
      -smallPoint[0], -smallPoint[1], -smallPoint[2]);
  // And scale down to get unit cube
  Matrix4x4 scale = scaleMatrix(
      1/boundsRange[0], 1/boundsRange[1], 1/boundsRange[2]);
  boundingCubeInverse = scale * xlate;
}

bool Mesh::faceIntersection(
    const Ray& ray, HitRecord* hitRecord, const Mesh::Face& face) {
  // Get a point on the plane
  Vector3D norm = face.normal;

  const auto rayNorm = ray.dir.dot(norm);

  // Parallel
  if (isZero(rayNorm)) return false;

  const auto& p0 = face.vertices.front().vertex();
  const double t = (p0 - ray.start).dot(norm) / rayNorm;
  // No intersection
  if (t < 0 || isZero(t)) {
    return false;
  }

  // Intersection point
  const auto planePt = ray.at(t);
  // Now check if planePt is "left" of everything
  for (size_t i = 0; i < face.vertices.size(); ++i) {
    // Go over points in order
    const auto& p1 = face.vertices[i].vertex();
    const auto& p2 = face.vertices[(i + 1) % face.vertices.size()].vertex();
    // from p1 to p2
    const auto side = p2 - p1;
    // cross from p1 to plane pt and dot against normal
    auto k = norm.dot(side.cross(planePt - p1));
    if (!isZero(k) && k < 0) {
      // Zero means on the side; negative means opposite dir from norm
      return false;
    }
  }

  if (interpolateNormals) {
    norm = interpolatedNormal(face, planePt);
  }

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

  HitRecord boundingRecord;

  // First, check for intersection against our bounding box
  if (!boundingCube.intersects(
        ray, &boundingRecord, boundingCubeInverse * inverseTransform)) {
    return false;
  }

  // Note: This is not implemented correctly, since faceIntersection
  // checks if the face was intersected *and* was a better t value
  bool hit = false;
  // For each polygon, check for polygon intersection lol...
  for (const auto& face : m_faces) {
    if (faceIntersection(newRay, hitRecord, face)) {
      // Use real ray
      hitRecord->point = ray.at(hitRecord->t);
      hitRecord->norm = inverseTransform.transpose() * hitRecord->norm;
      hitRecord->norm.normalize();
      hit = true;
    }

  }
  return hit;
}

Vector3D Mesh::interpolatedNormal(const Face& face, const Point3D& pt) {
  // Get the normal interpolated across the face
  if (face.vertices.size() != 3) {
    // Cannot
    return face.normal;
  }

  // Get the total area of this triangle
  const auto& v1 = face.vertices[0];
  const auto& v2 = face.vertices[1];
  const auto& v3 = face.vertices[2];

  auto v23 = v2.vertex() - v3.vertex();
  auto v21 = v2.vertex() - v1.vertex();
  auto v2p = v2.vertex() - pt;
  auto v13 = v1.vertex() - v3.vertex();
  auto v1p = v1.vertex() - pt;

  // NOTE: Normally these areas should be halved, but we are only using them
  // for ratios so they would cancel out
  double totalArea = (v23).cross(v21).length() / 2;

  double a1 = v23.cross(v2p).length() / 2;
  double a2 = v13.cross(v1p).length() / 2;
  double a3 = v21.cross(v2p).length() / 2;

  return (a1 / totalArea) * v1.normal() +
         (a2 / totalArea) * v2.normal() +
         (a3 / totalArea) * v3.normal();
}

std::vector<Mesh::Face> Mesh::getFaces(const Mesh::FaceInput& faceInput) const {
  std::vector<Mesh::Face> faces;
  for (const auto& face : faceInput) {
    std::vector<FaceVertex> vertices;
    for (const auto& vertex : face) {
      if (vertex.size() == 1) {
        vertices.emplace_back(this, vertex[0]);
      }
      else if (vertex.size() == 2) {
        vertices.emplace_back(this, vertex[0], vertex[1]);
      }
      else {
        std::cerr << "ERROR: Mesh has a bad face!" << std::endl;
      }
    }

    const auto& p0 = vertices.front().vertex();
    const auto& p1 = vertices[1].vertex();
    const auto& p2 = vertices.back().vertex();
    // Norm for whole face (used sometimes)
    const auto norm = (p1 - p0).cross(p2 - p0);

    faces.emplace_back(std::move(vertices), norm);
  }

  return faces;
}

const Point3D& Mesh::FaceVertex::vertex() const {
  return parent->m_verts[m_vertex];
}

const Vector3D& Mesh::FaceVertex::normal() const {
  return parent->m_normals[m_normal];
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
    for (const auto& val : face.vertices) {
      if (j++)
        out << ", ";
      out << val.vertex();
    }
    out << "]";
  }

  out << "});" << std::endl;
  return out;
}
