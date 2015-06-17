// William McDonald 20418145 wmcdonal
#include "Viewer.hpp"

#include <iostream>
#include <cmath>

#include <QtGlobal>
#include <QOpenGLFramebufferObject>
#include <QtWidgets>
#include <QtOpenGL>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

namespace {
const double TRANSLATE_FACTOR = 0.1;
const double ROTATE_FACTOR = 2;
}

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

Viewer::~Viewer() {}

void Viewer::resetPuppetOrientation() {
  puppetRotation.setToIdentity();
  update();
}

void Viewer::resetPuppetPosition() {
  puppetPosition.setToIdentity();
  update();
}

void Viewer::resetJoints() {
  opStack.resize(0);
  opStackPosition = -1;
  opMap.clear();
  update();
}

void Viewer::toggleShowCircle() {
  showCircle = !showCircle;
  update();
}

void Viewer::toggleZBuffer() {
  zBuffer = !zBuffer;
  if (zBuffer) {
    glEnable(GL_DEPTH_TEST);
  } else {
    glDisable(GL_DEPTH_TEST);
  }
  update();
}

void Viewer::toggleFrontfaceCull() {
  frontfaceCull = !frontfaceCull;
  updateFaceCulling();
}

void Viewer::toggleBackfaceCull() {
  backfaceCull = !backfaceCull;
  updateFaceCulling();
}

void Viewer::updateFaceCulling() {
  glEnable(GL_CULL_FACE);
  if (frontfaceCull && backfaceCull)
    glCullFace(GL_FRONT_AND_BACK);
  else if (frontfaceCull)
    glCullFace(GL_FRONT);
  else if (backfaceCull)
    glCullFace(GL_BACK);
  else
    glDisable(GL_CULL_FACE);
  update();
}

QSize Viewer::minimumSizeHint() const { return QSize(50, 50); }

QSize Viewer::sizeHint() const { return QSize(500, 500); }

void Viewer::initializeGL() {
  QGLFormat glFormat = QGLWidget::format();
  if (!glFormat.sampleBuffers()) {
    std::cerr << "Could not enable sample buffers." << std::endl;
    return;
  }

  glShadeModel(GL_SMOOTH);
  glClearColor(0.4, 0.4, 0.4, 0.0);
  glFrontFace(GL_CW);

  if (!sphereShaders.addShaderFromSourceFile(QGLShader::Vertex, "sphere.vert")) {
    std::cerr << "Cannot load vertex shader." << std::endl;
    return;
  }

  if (!sphereShaders.addShaderFromSourceFile(QGLShader::Fragment, "sphere.frag")) {
    std::cerr << "Cannot load fragment shader." << std::endl;
    return;
  }

  if (!sphereShaders.link()) {
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

  sphereShaders.bind();
  sphereShaders.enableAttributeArray("vert");
  sphereShaders.enableAttributeArray("norm");
  sphereShaders.enableAttributeArray("flatFlag");

  mvpMatrixLoc = sphereShaders.uniformLocation("mvpMatrix");
  mvMatrixLoc = sphereShaders.uniformLocation("mvMatrix");
  normMatrixLoc = sphereShaders.uniformLocation("normMatrix");
  lightPositionLoc = sphereShaders.uniformLocation("lightPosition");
  ambientLightLoc = sphereShaders.uniformLocation("ambientLight");
  diffuseColourLoc = sphereShaders.uniformLocation("diffuseColour");
  specularColourLoc = sphereShaders.uniformLocation("specularColour");
  shininessLoc = sphereShaders.uniformLocation("shininess");
  flatLoc = sphereShaders.uniformLocation("flatFlag");

  // Constant ambient light
  sphereShaders.setUniformValue(ambientLightLoc, QVector3D(0.1, 0.1, 0.1));
}

void Viewer::resizeGL(int width, int height) {
  if (height == 0) {
    height = 1;
  }

  mPerspMatrix.setToIdentity();
  mPerspMatrix.perspective(60.0, (float) width / (float) height, 0.001, 1000);

  glViewport(0, 0, width, height);
}

void Viewer::addOp(std::set<int> ids) {
  // Resize to current size
  opStack.resize(opStackPosition + 1);

  opStack.emplace_back(std::move(ids), Op());
  // Next position please
  opStackPosition += 1;
}

bool Viewer::redoOp() {
  if (opStackPosition + 1 == (int) opStack.size()) {
    // Cannot redo past the end
    return false;
  }

  opStackPosition += 1;

  // Redo in map
  auto& entry = opStack[opStackPosition];
  auto xAngle = entry.op.xAngle;
  auto yAngle = entry.op.yAngle;
  for (int i : entry.ids) {
    opMap[i].xAngle += xAngle;
    opMap[i].yAngle += yAngle;
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
  auto xAngle = entry.op.xAngle;
  auto yAngle = entry.op.yAngle;
  for (int i : entry.ids) {
    opMap[i].xAngle -= xAngle;
    opMap[i].yAngle -= yAngle;
  }

  // Don't actually remove from the stack - just decrease counter
  // That way we can increase counter to redo
  opStackPosition -= 1;
  return true;
}

void Viewer::mousePressEvent(QMouseEvent* event) {

  lastMouseX = event->x();
  lastMouseY = event->y();

  if (currentMode == Mode::POSITION) {
    // We can do nothing, since we don't have to keep stack
    return;
  }

  auto buttonsDown = qApp->mouseButtons();
  auto clickedButton = event->button();
  bool holdingOther = (buttonsDown & ~clickedButton);

  if (clickedButton == Qt::LeftButton && holdingOther) {
    // Do not allow picking while holding other buttons
    return;
  }

  if (clickedButton == Qt::LeftButton) {
    // We can pick now: Only holding left button
    auto id = findPickId(event->x(), event->y());
    std::cerr << "Found id " << id << std::endl;
    id = sceneRoot->getJointForId(id);

    std::cerr << "Found joint id " << id << std::endl;
    if (id > 0 && pickedIds.find(id) == pickedIds.end()) {
      // Not in the set: Add to set
      pickedIds.insert(id);
    }
    else {
      pickedIds.erase(id);
    }
    for (auto i : pickedIds) {
      std::cerr<<i<<',';
    }
    std::cerr << std::endl;
    update();
  }
  else if (pickedIds.size() > 0) {
    addOp(pickedIds);
  }
}

void Viewer::mouseReleaseEvent (QMouseEvent* event) {
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

void Viewer::mouseMoveEvent(QMouseEvent* event) {
  // So, while moving... update the top of the stack.
  int dx = event->x() - lastMouseX;
  // Y coordinates on screen are upside down
  int dy = lastMouseY - event->y();

  auto buttons = event->buttons();

  if (currentMode == Viewer::Mode::POSITION) {
    // Position mode
    // Left mouse button drags puppet
    if (buttons & Qt::LeftButton) {
      puppetPosition.translate(TRANSLATE_FACTOR * dx, TRANSLATE_FACTOR * dy, 0);
    }
    // Middle button translates puppet in Z
    if (buttons & Qt::MiddleButton) {
      puppetPosition.translate(0, 0, TRANSLATE_FACTOR * -dy);
    }
    // Right button rotates puppet according to some horrible trackball stuff
    if (buttons & Qt::RightButton) {
      QVector2D lastMouse(lastMouseX, lastMouseY);
      QVector2D newMouse(event->x(), event->y());
      trackballRotate(lastMouse, newMouse, &puppetRotation);
    }
    update();
  }
  // Joints mode
  // Take action if we are holdin' it down and have picked something
  else if ((buttons & (Qt::MiddleButton|Qt::RightButton)) && pickedIds.size()) {
    // Middle button is for x-axis rotation (most limbs)
    auto xAngle = (buttons & Qt::MiddleButton) ? -dy * ROTATE_FACTOR : 0;
    // Right button is for y-axis rotation (head turning)
    auto yAngle = (buttons & Qt::RightButton) ? dx * ROTATE_FACTOR : 0;

    for (int id : pickedIds) {
      // If we don't have any ops yet for this one
      if (opMap.find(id) == opMap.end()) opMap[id] = Op();
      opMap[id].xAngle += xAngle;
      opMap[id].yAngle += yAngle;
    }
    // Update the top of the stack
    opStack[opStackPosition].op.xAngle += xAngle;
    opStack[opStackPosition].op.yAngle += yAngle;
    update();
  }

  // Store mouse location again
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

void Viewer::paintGL() {
  glClearColor(0.4, 0.4, 0.4, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  sceneRoot->walk_gl(this);

  if (showCircle) {
    draw_trackball_circle();
  }
}

void Viewer::draw_trackball_circle() {
  auto w = width();
  auto h = height();

  setDiffuseColour(QColor(0.0, 0.0, 0.0));
  setSpecularColour(QColor(0.0, 0.0, 0.0));

  // Set orthographic Matrix
  QMatrix4x4 orthoMatrix;
  orthoMatrix.ortho(0.0, (float)w, 0.0, (float)h, -0.1, 0.1);

  // Translate the view to the middle
  QMatrix4x4 transformMatrix;
  transformMatrix.translate(w/2.0, h/2.0, 0.0);

  // Bind buffer object
  mCircleBufferObject.bind();
  sphereShaders.setAttributeBuffer("vert", GL_FLOAT, 0, 3);
  sphereShaders.setUniformValue(mvpMatrixLoc, orthoMatrix * transformMatrix);
  sphereShaders.setUniformValue(flatLoc, true);

  // Draw buffer
  glDrawArrays(GL_LINE_LOOP, 0, 40);
}

void Viewer::setMode(Viewer::Mode mode) {
  if (mode == currentMode) {
    return;
  }

  // lol, clear
  // TODO: How to handle dragging events? That is unclear
  //pickedIds.clear();
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
      auto normal = pts[j].normalized();

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

void Viewer::drawSphere(const QMatrix4x4& transform, bool picking) {
  auto r = sceneRoot->get_transform() * puppetRotation * sceneRoot->get_inverse();
  auto modelMatrix = puppetPosition * r * transform;

  auto vp = getCameraMatrix();
  auto mvMatrix = mTransformMatrix * modelMatrix;

  mSphereBufferObject.bind();
  sphereShaders.setAttributeBuffer("vert", GL_FLOAT, 0, 3);
  mSphereNormalBuffer.bind();
  sphereShaders.setAttributeBuffer("norm", GL_FLOAT, 0, 3);

  sphereShaders.setUniformValue(mvpMatrixLoc, vp * modelMatrix);
  sphereShaders.setUniformValue(mvMatrixLoc, mvMatrix);
  sphereShaders.setUniformValue(normMatrixLoc, mvMatrix.normalMatrix());
  // Whether or not we are picking (to do lighting or not)
  sphereShaders.setUniformValue(flatLoc, picking);

  // Light position shouldn't change: always on the eye
  sphereShaders.setUniformValue(lightPositionLoc, mTransformMatrix * QVector3D());

  glDrawArrays(GL_TRIANGLES, 0, numTriangles * 9);
}

void Viewer::setDiffuseColour(const QColor& c, int id) {
  // Set colour to be white (it's multiplied by the actual colour)
  if (pickedIds.find(id) != pickedIds.end())
    sphereShaders.setUniformValue(diffuseColourLoc, 1, 1, 1);
  else
    sphereShaders.setUniformValue(diffuseColourLoc, c.redF(), c.greenF(), c.blueF());
}

void Viewer::setSpecularColour(const QColor& c) {
  sphereShaders.setUniformValue(specularColourLoc, c.redF(), c.greenF(), c.blueF());
}

void Viewer::setShininess(const double shininess) {
  sphereShaders.setUniformValue(shininessLoc, (float) shininess);
}

void Viewer::trackballRotate(const QVector2D& startCoords,
                             const QVector2D& endCoords,
                             QMatrix4x4* mat) {
  // TODO: We could get these from a common location
  QVector2D trackballCenter(width() / 2.0, height() / 2.0);
  auto diam = std::min(width(), height()) / 2.0;

  // Convert into coordinates centered at trackball center
  auto start = (startCoords - trackballCenter);
  auto end = (endCoords - trackballCenter);
  // Y coordinates are upside down
  start[1] *= -1;
  end[1] *= -1;

  end *= 2.0 / diam;
  start *= 2.0 / diam;

  // Vectors from center of "trackball" to points
  QVector3D endVector(end[0], end[1], 1 - end[0]*end[0] - end[1]*end[1]);
  QVector3D startVector(start[0], start[1], 1 - start[0]*start[0] - start[1]*start[1]);

  // Cheat because vector would be const
  for (auto vp : {&endVector, &startVector}) {
    auto& v = *vp;
    if (v[2] < 0) {
      // Mouse outside trackball: rotation about Z
      v /= std::sqrt(1 - v[2]);
      v[2] = 0;
    }
    else {
      v[2] = std::sqrt(v[2]);
    }
  }

  // Vector to rotate about. Length of vector is angle of rotation in radians.
  auto rotationVector = QVector3D::crossProduct(startVector, endVector);
  auto angleDeg = rotationVector.length() / (2 * M_PI) * 360;
  rotationVector.normalize();
  // Qt rotates on the right. We want the left.
  QMatrix4x4 rotMat;
  rotMat.rotate(angleDeg, rotationVector);
  *mat = rotMat * *mat;
}

int Viewer::findPickId(int x, int y) {
  glDisable(GL_DITHER);
  glEnable(GL_DEPTH_TEST);
  QOpenGLFramebufferObject obj(width(), height());
  obj.setAttachment(QOpenGLFramebufferObject::Depth);
  obj.bind();
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  sceneRoot->walk_gl(this, true);

  glFlush(); glFinish();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  unsigned char data[4];
  glReadPixels(x, height()-y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
  if (!zBuffer) glDisable(GL_DEPTH_TEST);
  glEnable(GL_DITHER);
  obj.release();
  return (data[0] + (data[1] * 256) + (data[2] * 256*256))/4;
}

double Viewer::getXRotationAngle(int id) {
  if (opMap.find(id) == opMap.end()) {
    return 0;
  }
  return opMap[id].xAngle;
}

double Viewer::getYRotationAngle(int id) {
  if (opMap.find(id) == opMap.end()) {
    return 0;
  }
  return opMap[id].yAngle;
}
