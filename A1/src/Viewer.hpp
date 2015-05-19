#pragma once

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QMatrix4x4>
#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#else
#include <QGLBuffer>
#endif

#include <vector>

class Viewer : public QGLWidget {

  Q_OBJECT

 public:
  Viewer(const QGLFormat& format, QWidget *parent = 0);
  virtual ~Viewer();

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

  // If you want to render a new frame, call do not call paintGL(),
  // instead, call update() to ensure that the view gets a paint
  // event.

  enum class DrawMode { WIRE, FACE };

  void setMode(DrawMode mode);

 protected:
  // Events we implement

  // Called when GL is first initialized
  virtual void initializeGL();
  // Called when our window needs to be redrawn
  virtual void paintGL();
  // Called when the window is resized (formerly on_configure_event)
  virtual void resizeGL(int width, int height);
  // Called when a mouse button is pressed
  virtual void mousePressEvent ( QMouseEvent * event );
  // Called when a mouse button is released
  virtual void mouseReleaseEvent ( QMouseEvent * event );
  // Called when the mouse moves
  virtual void mouseMoveEvent ( QMouseEvent * event );

 private:
  QMatrix4x4 getCameraMatrix();
  void translateWorld(float x, float y, float z);
  void rotateWorld(float angle, float x, float y, float z);
  void scaleWorld(float x, float y, float z);


#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
  QOpenGLBuffer mVertexBufferObject;
  QOpenGLVertexArrayObject mVertexArrayObject;
#else
  QGLBuffer mVertexBufferObject;
#endif

  int mVertexLocation;
  int mMvpMatrixLocation;

  QMatrix4x4 mPerspMatrix;
  QMatrix4x4 mTransformMatrix;

  QTimer* mTimer;
  QGLShaderProgram mProgram;

  // Coordinates for a unit cube
  const float cubeCoords[12 * 3 * 3] = {
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
};
