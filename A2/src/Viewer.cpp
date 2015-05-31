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

Viewer::Viewer(const QGLFormat& format, QWidget *parent)
  : QGLWidget(format, parent),
    mVertexBufferObject(QOpenGLBuffer::VertexBuffer),
    mVertexArrayObject(this) {
  // Nothing to do here right now.
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

void Viewer::reset_view() {
  // Fill me in!
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

  if ( !mProgram.link() ) {
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

  // Here is where your drawing code should go.

  /* A few of lines are drawn below to show how it's done. */
  set_colour(QColor(1.0, 1.0, 1.0));

  draw_line(QVector2D(-0.9, -0.9),
        QVector2D(0.9, 0.9));
  draw_line(QVector2D(0.9, -0.9),
        QVector2D(-0.9, 0.9));

  draw_line(QVector2D(-0.9, -0.9),
        QVector2D(-0.4, -0.9));
  draw_line(QVector2D(-0.9, -0.9),
        QVector2D(-0.9, -0.4));
}

void Viewer::mousePressEvent(QMouseEvent* event) {
  std::cerr << "Stub: button " << event->button() << " pressed\n";

  const auto v = boxModel.getLines();
  std::cerr << "before:"<<v[0].start << std::endl;
  boxModel.rotateX(M_PI);
  boxModel.translate(0.0, 1.0, 0.0);
  boxModel.rotateX(M_PI / 2);
  const auto w = boxModel.getLines();
  std::cerr << "after:"<<w[0].start << std::endl;
}

void Viewer::mouseReleaseEvent(QMouseEvent* event) {
  std::cerr << "Stub: button " << event->button() << " released\n";
}

void Viewer::mouseMoveEvent(QMouseEvent* event) {
  std::cerr << "Stub: Motion at " << event->x() << ", " << event->y() << std::endl;
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
