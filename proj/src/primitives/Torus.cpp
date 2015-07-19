#include "Torus.hpp"

#include "algebra.hpp"
#include "HitRecord.hpp"
#include "polyroots.hpp"
#include "Ray.hpp"
#include "xform.hpp"

using namespace primitives;

Torus::Torus(double tubeRadius_) : tubeRadius(tubeRadius_) {
  // Height and width are 2 * (1 + r)
  auto height = 2 * (1 + tubeRadius);
  // Depth is simple 2 * r
  auto depth = 2 * tubeRadius;
  cubeInverse = scaleMatrix(height, height, depth);
  // Move centre of cube to origin
  cubeInverse = translationMatrix(-height/2, -height/2, -depth/2) * cubeInverse;
  cubeInverse = cubeInverse.invert();
}

bool Torus::intersects(const Ray& ray,
                       HitRecord* hitRecord,
                       const Matrix4x4& inverseTransform) const {
  // New ray
  const auto p1 = inverseTransform * ray.start;
  const auto p2 = inverseTransform * ray.other;
  Ray localRay(p1, p2);
  double t = solveIntersection(localRay);
  if (!isValid(t)) return false;

  Point3D localPoint = localRay.at(t);
  Point3D globalPoint = ray.at(t);

  // Get the norm. See: http://www.emeyex.com/site/projects/raytorus.pdf
  double k = Vector3D(localPoint).length2() - tubeRadius*tubeRadius - 1;
  Vector3D localNorm(
    4 * localPoint[0] * k,
    4 * localPoint[1] * k,
    4 * localPoint[2] * k + 8 * localPoint[2]
  );
  auto norm = inverseTransform.transpose() * localNorm;
  norm.normalize();
  return hitRecord->update(norm, globalPoint, t);
}

double Torus::solveIntersection(const Ray& localRay) const {
  // can be up to 4 intersections: a quartic.
  // See: http://www.emeyex.com/site/projects/raytorus.pdf
  double a = localRay.dir.dot(localRay.dir);
  double b = 2 * localRay.dir.dot(localRay.start);
  double c = Vector3D(localRay.start).dot(localRay.start)
             - tubeRadius*tubeRadius - 1;

  double a4 = a*a;
  double a3 = 2*a*b;
  double a2 = b*b + 2*a*c + 4*localRay.dir[2]*localRay.dir[2];
  double a1 = 2*b*c + 8*localRay.start[2]*localRay.dir[2];
  double a0 = c*c + 4*localRay.start[2]*localRay.start[2]
              - 4*tubeRadius*tubeRadius;

  // Our solver requires the x^4 coefficient to be 1
  a3 /= a4;
  a2 /= a4;
  a1 /= a4;
  a0 /= a4;

  double roots[4];
  size_t numRoots = quarticRoots(a3, a2, a1, a0, roots);
  if (numRoots <= 0) {
    return -1;
  }
  double t = roots[0];
  for (size_t i = 2; i <= 4; ++i) {
    if (numRoots >= i) {
      t = mostValid(t, roots[i - 1]);
    }
  }
  return t;
}

Point3D Torus::getMinPoint(const Matrix4x4& inverseTransform) const {
  return boundingCube.getMinPoint(cubeInverse * inverseTransform);
}

Point3D Torus::getMaxPoint(const Matrix4x4& inverseTransform) const {
  return boundingCube.getMaxPoint(cubeInverse * inverseTransform);
}

std::vector<Point3D>
Torus::getBoundingBox(const Matrix4x4& inverseTransform) const {
  return boundingCube.getBoundingBox(cubeInverse * inverseTransform);
}
