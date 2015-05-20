//William McDonald 20418145 wmcdonal
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

  enum class DrawMode { WIRE, FACE, MULTICOLOUR };

  void setMode(DrawMode mode);

  void resetView();
  void viewGame(Game* game);

 protected:
  // Events we implement

  // Called when GL is first initialized
  virtual void initializeGL();
  // Called when our window needs to be redrawn
  virtual void paintGL();
  // Called when the window is resized (formerly on_configure_event)
  virtual void resizeGL(int width, int height);
  // Called when a mouse button is pressed
  virtual void mousePressEvent (QMouseEvent * event);
  // Called when a mouse button is released
  virtual void mouseReleaseEvent (QMouseEvent * event);
  // Called when the mouse moves
  virtual void mouseMoveEvent (QMouseEvent * event);

 private:
  // we do not own this
  Game* game;

  DrawMode drawMode;

  QMatrix4x4 getCameraMatrix();
  void translateWorld(float x, float y, float z);
  void rotateWorld(float angle, float x, float y, float z);
  void scaleWorld(float x, float y, float z);
  void initializeWell();

  // Draw a cube according to current mode
  void setCubeColour(const float* colour);
  void setCubeColourById(int cubeId);
  void drawCube(const QMatrix4x4& transform, int cubeId, const float* colour);

  void updateRotVector(Qt::MouseButton button);


#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
  QOpenGLBuffer positionBuffer;
  QOpenGLBuffer colourBuffer;
  QOpenGLVertexArrayObject vao;
#else
  QGLBuffer positionBuffer;
  QGLBuffer colourBuffer;
#endif

  int mVertexLocation;
  int mMvpMatrixLocation;

  QMatrix4x4 mPerspMatrix;
  QMatrix4x4 mTransformMatrix;

  QTimer* mTimer;
  QGLShaderProgram mProgram;

  QVector3D pRotVec;
  int lastMouseX;
  int xRotD, yRotD, zRotD;
  bool rotX, rotY, rotZ;

  // Current scale
  float scale;
  // Amount to scale per pixel moved
  static const float SCALE_FACTOR;
  // Minimum and maximum scales
  static const float MIN_SCALE;
  static const float MAX_SCALE;

  // Radians to rotate per pixel moved
  static const float ROTATE_FACTOR;

  // Coordinates for a unit cube
  static const float cubeCoords[12 * 3 * 3];
  // Piece colours by id
  static const float pieceColours[7][3];

  static const float cubeColours[13][3];
};
