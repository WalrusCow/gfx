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
#include "material.hpp"

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
  void resetPuppetPosition();
  void resetPuppetOrientation();
  void resetJoints();
  void toggleShowCircle();
  void toggleZBuffer();
  void toggleFrontfaceCull();
  void toggleBackfaceCull();

  bool redoOp();
  // Pop a matrix for when walking (or can we use normal stack?)
  bool undoOp();

  // Get the net transforms for the object with given id
  QMatrix4x4 getTransforms(int id);

  // Draw a sphere with given transform and colour
  void drawSphere(const QMatrix4x4& transform);

  void pushWalkMatrix(const QMatrix4x4& matrix);
  void popWalkMatrix();
  QMatrix4x4 getWalkMatrix();

  void setColour(const QColor& colour);
  // These are the reflectance of the object.
  // It is assumed that ka = 1
  void setDiffuseColour(const QColor& colour);
  void setSpecularColour(const QColor& colour);
  void setShininess(const double shininess);

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

  bool zBuffer = false;
  bool backfaceCull = false;
  bool frontfaceCull = false;
  bool showCircle = false;

  // For dragging
  int lastMouseX;
  int lastMouseY;

  // For walking
  std::list<QMatrix4x4> walkStack;

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

  // For camera transform
  QMatrix4x4 mPerspMatrix;
  QMatrix4x4 mTransformMatrix;

  // Get camera matrix
  QMatrix4x4 getCameraMatrix();

  // OpenGL required members
  QOpenGLBuffer mCircleBufferObject;
  QOpenGLBuffer mSphereBufferObject;
  QOpenGLBuffer mSphereNormalBuffer;
  QOpenGLVertexArrayObject mVao;
  QGLShaderProgram mProgram;
  int mvpMatrixLoc, mvMatrixLoc, normMatrixLoc, lightPositionLoc;
  int ambientLightLoc, diffuseColourLoc, specularColourLoc, shininessLoc;
  int colourLoc;

  // Initialize openGL buffers
  void initSphereData(float* vertexBuffer, float* normBuffer, double theta);
  void initCircleData(float* buffer, double radius, double theta);

  // Rotate matrix mat by moving from start to end along trackball
  void trackballRotate(const QVector2D& startCoords,
                       const QVector2D& endCoords,
                       QMatrix4x4* mat);

  void updateFaceCulling();
};
