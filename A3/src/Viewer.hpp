// William McDonald 20418145 wmcdonal
#pragma once

#include <cmath>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QMatrix4x4>
#include <QtGlobal>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include "scene.hpp"

class Viewer : public QGLWidget {

  Q_OBJECT

 public:
  Viewer(const QGLFormat& format,
         std::unique_ptr<SceneNode> sceneRoot,
         QWidget *parent = 0);
  virtual ~Viewer();

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

  struct OpStackEntry {
    OpStackEntry(std::set<int> ids, QMatrix4x4 matrix)
        : ids(std::move(ids)), matrix(std::move(matrix)) {}
    OpStackEntry(){}

    std::set<int> ids;
    QMatrix4x4 matrix;
  };

  enum class Mode { POSITION, JOINTS };
  Mode currentMode = Mode::POSITION;
  void setMode(Mode mode);
  int lastMouseX;
  int lastMouseY;

  // Ids we have currently picked
  std::set<int> pickedIds;

  // TODO
  int find_pick_id(int x, int y){(void)x;(void)y;return 0;}

  // Push a matrix for when walking (or can we use normal stack?)
  // Let's see... Each node will simply push, do, pop. So that should be ok..
  void doOp(std::set<int> ids, QMatrix4x4 matrix);
  void redoOp();
  // Pop a matrix for when walking (or can we use normal stack?)
  void undoOp();

  // Also we want a stack of the list (could use a vector here)
  // TODO: Make private
  std::vector<OpStackEntry> opStack;
  // Map ids to matrices
  std::unordered_map<int, QMatrix4x4> opMap;
  // Where in the stack we are
  int opStackPosition = -1;

  QMatrix4x4 getTransforms(int id);

  // TODO: This must be w.r.t. world coordinates
  QMatrix4x4 puppetPosition;
  // TODO: How to handle this properly???
  // Undo then redo?
  QMatrix4x4 puppetRotation;

 protected:
  virtual void initializeGL();
  virtual void paintGL();
  virtual void resizeGL(int width, int height);
  virtual void mousePressEvent ( QMouseEvent * event );
  virtual void mouseReleaseEvent ( QMouseEvent * event );
  virtual void mouseMoveEvent ( QMouseEvent * event );

  // Draw a circle for the trackball, with OpenGL commands.
  // Assumes the context for the viewer is active.
  void draw_trackball_circle();

 private:
  const double theta = M_PI / 40;
  const int numTriangles = 2 * (2 * M_PI)/theta * M_PI/theta;

  std::unique_ptr<SceneNode> sceneRoot;

  QMatrix4x4 getCameraMatrix();
  void translateWorld(float x, float y, float z);
  void rotateWorld(float angle, float x, float y, float z);
  void scaleWorld(float x, float y, float z);
  void set_colour(const QColor& col);

  void initSphereData(float* vertexBuffer, double theta);
  void initCircleData(float* buffer, double radius, double theta);

  QOpenGLBuffer mCircleBufferObject;
  QOpenGLBuffer mSphereBufferObject;
  QOpenGLVertexArrayObject mVao;

  int mMvpMatrixLocation;
  int mColorLocation;

  QMatrix4x4 mPerspMatrix;
  QMatrix4x4 mTransformMatrix;
  QGLShaderProgram mProgram;
};
