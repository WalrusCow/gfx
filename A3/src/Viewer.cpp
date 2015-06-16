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
  walkStack.push_back(QMatrix4x4());
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
  // Three vertices per triangle, and each has a normal (3 floats)
  float sphereNormals[numTriangles * 3 * 3];
  initSphereData(sphereData, sphereNormals, theta);

  mSphereBufferObject.create();
  mSphereBufferObject.setUsagePattern(QOpenGLBuffer::StaticDraw);
  if (!mSphereBufferObject.bind()) {
    std::cerr << "Could not bind sphere vertex buffer." << std::endl;
    return;
  }
  mSphereBufferObject.allocate(sphereData, sizeof(sphereData));

  // Sphere normal buffer
  mSphereNormalBuffer.create();
  mSphereNormalBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
  if (!mSphereNormalBuffer.bind()) {
    std::cerr << "Could not bind sphere normal buffer." << std::endl;
    return;
  }
  mSphereNormalBuffer.allocate(sphereNormals, sizeof(sphereNormals));

  mProgram.bind();
  mProgram.enableAttributeArray("vert");
  mProgram.enableAttributeArray("norm");

  mvpMatrixLoc = mProgram.uniformLocation("mvpMatrix");
  mvMatrixLoc = mProgram.uniformLocation("mvMatrix");
  colourLoc = mProgram.uniformLocation("frag_colour");
  normMatrixLoc = mProgram.uniformLocation("normMatrix");
  lightPositionLoc = mProgram.uniformLocation("lightPosition");
  ambientColourLoc = mProgram.uniformLocation("ambientColour");
  diffuseColourLoc = mProgram.uniformLocation("diffuseColour");
  specularColourLoc = mProgram.uniformLocation("specularColour");
  shininessLoc = mProgram.uniformLocation("shininess");
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

bool Viewer::redoOp() {
  if (opStackPosition + 1 == (int) opStack.size()) {
    // Cannot redo past the end
    return false;
  }

  opStackPosition += 1;

  // Redo in map
  auto& entry = opStack[opStackPosition];
  auto m = entry.matrix;
  for (int i : entry.ids) {
    opMap[i] = opMap[i] * m;
  }
  return true;
}


bool Viewer::undoOp() {
  if (opStackPosition < 0) {
    // Cannot undo past the end
    return false;
  }

  // Undo in map
  auto& entry = opStack[opStackPosition];
  auto m = entry.matrix.inverted();
  for (int i : entry.ids) {
    opMap[i] = opMap[i] * m;
  }

  // Don't actually remove from the stack - just decrease counter
  // That way we can increase counter to redo
  opStackPosition -= 1;
  return true;
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
  QMatrix4x4 vMatrix;

  QMatrix4x4 cameraTransformation;
  auto cameraPosition = QVector3D(0, 0, 2.0);
  QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);

  vMatrix.lookAt(cameraPosition, QVector3D(0, 0, 0), cameraUpDirection);
  return mPerspMatrix * vMatrix * mTransformMatrix;
}

void Viewer::set_colour(const QColor& col) {
  mProgram.setUniformValue(colourLoc, col.red(), col.green(), col.blue());
}

void Viewer::paintGL() {
  // Clear framebuffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Set up lighting

  // Draw scene recursively
  sceneRoot->walk_gl(this);

  set_colour(QColor(1.0, 0.0, 0.0));

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
  mProgram.setUniformValue(mvpMatrixLoc, orthoMatrix * transformMatrix);

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
void Viewer::initSphereData(float* vertexBuffer, float* normBuffer, double theta) {
  int tri = 0; // How many triangles we drew
  int triSize = 3 * 3; // Three vertices, each with (x, y, z)

  // Helper to write a triangle determined by 3 points
  auto writeTri = [&] (const std::vector<QVector3D>& pts) {
    // Write three points
    for (int j = 0; j < 3; ++j) {
      // Point we are writing
      auto& pt = pts[j];
      auto normal = pts[j].normalized();//QVector3D::crossProduct(pt, pts[(j + 1) % 3]);

      // .. and each point has (x, y ,z)
      for (int i = 0; i < 3; ++i) {
        auto idx = (triSize*tri) + (3*j) + i;
        vertexBuffer[idx] = pt[i];
        normBuffer[idx] = normal[i];
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

void Viewer::pushWalkMatrix(const QMatrix4x4& matrix) {
  walkStack.push_back(walkStack.back() * matrix);
}

void Viewer::popWalkMatrix() {
  walkStack.pop_back();
}

QMatrix4x4 Viewer::getWalkMatrix() {
  return walkStack.back();
}

void printVector(const QVector3D& v) {
  std::cerr << "{"<<v[0]<<','<<v[1]<<','<<v[2]<<'}'<<std::endl;
}

void Viewer::drawSphere(const QMatrix4x4& transform, const Material& material) {
  set_colour(material.getColour());

  auto vp = getCameraMatrix();
  mSphereBufferObject.bind();
  mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);
  mSphereNormalBuffer.bind();
  mProgram.setAttributeBuffer("norm", GL_FLOAT, 0, 3);

  auto mvMatrix = mTransformMatrix * transform;
  mProgram.setUniformValue(mvpMatrixLoc, vp * transform);
  mProgram.setUniformValue(mvMatrixLoc, mvMatrix);
  mProgram.setUniformValue(normMatrixLoc, mvMatrix.normalMatrix());
  mProgram.setUniformValue(ambientColourLoc, QVector3D(0.125, 0.125, 0.125));
  mProgram.setUniformValue(diffuseColourLoc, QVector3D(0.5, 0.5, 0.5));
  mProgram.setUniformValue(specularColourLoc, QVector3D(1.0, 1.0, 1.0));
  mProgram.setUniformValue(shininessLoc, 100.0f);

  // Light position shouldn't change: always on the eye
  mProgram.setUniformValue(lightPositionLoc, mTransformMatrix * QVector3D());

  glDrawArrays(GL_TRIANGLES, 0, numTriangles * 9);
}
