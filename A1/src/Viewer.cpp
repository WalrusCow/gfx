//William McDonald 20418145 wmcdonal
#include <iostream>
#include <math.h>
#include <GL/glu.h>
#include <QtWidgets>
#include <QtOpenGL>
#include <QVector3D>
#include "Viewer.hpp"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

const float Viewer::SCALE_FACTOR = 1.001f;
const float Viewer::MIN_SCALE = pow(SCALE_FACTOR, -1200);
const float Viewer::MAX_SCALE = pow(SCALE_FACTOR, 800);
// Radians to rotate per pixel moved
const float Viewer::ROTATE_FACTOR = M_PI / 15;

// Coordinates for a unit cube
const float Viewer::cubeCoords[12 * 3 * 3] = {
  // Bottom (into screen)
  0.0f, 0.0f, 0.0f, // 1
  0.0f, 1.0f, 0.0f,
  1.0f, 0.0f, 0.0f, // end 1
  1.0f, 1.0f, 0.0f, // 2
  0.0f, 1.0f, 0.0f,
  1.0f, 0.0f, 0.0f, // end 1

  // Top (out of screen)
  0.0f, 0.0f, 1.0f, // 3
  0.0f, 1.0f, 1.0f,
  1.0f, 0.0f, 1.0f, // end 3
  1.0f, 1.0f, 1.0f, // 4
  0.0f, 1.0f, 1.0f,
  1.0f, 0.0f, 1.0f, // end 4

  // Top (y)
  0.0f, 1.0f, 0.0f, // 5
  0.0f, 1.0f, 1.0f,
  1.0f, 1.0f, 0.0f, // end 5
  1.0f, 1.0f, 1.0f, // 6
  0.0f, 1.0f, 1.0f,
  1.0f, 1.0f, 0.0f, // end 6

  // Bottom (y)
  0.0f, 0.0f, 0.0f, // 7
  0.0f, 0.0f, 1.0f,
  1.0f, 0.0f, 0.0f, // end 7
  1.0f, 0.0f, 1.0f, // 8
  0.0f, 0.0f, 1.0f,
  1.0f, 0.0f, 0.0f, // end 8

  // Left (x)
  0.0f, 0.0f, 0.0f, // 9
  0.0f, 0.0f, 1.0f,
  0.0f, 1.0f, 0.0f, // end 9
  0.0f, 1.0f, 1.0f, // 10
  0.0f, 0.0f, 1.0f,
  0.0f, 1.0f, 0.0f, // end 10

  // Right (x)
  1.0f, 0.0f, 0.0f, // 11
  1.0f, 0.0f, 1.0f,
  1.0f, 1.0f, 0.0f, // end 11
  1.0f, 1.0f, 1.0f, // 12
  1.0f, 0.0f, 1.0f,
  1.0f, 1.0f, 0.0f, // end 12
};

// Piece colours by id
const float Viewer::pieceColours[7][3] = {
  {1.0, 0.0, 0.0},
  {0.0, 1.0, 0.0},
  {0.0, 0.0, 1.0},
  {1.0, 1.0, 0.0},
  {0.0, 1.0, 1.0},
  {1.0, 0.0, 1.0},
  {1.0, 1.0, 1.0}
};

const float Viewer::cubeColours[13][3] = {
  {1.00, 0.00, 0.00},
  {1.00, 0.50, 0.00},
  {1.00, 0.75, 0.75},
  {0.50, 0.00, 0.00},
  {0.50, 0.00, 0.50},
  {0.00, 0.00, 1.00},
  {0.00, 1.00, 1.00},
  {0.00, 0.50, 0.50},
  {0.00, 1.00, 0.00},
  {0.00, 0.50, 0.00},
  {0.75, 1.00, 0.75},
  {1.00, 1.00, 0.00},
  {1.00, 0.00, 1.00},
};

Viewer::Viewer(const QGLFormat& format, QWidget *parent)
  : QGLWidget(format, parent),
#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    positionBuffer(QOpenGLBuffer::VertexBuffer),
    colourBuffer(QOpenGLBuffer::VertexBuffer),
    vao(this)
#else
    positionBuffer(QGLBuffer::VertexBuffer),
    colourBuffer(QGLBuffer::VertexBuffer)
#endif
{
  scale = 1.0f;
  drawMode = DrawMode::FACE;
  game = nullptr;
  mTimer = new QTimer(this);
  connect(mTimer, SIGNAL(timeout()), this, SLOT(update()));
  mTimer->start(1000 / 30);
}

Viewer::~Viewer() {

}

void Viewer::setMode(DrawMode mode) {
  drawMode = mode;
  switch (mode) {
    case DrawMode::WIRE:
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      break;
    case DrawMode::FACE:
    case DrawMode::MULTICOLOUR:
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      break;
  }
}

void Viewer::viewGame(Game* game) {
  this->game = game;
}

QSize Viewer::minimumSizeHint() const {
  return QSize(50, 50);
}

QSize Viewer::sizeHint() const {
  return QSize(300, 600);
}

void Viewer::initializeGL() {
  QGLFormat glFormat = QGLWidget::format();
  if (!glFormat.sampleBuffers()) {
    std::cerr << "Could not enable sample buffers." << std::endl;
    return;
  }

  glClearColor(0.7, 0.7, 1.0, 0.0);

  if (!mProgram.addShaderFromSourceFile(QGLShader::Vertex, "shader.vert")) {
    std::cerr << "Cannot load vertex shader." << std::endl;
    return;
  }

  if (!mProgram.addShaderFromSourceFile(QGLShader::Fragment, "shader.frag")) {
    std::cerr << "Cannot load fragment shader." << std::endl;
    return;
  }

  if (!mProgram.link()) {
    std::cerr << "Cannot link shaders." << std::endl;
    return;
  }

  mProgram.bind();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
  vao.create();
  vao.bind();

  positionBuffer.create();
  positionBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);

  colourBuffer.create();
  colourBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
#else
  /*
   * if qt version is less than 5.1, use the following commented code
   * instead of QOpenGLVertexVufferObject. Also use QGLBuffer instead of
   * QOpenGLBuffer.
   */
  uint vaoo;

  typedef void (APIENTRY *_glGenVertexArrays) (GLsizei, GLuint*);
  typedef void (APIENTRY *_glBindVertexArray) (GLuint);

  _glGenVertexArrays glGenVertexArrays;
  _glBindVertexArray glBindVertexArray;

  glGenVertexArrays = (_glGenVertexArrays) QGLWidget::context()
    ->getProcAddress("glGenVertexArrays");
  glBindVertexArray = (_glBindVertexArray) QGLWidget::context()
    ->getProcAddress("glBindVertexArray");

  glGenVertexArrays(1, &vaoo);
  glBindVertexArray(vaoo);

  colourBuffer.create();
  colourBuffer.setUsagePattern(QGLBuffer::StaticDraw);

  positionBuffer.create();
  positionBuffer.setUsagePattern(QGLBuffer::StaticDraw);
#endif


  if (!positionBuffer.bind()) {
    std::cerr << "could not bind vertex buffer to the context." << std::endl;
    return;
  }
  positionBuffer.allocate(cubeCoords, sizeof(cubeCoords));

  mProgram.enableAttributeArray("vert");
  mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);

  if (!colourBuffer.bind()) {
    std::cerr << "could not bind colour buffer to the context." << std::endl;
    return;
  }

  const float allocColours[12 * 3 * 3] = {0};
  colourBuffer.allocate(allocColours, sizeof(cubeCoords));


  mProgram.enableAttributeArray("colour");
  mProgram.setAttributeBuffer("colour", GL_FLOAT, 0, 3);

  mMvpMatrixLocation = mProgram.uniformLocation("mvpMatrix");

  initializeWell();
}

void Viewer::initializeWell() {
}

void Viewer::paintGL() {
  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Don't show faces that are in the back
  glEnable(GL_DEPTH_TEST);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
  vao.bind();
#endif

  if (qApp->mouseButtons() != Qt::NoButton) {
    pRotDx = 0;
  }
  // Keep on rotating, man
  if (pRotDx != 0) {
    rotateWorld(ROTATE_FACTOR * pRotDx, pRotVec);
  }

  auto cameraMatrix = getCameraMatrix();
  QMatrix4x4 wellTransform;
  // Top left
  wellTransform.translate(-6, 10, 0);

  // Negative ids, different from game
  int wellId = -1;

  // 20 deep and 10 across is 21 down, 11 right, 20 up
  float wellColour[3] = {0.25, 0.25, 0.25};

  for (int i = 0; i < 21; ++i) {
    wellTransform.translate(0, -1, 0);
    drawCube(cameraMatrix * wellTransform, wellId, wellColour);
    wellId -= 1;
  }
  for (int i = 0; i < 11; ++i) {
    wellTransform.translate(1, 0, 0);
    drawCube(cameraMatrix * wellTransform, wellId, wellColour);
    wellId -= 1;
  }
  for (int i = 0; i < 20; ++i) {
    wellTransform.translate(0, 1, 0);
    drawCube(cameraMatrix * wellTransform, wellId, wellColour);
    wellId -= 1;
  }

  // Draw in the game
  if (game == nullptr) return;
  QMatrix4x4 gameCubeTransform;

  // Bottom left
  gameCubeTransform.translate(-5, -10, 0);

  int w = game->getWidth();
  int h = game->getHeight() + 4;
  for (int r = 0; r < h; ++r) {
    for (int c = 0; c < w; ++c) {
      const BoardSpace& bs = game->get(r, c);
      if (bs.type >= 0) {
        const float* colour = cubeColours[bs.type];
        drawCube(cameraMatrix * gameCubeTransform, bs.id, colour);
      }
      // Right one
      gameCubeTransform.translate(1, 0, 0);
    }
    // Up one and left
    gameCubeTransform.translate(-10, 1, 0);
  }
}

void Viewer::drawCube(
    const QMatrix4x4& transform, int cubeId, const float* colour) {
  mProgram.setUniformValue(mMvpMatrixLocation, transform);
  if (drawMode == DrawMode::MULTICOLOUR) {
    setCubeColourById(cubeId);
  }
  else {
    setCubeColour(colour);
  }

  glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
}

void Viewer::setCubeColourById(int cubeId) {
  // 6 faces
  for (int i = 0; i < 6; ++i) {
    int off = 3 * 6 * i;
    int idx = (cubeId + 7*i) % 13;
    const float* colour = cubeColours[idx];
    for (int j = 0; j < 6; ++j, off += 3) {
      colourBuffer.write(sizeof(float) * off, colour, 3 * sizeof(float));
    }
  }
}

void Viewer::setCubeColour(const float* colour) {
  // 6 faces
  for (int i = 0; i < 6; ++i) {
    int off = 3 * 6 * i;
    for (int j = 0; j < 6; ++j, off += 3) {
      colourBuffer.write(sizeof(float) * off, colour, 3 * sizeof(float));
    }
  }
}

void Viewer::resizeGL(int width, int height) {
  if (height == 0) {
    height = 1;
  }

  mPerspMatrix.setToIdentity();
  mPerspMatrix.perspective(60.0, (float) width / (float) height, 0.001, 1000);

  glViewport(0, 0, width, height);
}

void Viewer::mousePressEvent (QMouseEvent* event) {
  // When any mouse button is pressed, we stop persistent rotation
  pRotVec.setX(0);
  pRotVec.setY(0);
  pRotVec.setZ(0);
  pRotDx = 0;

  lastMouseX = event->x();
}

void Viewer::mouseReleaseEvent (QMouseEvent* event) {
  (void) event;
  if (!(event->modifiers() & Qt::ShiftModifier)) {
    updateRotVector(event->button(), true);
  }
  lastMouseX = -1;
}

void Viewer::mouseMoveEvent (QMouseEvent* event) {
  int x = event->x();
  int dx = x - lastMouseX;

  if (event->modifiers() & Qt::ShiftModifier) {
    float scaleAmount = pow(SCALE_FACTOR, dx);
    scale *= scaleAmount;
    if (scale > MAX_SCALE) {
      scale = MAX_SCALE;
      scaleAmount = MAX_SCALE / scale;
    }
    else if (scale < MIN_SCALE) {
      scale = MIN_SCALE;
      scaleAmount = MIN_SCALE / scale;
    }
    scaleWorld(scaleAmount, scaleAmount, scaleAmount);
  }
  else {
    auto buttons = event->buttons();
    // 1 if appropriate button is held, else 0
    float xCoord = !!(buttons & Qt::LeftButton);
    float yCoord = !!(buttons & Qt::MidButton);
    float zCoord = !!(buttons & Qt::RightButton);
    rotateWorld(ROTATE_FACTOR * dx, xCoord, yCoord, zCoord);

    pRotDx = dx;
    lastMouseX = x;
  }
}

QMatrix4x4 Viewer::getCameraMatrix() {
  QMatrix4x4 vMatrix;

  QMatrix4x4 cameraTransformation;
  QVector3D cameraPosition = cameraTransformation * QVector3D(0, 0, 20.0);
  QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);

  vMatrix.lookAt(cameraPosition, QVector3D(0, 0, 0), cameraUpDirection);

  return mPerspMatrix * vMatrix * mTransformMatrix;
}

void Viewer::translateWorld(float x, float y, float z) {
  mTransformMatrix.translate(x, y, z);
}

void Viewer::rotateWorld(float angle, const QVector3D& vector) {
  mTransformMatrix.rotate(angle, vector);
}

void Viewer::rotateWorld(float angle, float x, float y, float z) {
  mTransformMatrix.rotate(angle, x, y, z);
}

void Viewer::scaleWorld(float x, float y, float z) {
  mTransformMatrix.scale(x, y, z);
}

void Viewer::updateRotVector(Qt::MouseButton button, bool release) {
  switch (button) {
    case Qt::LeftButton:
      pRotVec.setX(release);
      break;
    case Qt::MidButton:
      pRotVec.setY(release);
      break;
    case Qt::RightButton:
      pRotVec.setZ(release);
      break;
    default:
      // We do not need to handle any more buttons
      break;
  }
}

void Viewer::resetView() {
  scale = 1.0f;
  mTransformMatrix.setToIdentity();
}
