#include "Viewer.hpp"

#include <cmath>
#include <iostream>

#include <QtWidgets>
#include <QtOpenGL>
#include <GL/gl.h>
#include <GL/glu.h>

#include "AppWindow.hpp"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

const double Viewer::SCALE_FACTOR = 1.001;
const double Viewer::ROTATE_FACTOR = M_PI / 320;
const double Viewer::TRANSLATE_FACTOR = 0.01;
const double Viewer::FOV_FACTOR = M_PI / 640;
// About 5 degrees
const double Viewer::MIN_FOV = M_PI / 36;
// Can't go to PI without disaster :)
const double Viewer::MAX_FOV = M_PI - (M_PI / 9);
const double Viewer::MIN_NEAR = 0.1;

Viewer::Viewer(const QGLFormat& format, QWidget *parent)
  : QGLWidget(format, parent),
    mVertexBufferObject(QOpenGLBuffer::VertexBuffer),
    mVertexArrayObject(this) {

  appWindow = static_cast<AppWindow*>(parent);
  refreshTimer = new QTimer(this);
  connect(refreshTimer, SIGNAL(timeout()), this, SLOT(update()));
  refreshTimer->start(1000 / 30);
}

QSize Viewer::minimumSizeHint() const {
  return QSize(50, 50);
}

QSize Viewer::sizeHint() const {
  return QSize(300, 300);
}

void Viewer::resetView() {
  boxModel.reset();
  boxGnomon.reset();
  viewPoint.reset();
  near = 3;
  far = 8;
}

void Viewer::setMode(Mode newMode) {
  mode = newMode;
  appWindow->updateMessage(getModeString(), near, far);
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
}

void Viewer::paintGL() {
  draw_init();

  set_colour(QColor(1.0, 1.0, 1.0));

  auto viewM = viewPoint.getViewMatrix();
  auto perspectiveM = perspectiveMatrix();

  // Norms of clipping planes
  Vector3D nearNorm = {0, 0, 1};
  Vector3D farNorm = {0, 0, -1};
  Vector3D leftNorm = {1, 0, 0};
  Vector3D rightNorm = {-1, 0, 0};
  Vector3D topNorm = {0, -1, 0};
  Vector3D bottomNorm = {0, 1, 0};

  // Points on clipping planes
  Point3D nearPoint = {0, 0, near};
  Point3D farPoint = {0, 0, far};
  Point3D leftPoint = {-1, 0, 0};
  Point3D rightPoint = {1, 0, 0};
  Point3D topPoint = {0, 1, 0};
  Point3D bottomPoint = {0, -1, 0};

  for (const auto& model : {boxModel, boxGnomon, worldGnomon}) {
    auto v = model.getLines();

    for (auto& line : v) {
      // Now we have the view coordinates
      line.start = viewM * line.start;
      line.end = viewM * line.end;

      // Now we want to clip the line to the near and far planes first, to
      // avoid ambiguity. (Actually, we only need to do the near one, but
      // far is the same so why not do that too) :)
      if (!clipLine(&line, nearNorm, nearPoint) ||
          !clipLine(&line, farNorm, farPoint)) {
        continue;
      }

      // We need these because our vectors are 3D not 4D...
      auto startZ = line.start[2];
      auto endZ = line.end[2];

      // Now multiply to project it onto the near plane
      line.start = perspectiveM * line.start;
      line.end = perspectiveM * line.end;

      for (int i = 0; i < 3; ++i) {
        line.start[i] /= startZ;
        line.end[i] /= endZ;
      }

      // We can clip these afterwards
      if (!clipLine(&line, rightNorm, rightPoint) ||
          !clipLine(&line, leftNorm, leftPoint) ||
          !clipLine(&line, topNorm, topPoint) ||
          !clipLine(&line, bottomNorm, bottomPoint)) {
        continue;
      }

      auto p1 = QVector2D(line.start[0], line.start[1]);
      auto p2 = QVector2D(line.end[0], line.end[1]);
      draw_line(p1, p2);
    }
  }
}

bool Viewer::clipLine(Line3D* line, const Vector3D& norm, const Point3D& pt) {
  auto edgeCoordA = (line->start - pt).dot(norm);
  auto edgeCoordB = (line->end - pt).dot(norm);

  // Trivial
  if (edgeCoordA < 0 && edgeCoordB < 0) {
    return false;
  }
  if (edgeCoordA >= 0 && edgeCoordB >= 0) {
    return true;
  }

  auto t = edgeCoordA / (edgeCoordA - edgeCoordB);
  auto c = line->end - line->start;
  if (edgeCoordA < 0) {
    line->start = line->start + t * c;
  }
  else {
    line->end = line->start + t * c;
  }
  return true;
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
  if (L) fov += dx * FOV_FACTOR;
  // Enforce limits
  fov = std::max(MIN_FOV, std::min(MAX_FOV, fov));

  if (M) near += dx * TRANSLATE_FACTOR;
  if (R) far += dx * TRANSLATE_FACTOR;

  // Enforce limits
  near = std::max(MIN_NEAR, std::min(near, far));
  far = std::max(MIN_NEAR, std::max(near, far));
  appWindow->updateMessage(getModeString(), near, far);
}

Matrix4x4 Viewer::perspectiveMatrix() {
  auto n = near;
  auto f = far;
  auto s = std::tan(fov / 2);

  // Similar to OpenGL
  return {
    {1/s, 0, 0, 0},
    {0, 1/s, 0, 0},
    {0, 0, (f+n)/(f-n), (-2*f*n)/(f-n)},
    {0, 0, 1, 0}
  };
}

std::string Viewer::getModeString() {
  switch (mode) {
  case Mode::VIEW_ROTATE:
    return "Rotate view";
  case Mode::VIEW_TRANSLATE:
    return "Translate view";
  case Mode::VIEW_PERSPECTIVE:
    return "Perspective";
  case Mode::MODEL_ROTATE:
    return "Rotate model";
  case Mode::MODEL_TRANSLATE:
    return "Translate model";
  case Mode::MODEL_SCALE:
    return "Scale model";
  case Mode::VIEWPORT:
    return "Viewport";
  }
  // Error
  return "";
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
