#include "Cylinder.hpp"

#include "algebra.hpp"
#include "HitRecord.hpp"
#include "polyroots.hpp"
#include "Ray.hpp"
#include "xform.hpp"

using namespace primitives;

bool Cylinder::intersects(const Ray& ray,
                          HitRecord* hitRecord,
                          const Matrix4x4& inverseTransform) {
  // New ray
  const auto p1 = inverseTransform * ray.start;
  const auto p2 = inverseTransform * ray.other;
  const auto dir = p2 - p1;

  double t = solveIntersection(p1, dir);
  if (t < 0) return false;

  auto localPoint = p1 + t * dir;
  Vector3D localNorm(0, 0, 0);
  if (isZero(localPoint[2] + 1)) {
    localNorm = Vector3D(0, 0, -1);
  }
  else if (isZero(localPoint[2] - 1)) {
    localNorm = Vector3D(0, 0, 1);
  }
  else {
    // Not on one of the tops: Just like a circle
    localNorm = Vector3D(localPoint[0], localPoint[1], 0);
  }

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

double Cylinder::solveIntersection(const Point3D& p1, const Vector3D& dir) {
  auto t = mostValid(getCapT(p1, dir), getWallT(p1, dir));
  return isValid(t) ? t : -1;
}

double Cylinder::getCapT(const Point3D& p1, const Vector3D& dir) {
  // Get the best t for the intersection with the cylinder caps
  double cap1 = -1;
  double cap2 = -1;
  if (!isZero(dir[2])) {
    cap1 = (1 - p1[2]) / dir[2];
    cap2 = (-1 - p1[2]) / dir[2];
  }

  auto inCircle = [&] (double t) -> bool {
    const auto p = (p1 + t * dir);
    return (p[0] * p[0] + p[1] * p[1]) <= 1;
  };

  // Take care of special circle case
  if (!inCircle(cap1)) {
    cap1 = -1;
  }
  if (!inCircle(cap2)) {
    cap2 = -1;
  }

  return mostValid(cap1, cap2);
}

double Cylinder::getWallT(const Point3D& p1, const Vector3D& dir) {
  // Get the best t for the intersection with cylinder walls
  double a = dir[0] * dir[0] + dir[1] * dir[1];
  double b = 2 * (p1[0] * dir[0] + p1[1] * dir[1]);
  double c = p1[0] * p1[0] + p1[1] * p1[1] - 1;

  double roots[2];
  const auto numRoots = quadraticRoots(a, b, c, roots);

  if (numRoots == 0) {
    return -1;
  }

  const auto inCap = [&] (double t) -> bool {
    const auto z = (p1 + t * dir)[2];
    return z <= 1 && z >= -1;
  };

  if (numRoots == 1) {
    // Return root if valid else -1
    return (isValid(roots[0]) && inCap(roots[0])) ? roots[0] : -1;
  }

  // Set to invalid if not within the caps
  for (auto i = 0; i < 2; ++i) {
    if (!inCap(roots[i])) {
      roots[i] = -1;
    }
  }

  return mostValid(roots[0], roots[1]);
}

Point3D Cylinder::getMinPoint(const Matrix4x4& inverseTransform) const {
  auto cubeInv = translationMatrix(-1, -1, -1) * scaleMatrix(2, 2, 2);
  cubeInv = cubeInv.invert();
  return boundingCube.getMinPoint(cubeInv * inverseTransform);
}

Point3D Cylinder::getMaxPoint(const Matrix4x4& inverseTransform) const {
  auto cubeInv = translationMatrix(-1, -1, -1) * scaleMatrix(2, 2, 2);
  cubeInv = cubeInv.invert();
  return boundingCube.getMaxPoint(cubeInv * inverseTransform);
}
