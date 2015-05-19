#pragma once

#include <QOpenGLBuffer>
#include <QMatrix4x4>

class Cube {
 public:
  Cube(const QMatrix4x4& xform) : transform(xform) {}
  // Draw me onto OpenGL
  void draw(QOpenGLBuffer& colourBuffer) const;

  // We need the 6 face colours and a transform matrix
  QMatrix4x4 transform;

 private:
  // 6 rgbs, one for each face
  float faceColours[6][3];
};
