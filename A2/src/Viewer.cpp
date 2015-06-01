#include "Viewer.hpp"

#include <cmath>
#include <iostream>

#include <QtWidgets>
#include <QtOpenGL>
#include <GL/gl.h>
#include <GL/glu.h>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

const double Viewer::SCALE_FACTOR = 1.001;
const double Viewer::ROTATE_FACTOR = M_PI / 320;
const double Viewer::TRANSLATE_FACTOR = 0.01;
const double Viewer::FOV_FACTOR = 0.5;

Viewer::Viewer(const QGLFormat& format, QWidget *parent)
  : QGLWidget(format, parent),
    mVertexBufferObject(QOpenGLBuffer::VertexBuffer),
    mVertexArrayObject(this) {

  refreshTimer = new QTimer(this);
  connect(refreshTimer, SIGNAL(timeout()), this, SLOT(update()));
  refreshTimer->start(1000 / 30);
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

void Viewer::set_perspective(double fov, double aspect,
                             double near, double far) {
  // Fill me in!
}

void Viewer::resetView() {
  // Fill me in!
}

void Viewer::setMode(Mode newMode) {
  mode = newMode;
}

void Viewer::initializeGL() {
  // Do some OpenGL setup
  QGLFormat glFormat = QGLWidget::format();
  if (!glFormat.sampleBuffers()) {
    std::cerr << "Could not enable sample buffers." << std::endl;
    return;
  }

  glClearColor(0.7, 0.7, 0.7, 0.0);

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

  mVertexArrayObject.create();
  mVertexArrayObject.bind();

  mVertexBufferObject.create();
  mVertexBufferObject.setUsagePattern(QOpenGLBuffer::StaticDraw);

  if (!mVertexBufferObject.bind()) {
    std::cerr << "could not bind vertex buffer to the context." << std::endl;
    return;
  }

  mProgram.bind();

  mProgram.enableAttributeArray("vert");
  mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);

  mColorLocation = mProgram.uniformLocation("frag_color");

  boxModel.rotateZ(M_PI / 16);
  boxGnomon.rotateZ(M_PI / 16);
}

void Viewer::paintGL() {
  draw_init();

  set_colour(QColor(1.0, 1.0, 1.0));

  auto viewM = viewPoint.getViewMatrix();

  for (const auto& model : {boxModel, boxGnomon, worldGnomon}) {
    auto v = model.getLines();

    // TODO: Perspective
    for (auto& line : v) {
      // Now we have the view coordinates
      line.start = viewM * line.start;
      line.end = viewM * line.end;

      // We need to get the 

      auto z = line.start[2];
      auto p1 = QVector2D(line.start[0] / z, line.start[1] / z);
      z = line.end[2];
      auto p2 = QVector2D(line.end[0] / z, line.end[1] / z);
      draw_line(p1, p2);
    }
  }
}

void Viewer::mousePressEvent(QMouseEvent* event) {
  lastMouseX = event->x();
  return;
}

void Viewer::mouseReleaseEvent(QMouseEvent* event) {
}

void Viewer::mouseMoveEvent(QMouseEvent* event) {
  int dx = event->x() - lastMouseX;
  lastMouseX = event->x();

  auto buttons = event->buttons();
  bool left = !!(buttons & Qt::LeftButton);
  bool mid = !!(buttons & Qt::MidButton);
  bool right = !!(buttons & Qt::RightButton);

  switch (mode) {
  case Mode::VIEW_ROTATE:
    rotate(viewPoint, dx, left, mid, right);
    break;
  case Mode::VIEW_TRANSLATE:
    translate(viewPoint, dx, left, mid, right);
    break;
  case Mode::VIEW_PERSPECTIVE:
    changePerspective(dx, left, mid, right);
    break;
  case Mode::MODEL_ROTATE:
    rotate(boxModel, dx, left, mid, right);
    rotate(boxGnomon, dx, left, mid, right);
    break;
  case Mode::MODEL_TRANSLATE:
    translate(boxModel, dx, left, mid, right);
    translate(boxGnomon, dx, left, mid, right);
    break;
  case Mode::MODEL_SCALE:
    scale(boxModel, dx, left, mid, right);
    break;
  case Mode::VIEWPORT:
    // Nothing to do here
    break;
  }
}

void Viewer::scale(Model& model, int dx, bool L, bool M, bool R) {
  double val = std::pow(SCALE_FACTOR, dx);
  model.scale(L ? val : 1, M ? val : 1, R ? val : 1);
}

void Viewer::rotate(Movable& obj, int dx, bool L, bool M, bool R) {
  double val = dx * ROTATE_FACTOR;
  if (L) obj.rotateX(val);
  if (M) obj.rotateY(val);
  if (R) obj.rotateZ(val);
}

void Viewer::translate(Movable& obj, int dx, bool L, bool M, bool R) {
  double val = dx * TRANSLATE_FACTOR;
  obj.translate(L ? val : 0, M ? val : 0, R ? val : 0);
}

void Viewer::changePerspective(int dx, bool L, bool M, bool R) {
  if (L) viewPoint.changeFov(dx * FOV_FACTOR);
  if (M) viewPoint.translateNearPlane(dx * TRANSLATE_FACTOR);
  if (R) viewPoint.translateFarPlane(dx * TRANSLATE_FACTOR);
}

// Drawing Functions
// ******************* Do Not Touch ************************ //

void Viewer::draw_line(const QVector2D& p1, const QVector2D& p2) {

  const GLfloat lineVertices[] = {
  p1.x(), p1.y(), 1.0,
  p2.x(), p2.y(), 1.0
  };

  mVertexBufferObject.allocate(lineVertices, 3 * 2 * sizeof(float));

  glDrawArrays(GL_LINES, 0, 2);
}

void Viewer::set_colour(const QColor& col) {
  mProgram.setUniformValue(mColorLocation, col.red(), col.green(), col.blue());
}

void Viewer::draw_init() {
  glClearColor(0.7, 0.7, 0.7, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (double)width(), 0.0, (double)height());
  glViewport(0, 0, width(), height());

  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glLineWidth(1.0);
}
