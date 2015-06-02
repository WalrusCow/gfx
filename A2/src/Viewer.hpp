#pragma once

#include <string>

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QtGlobal>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include "Model.hpp"
#include "ViewPoint.hpp"

class AppWindow;

class Viewer : public QGLWidget {

  Q_OBJECT

 public:
  Viewer(const QGLFormat& format, QWidget *parent = 0);
  virtual ~Viewer();

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

  // Restore all the transforms and perspective parameters to their
  // original state. Set the viewport to its initial size.
  void resetView();

  enum class Mode {
    VIEW_ROTATE, VIEW_TRANSLATE, VIEW_PERSPECTIVE,
    MODEL_ROTATE, MODEL_TRANSLATE, MODEL_SCALE,
    VIEWPORT,
  };

  void setMode(Mode newMode);


 protected:
  virtual void initializeGL();
  virtual void paintGL();
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseReleaseEvent(QMouseEvent* event);
  virtual void mouseMoveEvent(QMouseEvent* event);

  // Draw a line -- call draw_init first!
  void draw_line(const QVector2D& p1, const QVector2D& p2) ;

  // Set the current colour
  void set_colour(const QColor& col);

  // Call this before you begin drawing. Width and height are the width
  // and height of the GL window.
  void draw_init();

 private:
  std::string getModeString();

  void scale(Model& model, int dx, bool L, bool M, bool R);
  void rotate(Movable& obj, int dx, bool L, bool M, bool R);
  void translate(Movable& obj, int dx, bool L, bool M, bool R);
  void changePerspective(int dx, bool L, bool M, bool R);

  // Clip a line in 3D to a plane
  // Modifies the line in-place if necessary
  // Returns false if the whole line is clipped out
  bool clipLine(Line3D* line, const Vector3D& norm, const Point3D& pt);

  Matrix4x4 perspectiveMatrix();

  double near = 3;
  double far = 8;
  double fov = M_PI / 4;

  static const double SCALE_FACTOR;
  static const double ROTATE_FACTOR;
  static const double TRANSLATE_FACTOR;
  static const double FOV_FACTOR;
  static const double MIN_FOV;
  static const double MAX_FOV;
  static const double MIN_NEAR;

  QOpenGLBuffer mVertexBufferObject;
  QOpenGLVertexArrayObject mVertexArrayObject;
  QGLShaderProgram mProgram;
  int mColorLocation;

  AppWindow* appWindow;

  QTimer* refreshTimer;
  Mode mode = Mode::MODEL_ROTATE;
  int lastMouseX;

  Model boxModel = Model("Box", {
      // "Front"
      {{1.0, 1.0, 1.0}, {-1.0, 1.0, 1.0}},
      {{-1.0, 1.0, 1.0}, {-1.0, -1.0, 1.0}},
      {{-1.0, -1.0, 1.0}, {1.0, -1.0, 1.0}},
      {{1.0, -1.0, 1.0}, {1.0, 1.0, 1.0}},

      // "Back"
      {{1.0, 1.0, -1.0}, {-1.0, 1.0, -1.0}},
      {{-1.0, 1.0, -1.0}, {-1.0, -1.0, -1.0}},
      {{-1.0, -1.0, -1.0}, {1.0, -1.0, -1.0}},
      {{1.0, -1.0, -1.0}, {1.0, 1.0, -1.0}},

      // Middle lines
      {{1.0, 1.0, 1.0}, {1.0, 1.0, -1.0}},
      {{-1.0, 1.0, 1.0}, {-1.0, 1.0, -1.0}},
      {{-1.0, -1.0, 1.0}, {-1.0, -1.0, -1.0}},
      {{1.0, -1.0, 1.0}, {1.0, -1.0, -1.0}},
  });

  Model boxGnomon = Model("BoxGnomon", {
      {{0, 0, 0}, {0.5, 0, 0}},
      {{0, 0, 0}, {0, 0.5, 0}},
      {{0, 0, 0}, {0, 0, 0.5}}
  });

  Model worldGnomon = Model("WorldGnomon", {
      {{0, 0, 0}, {0.5, 0, 0}},
      {{0, 0, 0}, {0, 0.5, 0}},
      {{0, 0, 0}, {0, 0, 0.5}}
  });

  ViewPoint viewPoint;
};
