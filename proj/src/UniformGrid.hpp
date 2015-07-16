#pragma once

#include <list>
#include <set>

#include "algebra.hpp"
#include "Model.hpp"
#include "primitives/Cube.hpp"
#include "Ray.hpp"

class UniformGrid {
 public:
  UniformGrid(const std::list<Model>& models,
              const Point3D& minPoint, const Point3D& maxPoint,
              uint32_t sizeFactor);

  // Get all models that this ray intersects
  std::set<const Model*> getModels(const Ray& ray) const;

 private:
  struct GridCell {
    std::vector<const Model*> models;
  };

  struct CellCoord {
    CellCoord(int x_, int y_, int z_) : x(x_), y(y_), z(z_) {}
    int x, y, z;
    int& operator[](size_t idx) {
      if (idx == 0) return x;
      if (idx == 1) return y;
      if (idx == 2) return z;
    }
    int operator[](size_t idx) const {
      if (idx == 0) return x;
      if (idx == 1) return y;
      if (idx == 2) return z;
    }
  };

  std::vector<GridCell> cells;

  double cellSize;
  // Number of cells on one side (same on x, y, z)
  int sideLength;
  Point3D startPoint;

  // Used for a variety of cube intersections
  Cube utilityCube;
  Matrix4x4 cellSizeScaleMatrix;
  Matrix4x4 gridSizeScaleMatrix;

  // Get index in the vector
  int indexFor(const CellCoord& coord) const;
  // The corresponding cell coordinate
  CellCoord coordAt(const Point3D& pt) const;
  // Get coord from index in vector
  CellCoord coordAt(int index) const;
  // Point in space a cell coordinate corresponds to (bottom left corner)
  Point3D pointAt(const CellCoord& coord) const;
  bool inGrid(const Point3D& pt) const;

  bool intersectsCell(const Model& model, const CellCoord& coord);
  void populateCells(const std::list<Model>& models);
};
