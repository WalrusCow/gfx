#include "UniformGrid.hpp"

#include <cmath>
#include <iostream>

#include "HitRecord.hpp"

UniformGrid::UniformGrid(const std::list<Model>& models,
                         const Point3D& minPoint, const Point3D& maxPoint,
                         uint32_t sizeFactor) {
  sideLength = (int) std::cbrt((double) sizeFactor * models.size());

  double minCoord = std::min({minPoint[0], minPoint[1], minPoint[2]});
  double maxCoord = std::max({maxPoint[0], maxPoint[1], maxPoint[2]});

  startPoint = Point3D(minCoord, minCoord, minCoord);

  double distance = minCoord - maxCoord;
  cellSize = distance / sideLength;
  cells.resize(sideLength * sideLength * sideLength);

  // Now we must populate the cells
  populateCells(models);
}

int UniformGrid::indexFor(const UniformGrid::CellCoord& coord) const {
  return coord.x * sideLength*sideLength + coord.y * sideLength + coord.z;
}

UniformGrid::CellCoord UniformGrid::coordAt(const Point3D& pt) const {
  // Adjust so that the minimum will be at (0, 0, 0)
  Point3D p(pt[0] - cellSize, pt[1] - cellSize, pt[2] - cellSize);
  return CellCoord(
    (int) (p[0] / cellSize),
    (int) (p[1] / cellSize),
    (int) (p[2] / cellSize)
  );
}

UniformGrid::CellCoord UniformGrid::coordAt(int index) const {
  int z = index % sideLength;
  index /= sideLength;
  int y = index % sideLength;
  index /= sideLength;
  int x = index;
  return CellCoord(x, y, z);
}

void UniformGrid::populateCells(const std::list<Model>& models) {
  // Populate the cells with the models
  for (size_t i = 0; i < cells.size(); ++i) {
    for (const auto& model : models) {
      if (intersectsCell(model, coordAt(i))) {
        cells[i].models.push_back(&model);
      }
    }
  }
}

Point3D UniformGrid::pointAt(const UniformGrid::CellCoord& coord) const {
  return startPoint + Vector3D(cellSize * coord.x,
                             cellSize * coord.y,
                             cellSize * coord.z);
}

bool UniformGrid::intersectsCell(const Model& model, const CellCoord& coord) {
  // Left side
  Point3D p0 = pointAt(coord);
  Point3D p1(p0[0], p0[1], p0[2] + cellSize);
  Point3D p2(p0[0], p0[1] + cellSize, p0[2] + cellSize);
  Point3D p3(p0[0], p0[1], p0[2] + cellSize);

  // Right side
  Point3D p4(p0[0] + cellSize, p0[1], p0[2]);
  Point3D p5(p0[0] + cellSize, p0[1], p0[2] + cellSize);
  Point3D p6(p0[0] + cellSize, p0[1] + cellSize, p0[2] + cellSize);
  Point3D p7(p0[0] + cellSize, p0[1], p0[2] + cellSize);

  HitRecord r;
  const std::vector<Point3D> pts = {p1, p2, p3, p4, p5, p6, p7};
  for (size_t i = 0; i < pts.size(); ++i) {
    Vector3D v = pts[(i % 4 == 0) ? i + 3 : i - 1] - pts[i];
    // Note: We are doing pts[i] - v and checking for t between 1 and 2.
    // This is equivalent to checking between 0 and 1 without doing the
    // subtraction, *but* we have an epsilon check in the intersects code.
    // For this case, we do *not* want to bother with epsilon check, so we
    // will check from 1 to 2 to avoid it.
    // Note also that in the case of a grid cell being entirely inside an
    // object this will *not* be true.  That is ok, since a ray can only
    // intersect an object at its edges.
    if (model.intersects(Ray(pts[i] - v, pts[i]), &r) && 1 <= r.t && r.t <= 2) {
      return true;
    }
  }

  // Now we have to check the struts between the two sides
  for (size_t i = 0; i < 4; ++i) {
    Vector3D v = pts[i + 4] - pts[i];
    if (model.intersects(Ray(pts[i] - v, pts[i]), &r) && 1 <= r.t && r.t <= 2) {
      return true;
    }
  }

  return false;
}

std::set<const Model*> UniformGrid::getModels(const Ray& ray) const {
  std::set<const Model*> models;

  // TODO: And if the ray does not begin in the thingy?

  // This is the beginning cell
  auto rayCoord = coordAt(ray.start);
  auto gridStartPoint = pointAt(rayCoord);

  Vector3D dir(
      std::abs(ray.dir[0]), std::abs(ray.dir[1]), std::abs(ray.dir[2]));

  // These values are in units of t (could be negative?)
  double dx = isZero(dir[0]) ? -1 : cellSize / dir[0];
  double dy = isZero(dir[1]) ? -1 : cellSize / dir[1];
  double dz = isZero(dir[2]) ? -1 : cellSize / dir[2];

  // We need to find the minimum t that is non-negative
  double nextX = isZero(dir[0]) ? -1 :
    (gridStartPoint[0] + cellSize - ray.start[0]) / ray.dir[0];
  double nextY = isZero(dir[1]) ? -1 :
    (gridStartPoint[1] + cellSize - ray.start[1]) / ray.dir[1];
  double nextZ = isZero(dir[2]) ? -1 :
    (gridStartPoint[2] + cellSize - ray.start[2]) / ray.dir[2];

  // How to change the cells each time we hit "nextX" or "nextY"
  int xInc = (ray.dir[0] > 0) ? 1 : -1;
  int yInc = (ray.dir[0] > 0) ? 1 : -1;
  int zInc = (ray.dir[0] > 0) ? 1 : -1;

  // Check if a coord is still valid
  auto coordOk = [&] (int coord) -> bool {
    return 0 <= coord && coord < sideLength;
  };

  while (coordOk(rayCoord.x) && coordOk(rayCoord.y) && coordOk(rayCoord.z)) {
    for (const Model* model : cells[indexFor(rayCoord)].models) {
      models.insert(model);
    }

    if ((nextX > 0) && nextX <= nextY && nextX <= nextZ) {
      nextX += dx;
      rayCoord.x += xInc;
    }
    else if ((nextY > 0) && nextY <= nextX && nextY <= nextZ) {
      nextY += dy;
      rayCoord.y += yInc;
    }
    else if ((nextZ > 0) && nextZ <= nextY && nextZ <= nextX) {
      nextZ += dz;
      rayCoord.z += zInc;
    }
    std::cerr << "Processing grid at "<<nextX<<' '<<nextY<<' '<<nextZ<<std::endl;
  }

  return models;
}
