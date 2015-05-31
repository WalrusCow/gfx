#pragma once

#include "algebra.hpp"

class Line {
 public:
  Line(Point2D start, Point2D end) : start(start), end(end) {
  }
  Point2D start;
  Point2D end;
};

class Model {
 public:
  // A model is composed of a number of lines
  Model(std::vector<Line> lines) : lines(std::move(lines)) {}
  void rotateX(double rad);
  void rotateY(double rad);
  void rotateZ(double rad);

  void translate(double x, double y, double z);

  void scale(double x, double y, double z);

  // We want to expose the transformed lines
  std::vector<Line> getLines() const;

 private:
  void rotate(double rad, const Vector3D& axis);
  // Composed model transformations
  Matrix4x4 modelMatrix;

  // Yay
  Point3D origin;

  Vector3D xAxis = {1, 0, 0};
  Vector3D yAxis = {0, 1, 0};
  Vector3D zAxis = {0, 0, 1};

  std::vector<Line> lines;
};
