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
  sideLength = (int) std::cbrt((double) sizeFactor * models.size());

  // Add some padding to avoid edge points
  double PADDING = 0.1;
  double minCoord = std::min({minPoint[0], minPoint[1], minPoint[2]}) - PADDING;
  double maxCoord = std::max({maxPoint[0], maxPoint[1], maxPoint[2]}) + PADDING;

  startPoint = Point3D(minCoord, minCoord, minCoord);

  double distance = maxCoord - minCoord;
  cellSize = distance / sideLength;
  cellSizeScaleMatrix = scaleMatrix(cellSize, cellSize, cellSize);
  gridSizeScaleMatrix = scaleMatrix(distance, distance, distance);

  cells.resize(sideLength * sideLength * sideLength);

  std::clock_t start = std::clock();
  // Now we must populate the cells
  populateCells(models);
  std::cerr << "Populating time: "
            << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000)
            << " ms" << std::endl;
}

int UniformGrid::indexFor(const UniformGrid::CellCoord& coord) const {
  return coord.x * sideLength*sideLength + coord.y * sideLength + coord.z;
}

UniformGrid::CellCoord UniformGrid::coordAt(const Point3D& pt) const {
  // Adjust so that the minimum will be at (0, 0, 0)
  Point3D p(pt[0] - startPoint[0], pt[1] - startPoint[1], pt[2] - startPoint[2]);
  auto c = CellCoord(
    (int) (p[0] / cellSize),
    (int) (p[1] / cellSize),
    (int) (p[2] / cellSize)
  );
  for (int i = 0; i < 3; ++i) {
    if (c[i] >= sideLength) c[i] -= 1;
  }
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
  std::cerr << "Cell size: " << cellSize
            << " and start point " << startPoint << std::endl;
  for (size_t i = 0; i < cells.size(); ++i) {
    for (const auto& model : models) {
      if (intersectsCell(model, coordAt(i))) {
        cells[i].models.push_back(&model);
      }
    }
  }
  std::cerr << "Done populating cells" << std::endl;
}

Point3D UniformGrid::pointAt(const UniformGrid::CellCoord& coord) const {
  return startPoint + Vector3D(cellSize * coord.x,
                             cellSize * coord.y,
                             cellSize * coord.z);
}

bool UniformGrid::intersectsCell(const Model& model, const CellCoord& coord) {
  //std::cerr<<std::endl<<std::endl<<std::endl;
  //std::cerr << "Checking coord " << coord.x << ','<<coord.y<<','<<coord.z<<std::endl;
  // Left side
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
  //std::cerr<<"Cell points are ";
  for (const auto& pt:pts) {
    //std::cerr<<pt<<' ';
  }
  //std::cerr<<std::endl;

  auto cellMat = translationMatrix(p0[0], p0[1], p0[2]) * cellSizeScaleMatrix;
  // But we need the inverse of course
  cellMat = cellMat.invert();

  // Check if a pt is in the cell
  auto inCell = [&] (const Point3D& pt) -> bool {
    return p0[0] <= pt[0] && pt[0] <= (p0[0] + cellSize) &&
           p0[1] <= pt[1] && pt[1] <= (p0[1] + cellSize) &&
           p0[2] <= pt[2] && pt[2] <= (p0[2] + cellSize);
  };

  // First, we need to get the 8 points of the bounding box
  auto bbox = model.getBoundingBox();
  //std::cerr<<"bbox points are ";
  for (const auto& pt:bbox) {
    //std::cerr<<pt<<' ';
  }
  //std::cerr<<std::endl;

  auto inBoundingBox = [&bbox] (const Point3D& pt) -> bool {
    // We are in the box if we are in between all the opposite parallel planes
    const auto c1 = bbox[0]; // Bottom back left corner
    const auto v1a = bbox[1] - c1; // Bottom back left to bottom back right
    const auto v1b = bbox[3] - c1; // Bottom back left to top back left
    const auto v1c = bbox[4] - c1; // Bottom back left to bottom front left
    const auto n1 = v1a.cross(v1b); // Back face
    const auto n2 = v1b.cross(v1c); // Left face
    const auto n3 = v1c.cross(v1a); // Bottom face

    const auto c2 = bbox[6]; // Top front right corner
    const auto v2a = bbox[5] - c2; // Top front right to bottom front right
    const auto v2b = bbox[7] - c2; // Top front right to top front left
    const auto v2c = bbox[2] - c2; // Top front right to top back right
    // We want this to be opposite sign (i.e. not pointing inwards)
    // so we do the opposite cross as above
    const auto n4 = v2b.cross(v2a); // Front face
    const auto n5 = v2c.cross(v2b); // Top face
    const auto n6 = v2a.cross(v2c); // Right face

    //std::cerr << "planes " << n1 <<"||"<<c1<<"  "<<n4<<"||"<<c2;
    if (betweenPlanes(n1, c1, n4, c2, pt)) {
      //std::cerr << " pt " << pt << " is between them!" << std::endl;
    }
    else {
      //std::cerr << " pt " << pt << " not between them!" << std::endl;
    }

    //std::cerr << "planes " << n2 <<"||"<<c1<<"  "<<n6<<"||"<<c2;
    if (betweenPlanes(n2, c1, n6, c2, pt)) {
      //std::cerr << " pt " << pt << " is between them!" << std::endl;
    }
    else {
      //std::cerr << " pt " << pt << " not between them!" << std::endl;
    }

    //std::cerr << "planes " << n3 <<"||"<<c1<<"  "<<n5<<"||"<<c2;
    if (betweenPlanes(n3, c1, n5, c2, pt)) {
      //std::cerr << " pt " << pt << " is between them!" << std::endl;
    }
    else {
      //std::cerr << " pt " << pt << " not between them!" << std::endl;
    }

    return betweenPlanes(n1, c1, n4, c2, pt) &&
           betweenPlanes(n2, c1, n6, c2, pt) &&
           betweenPlanes(n3, c1, n5, c2, pt);
  };

  // A corner of the bbox being inside the cell implies an intersection
  // between the bbox and the cell.
  for (const auto& pt : bbox) {
    if (inCell(pt)) {
      //std::cerr << "pt " << pt << " is inside the cell!" << std::endl;
      return true;
    }
  }

  // Similarly, a corner of cell inside bbox implies intersection
  for (const auto& pt : pts) {
    if (inBoundingBox(pt)) {
      //std::cerr << "pt " << pt << " is in the bounding box" << std::endl;
      return true;
    }
  }

  // Check if any of the 12 lines from bbox intersect this cell
  HitRecord hr;
  for (size_t i = 0; i < 8; ++i) {
    // This is the vector of one edge
    Vector3D v = bbox[(i % 4 == 0) ? i + 3 : i - 1] - bbox[i];
    Ray ray(bbox[i] - v, bbox[i]);
    if (utilityCube.intersects(ray, &hr, cellMat) && 1 <= hr.t && hr.t <= 2) {
      // This edge of the bounding box intersects our cell cube.
      //std::cerr << "the ray from " << ray.start << " to " << ray.dir << " intersects the cell" << std::endl;
      return true;
    }
  }
  for (size_t i = 0; i < 4; ++i) {
    Vector3D v = bbox[i + 4] - bbox[i];
    Ray ray(bbox[i] - v, bbox[i]);
    if (utilityCube.intersects(ray, &hr, cellMat) && 1 <= hr.t && hr.t <= 2) {
      // This edge of the bounding box intersects our cell cube.
      //std::cerr << "the ray from " << ray.start << " to " << ray.dir << " intersects the cell" << std::endl;
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
      if (1 <= hr.t && hr.t <= 2) {
        //std::cerr << "the ray from " << pts[i] << " to " << v << " intersects the cell" << std::endl;
        return true;
      }
    }
  }

  // Now we have to check the struts between the two sides
  for (size_t i = 0; i < 4; ++i) {
    Vector3D v = pts[i + 4] - pts[i];
    if (model.intersects(Ray(pts[i] - v, pts[i]), &hr)) {
      if (1 <= hr.t && hr.t <= 2) {
        //std::cerr << "the ray from " << pts[i] << " to " << v << " intersects the cell" << std::endl;
        return true;
      }
    }
  }

  //std::cerr << "NOT INTERSECTED" << std::endl;
  return false;
}

int count = 0;

std::set<const Model*> UniformGrid::getModels(const Ray& ray) const {
  //1std::cerr<<std::endl<<std::endl<<std::endl;
  //1std::cerr << "Coordinate is " << count / 8 << ','<<count%8<<std::endl;
  //1std::cerr << "This is the " << count << " call" << std::endl;
  //1if (count == 10) {
    //1std::cerr << "This is the one!!" << std::endl;
  //1}
  //1count += 1;
  //1std::cerr << "Getting models for ray " << ray.start << " " << ray.other << std::endl;
  std::set<const Model*> models;

  // TODO: And if the ray does not begin in the thingy?
  // ... then use a Cube the size of the whole grid to determine the
  // intersection t value. That is where the ray will have entered the
  // grid, and we can then use other nice things.
  Point3D nextT(0, 0, 0);

  // The point *within* the grid where the ray first intersected it
  Point3D rayStartPoint(0, 0, 0);

  if (!inGrid(ray.start)) {
    //std::cerr << "Not in the grid?"<<std::endl;
    const auto& sp = startPoint;
    // Not in the grid: We must compute fancy shit!
    auto gridCubeInverse = (translationMatrix(sp[0], sp[0], sp[0]) *
                           gridSizeScaleMatrix).invert();
    HitRecord hr;
    if (!utilityCube.intersects(ray, &hr, gridCubeInverse)) {
      // Does not intersect the grid even
      return models;
    }
    nextT[0] = hr.t;
    nextT[1] = hr.t;
    nextT[2] = hr.t;
    rayStartPoint = ray.at(hr.t);
  }
  else {
    //std::cerr << "It was in the grid." << std::endl;
    rayStartPoint = ray.start;
  }

  // Place in the grid we are currently stepping through
  CellCoord gridCoord = coordAt(rayStartPoint);

  Vector3D dir(
      std::abs(ray.dir[0]), std::abs(ray.dir[1]), std::abs(ray.dir[2]));

  // These values are in units of t: how far we must go to travel a whole cell
  Vector3D dt(
    isZero(dir[0]) ? -1 : cellSize / dir[0],
    isZero(dir[1]) ? -1 : cellSize / dir[1],
    isZero(dir[2]) ? -1 : cellSize / dir[2]
  );
  {
    // The bottom left corner of the cell we are starting in
    Point3D gridStartPoint = pointAt(gridCoord);
    Point3D cellSzPt(cellSize, cellSize, cellSize);
    // We must go to (gridStartPoint + cellVec) in each dir, and we have
    // already travelled by rayStartPoint.
    auto distToTravel = cellSzPt + (gridStartPoint - rayStartPoint);

    // Determine how far, in units of t, we have to go in any direction
    // to reach the next cell
    for (int i = 0; i < 3; ++i) {
      if (isZero(dir[i])) nextT[i] = -1;
      else nextT[i] += distToTravel[i] / dir[i];
    }
  }

  // Which direction in the grid to move when we hit a "next" value
  CellCoord incs(
    (ray.dir[0] > 0) ? 1 : -1,
    (ray.dir[1] > 0) ? 1 : -1,
    (ray.dir[2] > 0) ? 1 : -1
  );

  // Check if a coord is still valid
  auto coordOk = [&] (int coord) -> bool {
    return 0 <= coord && coord < sideLength;
  };
  auto smaller = [] (double a, double b) -> bool {
    return (b < 0) || a <= b;
  };

    //1std::cerr << "Original grid coords " << gridCoord.x<<','<<gridCoord.y<<','<<gridCoord.z<<std::endl;
  while (coordOk(gridCoord.x) && coordOk(gridCoord.y) && coordOk(gridCoord.z)) {
    //1std::cerr<<std::endl;
    //1std::cerr << "Processing " << gridCoord.x<<','<<gridCoord.y<<','<<gridCoord.z<<std::endl;
    for (const Model* model : cells[indexFor(gridCoord)].models) {
      //1std::cerr << "Inserting model hehe" << std::endl;
      models.insert(model);
    }

    for (int i = 0; i < 3; ++i) {
      if (nextT[i] < 0) continue;
      const auto a = nextT[(i + 1) % 3];
      const auto b = nextT[(i + 2) % 3];
      if (smaller(nextT[i], a) && smaller(nextT[i], b)) {
        nextT[i] += dt[i];
        gridCoord[i] += incs[i];
        //1std::cerr << "We got " << i << " as the next one " << nextT[i] -dt[i]<< std::endl;
        break;
      }
      if (i == 2) {
        //1std::cerr << "We should never see this!" << std::endl;
      }
    }
  }

  //1std::cerr << "Returning " << models.size() << " models" <<std::endl;
  return models;
}

bool UniformGrid::inGrid(const Point3D& pt) const {
  const double d = cellSize * sideLength;
  return startPoint[0] <= pt[0] && pt[0] <= startPoint[0] + d &&
         startPoint[1] <= pt[1] && pt[1] <= startPoint[1] + d &&
         startPoint[2] <= pt[2] && pt[2] <= startPoint[2] + d;
}
