#include "Cube.hpp"

#include "algebra.hpp"
#include "HitRecord.hpp"
#include "polyroots.hpp"
#include "Ray.hpp"

using namespace primitives;

bool Cube::intersects(
    const Ray& ray, HitRecord* hitRecord, const Matrix4x4& inverseTransform) {
  // New ray
  const auto a = inverseTransform * ray.start;
  const auto b = inverseTransform * ray.other;
  const auto dir = b - a;

  // Ohkay
  double t = solveIntersection(a, dir);
  if (t < 0) return false;

  auto localPoint = a + t * dir;

  // We will be on some face, and that coordinate will be ~ 0 or 1
  Vector3D localNorm(0, 0, 0);
  auto getNormCoord = [&] (int c) {
    if (isEqual(localPoint[c], 1.0)) {
      localNorm[c] = 1;
    }
    else if (isZero(localPoint[c])) {
      localNorm[c] = -1;
    }
  };
  for (int c = 0; c < 3; ++c) getNormCoord(c);

  // t is unchanged by this
  // Remember to use the *original* intersection point
  auto point = ray.at(t);

  // To get the appropriate normal vector, we must also transpose
  // the inverse transform
  auto norm = inverseTransform.transpose() * localNorm;
  norm.normalize();

  // We will return whether or not this intersection was
  // better than whatever was already stored there
  return hitRecord->update(norm, point, t);
}

double Cube::solveIntersection(const Point3D& a, const Vector3D& dir) {
  int count = 0;
  double minT = -1;
  for (const auto& face : {f1, f2, f3, f4, f5, f6}) {
    // Check intersection with each face
    // We will use a parametric eqn for a square
    // p0 + (p1 - p0)r + (p3 - p0)s = p
    // 0 <= r <= 1; 0 <= s <= 1
    const auto& p0 = face[0];
    const auto& p1 = face[1];
    const auto& p3 = face[3];

    Vector3D col1 = p1 - p0;
    Vector3D col2 = p3 - p0;
    Vector3D col3 = -dir;
    Vector3D rhs = -p0 + Vector3D(a[0], a[1], a[2]);

    double detA = det(col1, col2, col3);
    if (isZero(detA)) {
      continue;
    }
    double c1 = det(rhs, col2, col3);
    double r = c1 / detA;
    if (r < 0 || r > 1) {
      // Outside of square
      continue;
    }

    double c2 = det(col1, rhs, col3);
    double s = c2 / detA;
    if (s < 0 || s > 1) {
      // Outside of square
      continue;
    }

    double c3 = det(col1, col2, rhs);
    double t = c3 / detA;
    if (t < 0 || isZero(t)) {
      continue;
    }

    // Intersection on this face
    if (count == 0) {
      count = 1;
      minT = t;
    }
    else if (count == 1) {
      // If we have already found an intersection, then we just found the
      // second intersection. There cannot be more than 2, so we can stop.
      count = 2;
      // If the new one is zero or the new one is bigger, do not keep it
      minT = (isZero(t) || minT <= t) ? minT : t;
      break;
    }
  }

  // And discard near zero
  return isZero(minT) ? -1 : minT;
}

Point3D Cube::getMinPoint(const Matrix4x4& inverseTransform) const {
  auto xform = inverseTransform.invert();
  Point3D minPoint(xform * p0);
  for (const auto& p : {p1, p2, p3, p4, p5, p6, p7}) {
    auto k = xform * p;
    minPoint[0] = std::min(minPoint[0], k[0]);
    minPoint[1] = std::min(minPoint[1], k[1]);
    minPoint[2] = std::min(minPoint[2], k[2]);
  }
  return minPoint;
}

Point3D Cube::getMaxPoint(const Matrix4x4& inverseTransform) const {
  auto xform = inverseTransform.invert();
  Point3D maxPoint(xform * p0);
  for (const auto& p : {p1, p2, p3, p4, p5, p6, p7}) {
    auto k = xform * p;
    maxPoint[0] = std::max(maxPoint[0], k[0]);
    maxPoint[1] = std::max(maxPoint[1], k[1]);
    maxPoint[2] = std::max(maxPoint[2], k[2]);
  }
  return maxPoint;
}
