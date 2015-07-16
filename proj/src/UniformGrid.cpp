#include "UniformGrid.hpp"

#include <cmath>
#include <ctime>
#include <iostream>

#include "HitRecord.hpp"
#include "primitives/Cube.hpp"
#include "xform.hpp"

namespace {

// Check if p is between planes with vectors n1 and n2
bool betweenPlanes(const Vector3D& n1, const Point3D& p1,
                   const Vector3D& n2, const Point3D& p2,
                   const Point3D& p) {
  return (n1.dot(p - p1) * n2.dot(p - p2)) <= 0;
}

}

UniformGrid::UniformGrid(const std::list<Model>& models,
                         const Point3D& minPoint, const Point3D& maxPoint,
                         uint32_t sizeFactor) {
  //std::cerr << "Got " << models.size() << " models." << std::endl;
  sideLength = (int) std::cbrt((double) sizeFactor * models.size());

  // Add some padding to avoid edge points
  double PADDING = 0.1;
  double minCoord = std::min({minPoint[0], minPoint[1], minPoint[2]}) - PADDING;
  double maxCoord = std::max({maxPoint[0], maxPoint[1], maxPoint[2]}) + PADDING;

  startPoint = Point3D(minCoord, minCoord, minCoord);

  double distance = maxCoord - minCoord;
  cellSize = distance / sideLength;
  cells.resize(sideLength * sideLength * sideLength);

  std::clock_t start = std::clock();
  // Now we must populate the cells
  populateCells(models);
  std::cerr << "Populating time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;
}

int UniformGrid::indexFor(const UniformGrid::CellCoord& coord) const {
  return coord.x * sideLength*sideLength + coord.y * sideLength + coord.z;
}

UniformGrid::CellCoord UniformGrid::coordAt(const Point3D& pt) const {
  // Adjust so that the minimum will be at (0, 0, 0)
  Point3D p(pt[0] - startPoint[0], pt[1] - startPoint[1], pt[2] - startPoint[2]);
  auto c= CellCoord(

    (int) (p[0] / cellSize),
    (int) (p[1] / cellSize),
    (int) (p[2] / cellSize)
  );
  return c;
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
  std::cerr << "Populating the cells: " << cells.size() << std::endl;
  std::cerr << "have cell size: " << cellSize << " and start point " << startPoint << std::endl;
  for (size_t i = 0; i < cells.size(); ++i) {
    for (const auto& model : models) {
      if (intersectsCell(model, coordAt(i))) {
        //std::cerr << "Putting model into " << c.x <<','<<c.y<<','<<c.z<<'\n';
        cells[i].models.push_back(&model);
      }
    }
  }
  std::cerr << "Done populating cells" << std::endl;
  for (size_t i = 0; i < cells.size(); ++i) {
    if (cells[i].models.size() > 0) {
      auto c = coordAt(i);
      //std::cerr << "Cell "<<c.x<<','<<c.y<<','<<c.z<< " has " << cells[i].models.size() << " models" << std::endl;
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
  //std::cerr<<"Hello"<<std::endl;
  // Bottom left point
  Point3D p0 = pointAt(coord);
  Point3D p1(p0[0], p0[1] + cellSize, p0[2]);
  Point3D p2(p0[0], p0[1] + cellSize, p0[2] + cellSize);
  Point3D p3(p0[0], p0[1], p0[2] + cellSize);

  // Right side
  Point3D p4(p0[0] + cellSize, p0[1], p0[2]);
  Point3D p5(p0[0] + cellSize, p0[1] + cellSize, p0[2]);
  Point3D p6(p0[0] + cellSize, p0[1] + cellSize, p0[2] + cellSize);
  Point3D p7(p0[0] + cellSize, p0[1], p0[2] + cellSize);

  const std::vector<Point3D> pts = {p0, p1, p2, p3, p4, p5, p6, p7};

  Cube cellCube;
  auto cellMat = translationMatrix(p0[0], p0[1], p0[2]) *
                 scaleMatrix(cellSize, cellSize, cellSize);
  // But we need the inverse of course
  cellMat = cellMat.invert();

  // Check if a pt is in the cell
  auto inCell = [&] (const Point3D& pt) -> bool {
    return p0[0] <= pt[0] && pt[0] <= (p0[0] + cellSize) &&
           p0[1] <= pt[1] && pt[1] <= (p0[1] + cellSize) &&
           p0[2] <= pt[2] && pt[2] <= (p0[2] + cellSize);
  };

  // We want to do line-rectangle intersection on a lot of things...
  // So basically we want to check if ANY OF:
  // (1) A face of bounding box (not axis aligned) intersects a face of our cell
  // (2) OR the bounding box has a corner in our cell
  // (3) OR our cell has a corner in the bounding box
  // Idea is that (2) and (3) are ez to check, but (1) is harder. So we will do
  // them first.

  // First, we need to get the 8 points of the bounding box
  auto bbox = model.getBoundingBox();

  //std::cerr << "Cell points: ";
  for (auto p : pts) {
    //std::cerr << p << ' ';
  }
  //std::cerr<<std::endl;
  //std::cerr << "Bounding box points: ";
  for (auto p : bbox) {
    //std::cerr << p << ' ';
  }
  //std::cerr<<std::endl;

  auto inBoundingBox = [&bbox] (const Point3D& pt) -> bool {
    // We are in the box if we are in between all the opposite parallel planes
    static const auto& c1 = bbox[0]; // Bottom back left corner
    static const auto v1a = bbox[1] - c1; // Bottom back left to bottom back right
    static const auto v1b = bbox[3] - c1; // Bottom back left to top back left
    static const auto v1c = bbox[4] - c1; // Bottom back left to bottom front left
    static const auto n1 = v1a.cross(v1b); // Back face
    static const auto n2 = v1b.cross(v1c); // Left face
    static const auto n3 = v1c.cross(v1a); // Bottom face

    static const auto& c2 = bbox[6]; // Top front right corner
    static const auto v2a = bbox[5] - c2; // Top front right to bottom front right
    static const auto v2b = bbox[7] - c2; // Top front right to top front left
    static const auto v2c = bbox[2] - c2; // Top front right to top back right
    // We want this to be opposite sign (i.e. not pointing inwards)
    // so we do the opposite cross as above
    static const auto n4 = v2b.cross(v2a); // Front face
    static const auto n5 = v2c.cross(v2b); // Top face
    static const auto n6 = v2a.cross(v2c); // Right face

    // So the parallel planes are:
    // (n1, n4), (n2, n6), (n3, n5)
    if (betweenPlanes(n1, c1, n4, c2, pt)) {
      //std::cerr << "Point " << pt << " is between front and back" << std::endl;
      //std::cerr << n1 << " " << n4 << std::endl;
      //std::cerr << n1.dot(pt - c1) << " * " << n4.dot(pt - c2) << " <= 0" << std::endl;
    }
    if (betweenPlanes(n2, c1, n6, c2, pt)) {
      //std::cerr << "Point " << pt << " is between left and right" << std::endl;
      //std::cerr << n2 << " " << n6 << std::endl;
      //std::cerr << n2.dot(pt - c1) << " * " << n6.dot(pt - c2) << " <= 0" << std::endl;
    }
    if (betweenPlanes(n3, c1, n5, c2, pt)) {
      //std::cerr << "Point " << pt << " is between top and bottom" << std::endl;
      //std::cerr << n3 << " " << n5 << std::endl;
      //std::cerr << n3.dot(pt - c1) << " * " << n5.dot(pt - c2) << " <= 0" << std::endl;
    }

    return betweenPlanes(n1, c1, n4, c2, pt) &&
           betweenPlanes(n2, c1, n6, c2, pt) &&
           betweenPlanes(n3, c1, n5, c2, pt);
  };

  // A corner of the bbox being inside the cell implies an intersection
  // between the bbox and the cell.
  for (const auto& pt : bbox) {
    if (inCell(pt)) {
      //std::cerr << "The point " << pt << " is in the cell" << std::endl;
      return true;
    }
  }

  // Similarly, a corner of cell inside bbox implies intersection
  for (const auto& pt : pts) {
    if (inBoundingBox(pt)) {
      //std::cerr << "The point " << pt << " is in the bounding box" << std::endl;
      return true;
    }
  }

  // Check if any of the 12 lines from bbox intersect this cell
  HitRecord hr;
  for (size_t i = 0; i < 8; ++i) {
    // This is the vector of one edge
    Vector3D v = bbox[(i % 4 == 0) ? i + 3 : i - 1] - bbox[i];
    Ray ray(bbox[i] - v, bbox[i]);
    if (cellCube.intersects(ray, &hr, cellMat) && 1 <= hr.t && hr.t <= 2) {
      // This edge of the bounding box intersects our cell cube.
      return true;
    }
  }
  for (size_t i = 0; i < 4; ++i) {
    Vector3D v = bbox[i + 4] - bbox[i];
    Ray ray(bbox[i] - v, bbox[i]);
    if (cellCube.intersects(ray, &hr, cellMat) && 1 <= hr.t && hr.t <= 2) {
      // This edge of the bounding box intersects our cell cube.
      return true;
    }
  }

  // Now check if any of the 12 lines from this cell intersect the model
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
    if (model.intersects(Ray(pts[i] - v, pts[i]), &hr)) {
      //std::cerr << "Intersected..." << std::endl;
      if (1 <= hr.t && hr.t <= 2) {
        //std::cerr << "Good t value" << std::endl;
        return true;
      }
    }
  }

  // Now we have to check the struts between the two sides
  for (size_t i = 0; i < 4; ++i) {
    Vector3D v = pts[i + 4] - pts[i];
    if (model.intersects(Ray(pts[i] - v, pts[i]), &hr)) {
      //std::cerr << "Intersected..." << std::endl;
      if (1 <= hr.t && hr.t <= 2) {
        //std::cerr << "Good t value" << std::endl;
        return true;
      }
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
    (gridStartPoint[0] + cellSize - ray.start[0]) / dir[0];
  double nextY = isZero(dir[1]) ? -1 :
    (gridStartPoint[1] + cellSize - ray.start[1]) / dir[1];
  double nextZ = isZero(dir[2]) ? -1 :
    (gridStartPoint[2] + cellSize - ray.start[2]) / dir[2];

  // How to change the cells each time we hit "nextX" or "nextY"
  int xInc = (ray.dir[0] > 0) ? 1 : -1;
  int yInc = (ray.dir[1] > 0) ? 1 : -1;
  int zInc = (ray.dir[2] > 0) ? 1 : -1;

  // Check if a coord is still valid
  auto coordOk = [&] (int coord) -> bool {
    return 0 <= coord && coord < sideLength;
  };
  auto smaller = [] (double a, double b) -> bool {
    return (b < 0) || a <= b;
  };

  //std::cerr << "before Processing at "<<rayCoord.x<<','<<rayCoord.y<<','<<rayCoord.z<<'\n';
  while (coordOk(rayCoord.x) && coordOk(rayCoord.y) && coordOk(rayCoord.z)) {
    //std::cerr << "Processing at "<<rayCoord.x<<','<<rayCoord.y<<','<<rayCoord.z
              //<< " with nexts: " << nextX << ' '<<nextY<<' '<<nextZ<<std::endl;
    for (const Model* model : cells[indexFor(rayCoord)].models) {
      models.insert(model);
    }

    if ((nextX > 0) && smaller(nextX, nextY) && smaller(nextX, nextZ)) {
      ////std::cerr<<"X"<<std::endl;
      nextX += dx;
      rayCoord.x += xInc;
    }
    else if ((nextY > 0) && smaller(nextY, nextX) && smaller(nextY, nextZ)) {
      //std::cerr<<"Y"<<std::endl;
      nextY += dy;
      rayCoord.y += yInc;
    }
    else if ((nextZ > 0) && smaller(nextZ, nextY) && smaller(nextZ, nextX)) {
      //std::cerr<<"Z"<<std::endl;
      nextZ += dz;
      rayCoord.z += zInc;
    }
    else {
      //std::cerr << "None..." << std::endl;
    }
  }

  //std::cerr << "Got " << models.size() << " models" << std::endl;
  return models;
}
