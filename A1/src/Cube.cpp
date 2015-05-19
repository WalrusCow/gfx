#include <stdlib.h>
#include <QtOpenGL>

#include "Cube.hpp"

Cube::Cube(const QMatrix4x4& xform, float* colour) : transform(xform) {
  for (int i = 0; i < 6; ++i) {
    for (int j = 0; j < 3; ++j) {
      faceColours[i][j] = colour[j];
    }
  }
}

void Cube::draw(QOpenGLBuffer& colourBuffer) const {
  // 6 faces
  for (int i = 0; i < 6; ++i) {
    auto c = faceColours[i];

    int off = 3 * 6 * i;
    for (int j = 0; j < 6; ++j, off += 3) {
      colourBuffer.write(sizeof(float) * off, c, 3 * sizeof(float));
    }
  }

  glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
}

void Cube::useColour(float r, float g, float b) {
  for (int i = 0; i < 6; ++i) {
    faceColours[i][0] = r;
    faceColours[i][1] = g;
    faceColours[i][2] = b;
  }
}

void Cube::randomizeColours() {

}
