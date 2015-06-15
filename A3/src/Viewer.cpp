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
               mVertexArrayObject(this) {
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
  glClearColor( 0.4, 0.4, 0.4, 0.0 );
  glEnable(GL_DEPTH_TEST);

  if (!mProgram.addShaderFromSourceFile(QGLShader::Vertex, "shader.vert")) {
    std::cerr << "Cannot load vertex shader." << std::endl;
    return;
  }

  if (!mProgram.addShaderFromSourceFile(QGLShader::Fragment, "shader.frag")) {
    std::cerr << "Cannot load fragment shader." << std::endl;
    return;
  }

  if ( !mProgram.link() ) {
    std::cerr << "Cannot link shaders." << std::endl;
    return;
  }

  float circleData[120];

  auto w = width();
  auto h = height();
  double radius = w < h ?  (float)w * 0.25 : (float)h * 0.25;

  for(size_t i = 0; i < 40; ++i) {
    circleData[i*3] = radius * std::cos(i*2*M_PI/40);
    circleData[i*3 + 1] = radius * std::sin(i*2*M_PI/40);
    circleData[i*3 + 2] = 0.0;
  }


  mVertexArrayObject.create();
  mVertexArrayObject.bind();

  mCircleBufferObject.create();
  mCircleBufferObject.setUsagePattern(QOpenGLBuffer::StaticDraw);

  if (!mCircleBufferObject.bind()) {
    std::cerr << "could not bind vertex buffer to the context." << std::endl;
    return;
  }

  mCircleBufferObject.allocate(circleData, 40 * 3 * sizeof(float));

  mProgram.bind();

  mProgram.enableAttributeArray("vert");
  mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);

  mMvpMatrixLocation = mProgram.uniformLocation("mvpMatrix");
  mColorLocation = mProgram.uniformLocation("frag_color");
}

void Viewer::paintGL() {
  // Clear framebuffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Set up lighting

  // Draw scene recursively
  sceneRoot->walk_gl(this);

  draw_trackball_circle();
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

  if (opStackPosition + 1 < opStack.size()) {
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
  if (opStackPosition + 1 == opStack.size()) {
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
  //std::cerr << "Stub: button " << event->button() << " pressed\n";
  // Push new matrix on to stack - but only if no mouse buttons are held down
  // at the moment (?)
  auto buttonsDown = qApp->mouseButtons();
  std::cerr << buttonsDown << std::endl;

  // If there are additional buttons held other than the one that was down
  if ((buttonsDown & (~ event->button())) != Qt::NoButton) {
    // .. Then we are pressing a second button
  }
  else {
    // New blank transform matrix on top of stack
    doOp(pickedIds, QMatrix4x4());
  }

  // TODO: get time (for click?)
  lastMouseX = event->x();
  lastMouseY = event->y();
}

void Viewer::mouseReleaseEvent ( QMouseEvent * event ) {
  if (currentMode == Viewer::Mode::POSITION) {
    // Nothing to do
    return;
  }

  if (qApp->mouseButtons() != Qt::NoButton) {
    // Still dragging around.. do nothing
    return;
  }

  // We are in joints mode: we can pick parts
  if (event->x() == lastMouseX && event->y() == lastMouseY) {
    // We were probably just a click
    auto id = find_pick_id(event->x(), event->y());
    if (pickedIds.find(id) != pickedIds.end()) {
      // Not in the set: Add to set
      pickedIds.insert(id);
    }
  }
}

void Viewer::mouseMoveEvent (QMouseEvent* event) {
  //std::cerr << "Stub: Motion at " << event->x() << ", " << event->y() << std::endl;
  // So, while moving... update the top of the stack.
  int dx = event->x() - lastMouseX;
  int dy = event->y() - lastMouseY;

  if (currentMode == Viewer::Mode::POSITION) {
    // Position mode
    // TODO: How to dooooo...
    // We also need to be able to undo thissssss...
    // fuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuck...
    QMatrix4x4 op;// = getPuppetOp();
    opMap[PUPPET_TRANSLATE_ID] *= op;
  }
  else if (pickedIds.size() > 0) {
    // Joints mode
    // Only do anything if we have picked at least one item

    // Also we need to update the map and shit
    QMatrix4x4 op;//= doGradualOp();
    for (int id : pickedIds) {
      opMap[id] *= op;
    }

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

void Viewer::draw_trackball_circle() {
  int current_width = width();
  int current_height = height();

  // Set up for orthogonal drawing to draw a circle on screen.
  // You'll want to make the rest of the function conditional on
  // whether or not we want to draw the circle this time around.

  set_colour(QColor(0.0, 0.0, 0.0));

  // Set orthographic Matrix
  QMatrix4x4 orthoMatrix;
  orthoMatrix.ortho(0.0, (float)current_width,
                    0.0, (float)current_height, -0.1, 0.1);

  // Translate the view to the middle
  QMatrix4x4 transformMatrix;
  transformMatrix.translate(width()/2.0, height()/2.0, 0.0);

  // Bind buffer object
  mCircleBufferObject.bind();
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
