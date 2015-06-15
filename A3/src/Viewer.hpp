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

  enum class Mode { POSITION, JOINTS };
  void setMode(Mode mode);

  bool redoOp();
  // Pop a matrix for when walking (or can we use normal stack?)
  bool undoOp();

  // Get the net transforms for the object with given id
  QMatrix4x4 getTransforms(int id);

 protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;
  void mousePressEvent (QMouseEvent* event) override;
  void mouseReleaseEvent (QMouseEvent* event) override;
  void mouseMoveEvent (QMouseEvent* event) override;

 private:
  // TODO: Names
  const double theta = M_PI / 40;
  const int numTriangles = 2 * (2 * M_PI)/theta * M_PI/theta;

  // For dragging
  int lastMouseX;
  int lastMouseY;

  // Ids we have currently picked
  // TODO: Picking
  std::set<int> pickedIds;
  int find_pick_id(int x, int y){(void)x;(void)y;return 0;}

  // What scene we are drawing
  std::unique_ptr<SceneNode> sceneRoot;

  // What mode are we in for operations?
  Mode currentMode = Mode::POSITION;

  // Add an operation affecting some set of ids to the undo/redo stack
  void doOp(std::set<int> ids, QMatrix4x4 matrix);

  struct OpStackEntry {
    OpStackEntry(std::set<int> ids, QMatrix4x4 matrix)
        : ids(std::move(ids)), matrix(std::move(matrix)) {}
    OpStackEntry(){}

    std::set<int> ids;
    QMatrix4x4 matrix;
  };
  std::vector<OpStackEntry> opStack;
  // Where in the stack we are
  int opStackPosition = -1;
  // Track each object's net transformations
  std::unordered_map<int, QMatrix4x4> opMap;

  // Puppet position and rotation matrices (no need for undo/redo)
  QMatrix4x4 puppetPosition;
  QMatrix4x4 puppetRotation;

  // Draw circle for trackball
  void draw_trackball_circle();
  // Set colour for OpenGL drawing
  void set_colour(const QColor& col);

  // For camera transform
  QMatrix4x4 mPerspMatrix;
  QMatrix4x4 mTransformMatrix;

  // Functions for getting/modifying camera
  QMatrix4x4 getCameraMatrix();
  void translateWorld(float x, float y, float z);
  void rotateWorld(float angle, float x, float y, float z);
  void scaleWorld(float x, float y, float z);

  // OpenGL required members
  QOpenGLBuffer mCircleBufferObject;
  QOpenGLBuffer mSphereBufferObject;
  QOpenGLVertexArrayObject mVao;
  QGLShaderProgram mProgram;
  int mMvpMatrixLocation;
  int mColorLocation;

  // Initialize openGL buffers
  void initSphereData(float* vertexBuffer, double theta);
  void initCircleData(float* buffer, double radius, double theta);
};
