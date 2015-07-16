#include "Sphere.hpp"

#include <algorithm>

#include "algebra.hpp"
#include "HitRecord.hpp"
#include "polyroots.hpp"
#include "Ray.hpp"
#include "xform.hpp"

using namespace primitives;

bool Sphere::intersects(const Ray& ray,
                        HitRecord* hitRecord,
                        const Matrix4x4& inverseTransform) const {
  // New ray
  const auto p1 = inverseTransform * ray.start;
  const auto p2 = inverseTransform * ray.other;
  const auto dir = p2 - p1;

  double t = solveIntersection(p1, dir);
  if (t < 0) return false;

  // Since this is a unit sphere, the norm at p is a vector to p
  auto localPoint = p1 + t * dir;
  Vector3D localNorm(localPoint[0], localPoint[1], localPoint[2]);

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

double Sphere::solveIntersection(const Point3D& p1, const Vector3D& dir) const {
  // Now do intersection against unit sphere centred at origin
  // x^2 + y^2 + z^2 = 1
  // We want to check whether or not p.dot(p) == 1 for some p on the line
  // pt on line is p = p1 + t * dir
  // Expand into  (dir^2)*(t^2) + (dir*p1)*2t - 1 = 0
  const auto a = dir.dot(dir);
  // No dot product is defined for points and vectors
  const auto b = 2 * (dir[0]*p1[0] + dir[1]*p1[1] + dir[2]*p1[2]);
  //... or points and points
  const auto c = p1[0]*p1[0] + p1[1]*p1[1] + p1[2]*p1[2] -1;
  double roots[2];

  const auto numRoots = quadraticRoots(a, b, c, roots);

  if (numRoots == 0) {
    // No intersection
    return -1;
  }

  // Only t >= 0 matters (and we should probably discard < EPSILON as well)
  double t = roots[0];
  if (numRoots == 2 && roots[1] > 0 && !isZero(roots[1])) {
    // Take smallest non-negative value
    t = (t < 0) ? roots[1] : std::min(t, roots[1]);
  }
  if (t < 0 || isZero(t)) {
    // No *valid* intersections
    return -1;
  }
  return t;
}

Point3D Sphere::getMinPoint(const Matrix4x4& inverseTransform) const {
  auto cubeInv = translationMatrix(-1, -1, -1) * scaleMatrix(2, 2, 2);
  cubeInv = cubeInv.invert();
  return boundingCube.getMinPoint(cubeInv * inverseTransform);
}

Point3D Sphere::getMaxPoint(const Matrix4x4& inverseTransform) const {
  auto cubeInv = translationMatrix(-1, -1, -1) * scaleMatrix(2, 2, 2);
  cubeInv = cubeInv.invert();
  return boundingCube.getMaxPoint(cubeInv * inverseTransform);
}

std::vector<Point3D>
Sphere::getBoundingBox(const Matrix4x4& inverseTransform) const {
  auto cubeInv = translationMatrix(-1, -1, -1) * scaleMatrix(2, 2, 2);
  cubeInv = cubeInv.invert();
  return boundingCube.getBoundingBox(cubeInv * inverseTransform);
}
