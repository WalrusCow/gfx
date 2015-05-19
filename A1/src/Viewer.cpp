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

Viewer::Viewer(const QGLFormat& format, QWidget *parent)
  : QGLWidget(format, parent),
    positionBuffer(QOpenGLBuffer::VertexBuffer),
    colourBuffer(QOpenGLBuffer::VertexBuffer),
    vao(this) {
  mTimer = new QTimer(this);
  connect(mTimer, SIGNAL(timeout()), this, SLOT(update()));
  mTimer->start(1000 / 30);
}

Viewer::~Viewer() {

}

void Viewer::setMode(DrawMode mode) {
  switch (mode) {
    case DrawMode::WIRE:
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      break;
    case DrawMode::FACE:
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      break;
  }
}

void Viewer::viewGame(const Game* game) {
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

  vao.create();
  vao.bind();

  positionBuffer.create();
  positionBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);

  if (!positionBuffer.bind()) {
    std::cerr << "could not bind vertex buffer to the context." << std::endl;
    return;
  }
  positionBuffer.allocate(cubeCoords, sizeof(cubeCoords));

  mProgram.enableAttributeArray("vert");
  mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);

  colourBuffer.create();
  colourBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
  if (!colourBuffer.bind()) {
    std::cerr << "could not bind colour buffer to the context." << std::endl;
    return;
  }
  colourBuffer.allocate(cubeColours, sizeof(cubeColours));


  mProgram.enableAttributeArray("colour");
  mProgram.setAttributeBuffer("colour", GL_FLOAT, 0, 3);

  mMvpMatrixLocation = mProgram.uniformLocation("mvpMatrix");

  initializeWell();
}

void Viewer::initializeWell() {
  QMatrix4x4 wellTransform;
  // Top left
  wellTransform.translate(-6, 11, 0);

  // Initialize well cubes
  // 20 deep and 10 across is 21 down, 11 right, 20 up
  float wellColour[3] = {0.25, 0.25, 0.25};
  for (int i = 0; i < 21; ++i) {
    wellTransform.translate(0, -1, 0);
    cubes.emplace_back(wellTransform, wellColour);
  }
  for (int i = 0; i < 11; ++i) {
    wellTransform.translate(1, 0, 0);
    cubes.emplace_back(wellTransform, wellColour);
  }
  for (int i = 0; i < 20; ++i) {
    wellTransform.translate(0, 1, 0);
    cubes.emplace_back(wellTransform, wellColour);
  }
}

void Viewer::paintGL() {
  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Don't show faces that are in the back
  glEnable(GL_DEPTH_TEST);

  // We are drawing cuubes
  vao.bind();

  if (qApp->mouseButtons() != Qt::NoButton) {
    pRotDx = 0;
  }
  // Keep on rotating, man
  if (pRotDx != 0) {
    rotateWorld(ROTATE_FACTOR * pRotDx, pRotVec);
  }

  auto cameraMatrix = getCameraMatrix();

  for (const auto& cube : cubes) {
    mProgram.setUniformValue(mMvpMatrixLocation, cameraMatrix * cube.transform);
    cube.draw(colourBuffer);
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

  if (event->modifiers() & Qt::ShiftModifier) {
    scaling = true;
  }
  lastMouseX = event->x();
}

void Viewer::mouseReleaseEvent (QMouseEvent* event) {
  (void) event;
  if (scaling) {
    scaling = false;
  }
  else {
    updateRotVector(event->button(), true);
  }
  lastMouseX = -1;
}

void Viewer::mouseMoveEvent (QMouseEvent* event) {
  int x = event->x();
  int dx = x - lastMouseX;

  if (scaling) {
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
