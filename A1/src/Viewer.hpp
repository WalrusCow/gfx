#pragma once

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QMatrix4x4>
#include <QtGlobal>

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include "game.hpp"

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

  void resetView();
  void viewGame(Game* game);

 protected:
  // Events we implement

  // Called when GL is first initialized
  virtual void initializeGL() override;
  // Called when our window needs to be redrawn
  virtual void paintGL() override;
  // Called when the window is resized (formerly on_configure_event)
  virtual void resizeGL(int width, int height) override;
  // Called when a mouse button is pressed
  virtual void mousePressEvent (QMouseEvent * event) override;
  // Called when a mouse button is released
  virtual void mouseReleaseEvent (QMouseEvent * event) override;
  // Called when the mouse moves
  virtual void mouseMoveEvent (QMouseEvent * event) override;

 private:
  // we do not own this
  Game* game = nullptr;

  QMatrix4x4 getCameraMatrix();
  void translateWorld(float x, float y, float z);
  void rotateWorld(float angle, float x, float y, float z);
  void rotateWorld(float angle, const QVector3D& vector);
  void scaleWorld(float x, float y, float z);
  void initializeWell();

  // Draw a cube according to current mode
  void drawCube(const QMatrix4x4& transform, float* colour);

  void updateRotVector(Qt::MouseButton button, bool release);

  QOpenGLBuffer positionBuffer;
  QOpenGLBuffer colourBuffer;
  QOpenGLVertexArrayObject vao;

  int mVertexLocation;
  int mMvpMatrixLocation;

  QMatrix4x4 mPerspMatrix;
  QMatrix4x4 mTransformMatrix;

  QTimer* mTimer;
  QGLShaderProgram mProgram;

  bool scaling = false;

  QVector3D pRotVec;
  int lastMouseX;
  int pRotDx;

  // Current scale
  float scale = 1.0f;
  // Amount to scale per pixel moved
  const float SCALE_FACTOR = 1.005;
  // Minimum and maximum scales
  const float MIN_SCALE = pow(SCALE_FACTOR, -400);
  const float MAX_SCALE = pow(SCALE_FACTOR, 100);

  // Radians to rotate per pixel moved
  const float ROTATE_FACTOR = M_PI / 15;

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

  // Colors for a unit cube
  const float cubeColours[12 * 3 * 3] = {0};

  // Piece colours by id
  const float pieceColours[7][3] = {
    {1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 0.0, 1.0},
    {1.0, 1.0, 0.0},
    {0.0, 1.0, 1.0},
    {1.0, 0.0, 1.0},
    {1.0, 1.0, 1.0}
  };
};
