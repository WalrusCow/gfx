#pragma once

#include <QOpenGLBuffer>
#include <QMatrix4x4>

class Cube {
 public:
  // Take a matrix to position this cube and an array of {r, g, b} floats
  Cube(const QMatrix4x4& xform, float* colour);
  // Draw me onto OpenGL
  void draw(QOpenGLBuffer& colourBuffer) const;

  // Set all faces to a single colour
  void useColour(float r, float g, float b);
  // Randomize each face colour, ensuring no duplicates
  void randomizeColours();

  // We need the 6 face colours and a transform matrix
  QMatrix4x4 transform;

 private:
  // 6 rgbs, one for each face
  float faceColours[6][3];
};
