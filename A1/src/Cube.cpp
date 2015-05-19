#include <stdlib.h>
#include <QtOpenGL>

#include "Cube.hpp"

void Cube::draw(QOpenGLBuffer& colourBuffer) const {
  float rgb[3];
  // 6 faces
  for (int i = 0; i < 6; ++i) {
    // rgb values
    for (int j = 0; j < 3; ++j)
      rgb[j] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

    int off = 3 * 6 * i;
    for (int j = 0; j < 6; ++j, off += 3) {
      colourBuffer.write(sizeof(float) * off, &rgb, 3 * sizeof(float));
    }
  }

  glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
}
