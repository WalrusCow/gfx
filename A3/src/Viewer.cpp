// William McDonald 20418145 wmcdonal
#include "Viewer.hpp"

#include <iostream>
#include <cmath>

#include <QtGlobal>
#include <QtWidgets>
#include <QtOpenGL>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

Viewer::Viewer(const QGLFormat& format,
               std::unique_ptr<SceneNode> sceneRoot,
               QWidget *parent)
               : QGLWidget(format, parent),
               sceneRoot(std::move(sceneRoot)),
               mCircleBufferObject(QOpenGLBuffer::VertexBuffer),
               mSphereBufferObject(QOpenGLBuffer::VertexBuffer),
               mVao(this) {
}

Viewer::~Viewer() {
  // Nothing to do here right now.
}

QSize Viewer::minimumSizeHint() const {
  return QSize(50, 50);
}

QSize Viewer::sizeHint() const {
  return QSize(300, 300);
}

void Viewer::initializeGL() {
  QGLFormat glFormat = QGLWidget::format();
  if (!glFormat.sampleBuffers()) {
    std::cerr << "Could not enable sample buffers." << std::endl;
    return;
  }

  glShadeModel(GL_SMOOTH);
  glClearColor(0.4, 0.4, 0.4, 0.0);
  glEnable(GL_DEPTH_TEST);

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

  mVao.create();
  mVao.bind();

  float circleData[120];
  auto w = width();
  auto h = height();
  double radius = w < h ? (float)w * 0.25 : (float)h * 0.25;
  initCircleData(circleData, radius, M_PI / 40);

  mCircleBufferObject.create();
  mCircleBufferObject.setUsagePattern(QOpenGLBuffer::StaticDraw);
  if (!mCircleBufferObject.bind()) {
    std::cerr << "Could not bind circle vertex buffer." << std::endl;
    return;
  }
  mCircleBufferObject.allocate(circleData, sizeof(circleData));

  // Nine floats per triangle
  float sphereData[numTriangles * 9];
  initSphereData(sphereData, theta);

  mSphereBufferObject.create();
  mSphereBufferObject.setUsagePattern(QOpenGLBuffer::StaticDraw);
  if (!mSphereBufferObject.bind()) {
    std::cerr << "Could not bind sphere vertex buffer." << std::endl;
    return;
  }
  mSphereBufferObject.allocate(sphereData, sizeof(sphereData));

  mProgram.bind();
  mProgram.enableAttributeArray("vert");

  mMvpMatrixLocation = mProgram.uniformLocation("mvpMatrix");
  mColorLocation = mProgram.uniformLocation("frag_color");
}

void Viewer::resizeGL(int width, int height) {
  if (height == 0) {
    height = 1;
  }

  mPerspMatrix.setToIdentity();
  mPerspMatrix.perspective(60.0, (float) width / (float) height, 0.001, 1000);

  glViewport(0, 0, width, height);
}

void Viewer::doOp(std::set<int> ids, QMatrix4x4 matrix) {
  opStackPosition += 1;

  if (opStackPosition + 1 < (int) opStack.size()) {
    // Clear next entries
    opStack.resize(opStackPosition + 1);
  }

  // Modify the op map to include latest change
  for (int id : ids) {
    opMap[id] = opMap[id] * matrix;
  }

  // Push to end
  opStack.emplace_back(std::move(ids), std::move(matrix));
}

void Viewer::redoOp() {
  if (opStackPosition + 1 == (int) opStack.size()) {
    // TODO: Cannot
  }
  opStackPosition += 1;
}


void Viewer::undoOp() {
  // Undo
  if (opStackPosition < 0) {
    // TODO: Cannot. Throw error?
  }

  auto& entry = opStack[opStackPosition];
  // Undo in map
  auto m = entry.matrix.inverted();
  for (int i : entry.ids) {
    opMap[i] = opMap[i] * m;
  }

  // Don't actually remove from the stack - just decrease counter
  opStackPosition -= 1;
}

QMatrix4x4 Viewer::getTransforms(int id) {
  // Retrieve all transforms for the given id
  return opMap[id];
}

void Viewer::mousePressEvent ( QMouseEvent * event ) {
  auto buttonsDown = qApp->mouseButtons();

  lastMouseX = event->x();
  lastMouseY = event->y();

  if (currentMode == Mode::POSITION) {
    // TODO: I think we can do nothing, since we don't have to keep stack
    return;
  }

  auto clickedButton = event->button();
  // If there are additional buttons held other than the one that was down
  if ((buttonsDown & ~clickedButton) != Qt::NoButton) {
    // Then we are pressing a second button: nothing to do (else we would get
    // bizarre behaviour)
    return;
  }

  if (event->button() == Qt::LeftButton) {
    // We can pick now
    auto id = find_pick_id(event->x(), event->y());
    if (pickedIds.find(id) != pickedIds.end()) {
      // Not in the set: Add to set
      pickedIds.insert(id);
    }
    return;
  }
  else if (pickedIds.size() > 0) {
    // We have picked at least one thing or are in position mode
    // begin the operation of dragging etc
    doOp(pickedIds, QMatrix4x4());
  }
}

void Viewer::mouseReleaseEvent ( QMouseEvent * event ) {
  (void)event;
  // Nothing to do (already pushed on stack)
  return;

  if (currentMode == Viewer::Mode::POSITION) {
    // Nothing to do
    return;
  }

  if (qApp->mouseButtons() != Qt::NoButton) {
    // Still dragging around.. do nothing
    return;
  }
}

void Viewer::mouseMoveEvent (QMouseEvent* event) {
  //std::cerr << "Stub: Motion at " << event->x() << ", " << event->y() << std::endl;
  // So, while moving... update the top of the stack.
  //int dx = event->x() - lastMouseX;
  // Y coordinates on screen are upside down
  //int dy = lastMouseY - event->y();

  if (currentMode == Viewer::Mode::POSITION) {
    // Position mode
    QMatrix4x4 op;// = getPuppetOp();
    //opMap[PUPPET_TRANSLATE_ID] *= op;
  }
  else if (pickedIds.size() > 0) {
    // Joints mode
    // Only do anything if we have picked at least one item

    QMatrix4x4 op;//= doGradualOp(dx, dy);
    for (int id : pickedIds) {
      opMap[id] *= op;
    }
    // And update the stack as well
    opStack[opStackPosition].matrix *= op;
  }

  lastMouseX = event->x();
  lastMouseY = event->y();
}

QMatrix4x4 Viewer::getCameraMatrix() {
  // Todo: Ask if we want to keep this.
  QMatrix4x4 vMatrix;

  QMatrix4x4 cameraTransformation;
  QVector3D cameraPosition = cameraTransformation * QVector3D(0, 0, 20.0);
  QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);

  vMatrix.lookAt(cameraPosition, QVector3D(0, 0, 0), cameraUpDirection);

  return mPerspMatrix * vMatrix * mTransformMatrix;
}

void Viewer::translateWorld(float x, float y, float z) {
  // Todo: Ask if we want to keep this.
  mTransformMatrix.translate(x, y, z);
}

void Viewer::rotateWorld(float angle, float x, float y, float z) {
  // Todo: Ask if we want to keep this.
  mTransformMatrix.rotate(angle, x, y, z);
}

void Viewer::scaleWorld(float x, float y, float z) {
  // Todo: Ask if we want to keep this.
  mTransformMatrix.scale(x, y, z);
}

void Viewer::set_colour(const QColor& col) {
  mProgram.setUniformValue(mColorLocation, col.red(), col.green(), col.blue());
}

void Viewer::paintGL() {
  // Clear framebuffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Set up lighting

  // Draw scene recursively
  sceneRoot->walk_gl(this);

  set_colour(QColor(1.0, 0.0, 0.0));

  auto m = getCameraMatrix();
  mSphereBufferObject.bind();
  mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);

  mProgram.setUniformValue(mMvpMatrixLocation, m);
  glDrawArrays(GL_TRIANGLES, 0, numTriangles * 9);

  draw_trackball_circle();
}

void Viewer::draw_trackball_circle() {
  auto w = width();
  auto h = height();

  // Set up for orthogonal drawing to draw a circle on screen.
  // You'll want to make the rest of the function conditional on
  // whether or not we want to draw the circle this time around.

  set_colour(QColor(0.0, 0.0, 0.0));

  // Set orthographic Matrix
  QMatrix4x4 orthoMatrix;
  orthoMatrix.ortho(0.0, (float)w, 0.0, (float)h, -0.1, 0.1);

  // Translate the view to the middle
  QMatrix4x4 transformMatrix;
  transformMatrix.translate(w/2.0, h/2.0, 0.0);

  // Bind buffer object
  mCircleBufferObject.bind();
  mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);
  mProgram.setUniformValue(mMvpMatrixLocation, orthoMatrix * transformMatrix);

  // Draw buffer
  glDrawArrays(GL_LINE_LOOP, 0, 40);
}

void Viewer::setMode(Viewer::Mode mode) {
  if (mode == currentMode) {
    return;
  }

  // lol, clear
  // TODO: How to handle dragging events? That is unclear
  pickedIds.clear();
  currentMode = mode;
}

// Use quadrilaterals to model a sphere
void Viewer::initSphereData(float* vertexBuffer, double theta) {
  int tri = 0; // How many triangles we drew
  int triSize = 3 * 3; // Three vertices, each with (x, y, z)

  // Helper to write a triangle determined by 3 points
  auto writeTri = [&] (const std::vector<QVector3D> pts) {
    for (int pt = 0; pt < 3; ++pt) {
      for (int i = 0; i < 3; ++i) {
        vertexBuffer[triSize * tri + 3 * pt + i] = pts[pt][i];
      }
    }
    tri += 1;
  };

  // This loop goes around the circumference (standing on XZ plane)
  for (double i = 0; i < 2 * M_PI; i += theta) {
    // This loop goes from the bottom to the top
    for (double j = -M_PI / 2; j < M_PI / 2; j += theta) {
      auto y1 = std::sin(j);
      auto y2 = std::sin(j + theta);
      auto j1 = std::cos(j);
      auto j2 = std::cos(j + theta);

      // Clockwise starting from bottom right if you look out from centre
      QVector3D p1(j1 * std::cos(i), y1, j1 * std::sin(i));
      QVector3D p2(j1 * std::cos(i + theta), y1, j1 * std::sin(i + theta));
      QVector3D p3(j2 * std::cos(i + theta), y2, j2 * std::sin(i + theta));
      QVector3D p4(j2 * std::cos(i), y2, j2 * std::sin(i));

      // We only have triangles, so use 2 for the quadrilateral
      writeTri({p1, p2, p4});
      writeTri({p2, p3, p4});
    }
  }
}

void Viewer::initCircleData(float* buffer, double radius, double theta) {
  for(size_t i = 0; i < 40; ++i) {
    buffer[i * 3] = radius * std::cos(i * 2 * theta);
    buffer[i * 3 + 1] = radius * std::sin(i * 2 * theta);
    buffer[i * 3 + 2] = 0.0;
  }
}
