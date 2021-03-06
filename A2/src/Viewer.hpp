// William McDonald 20418145 wmcdonal
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
  void initializeGL() override;
  void paintGL() override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

  // Draw a line -- call draw_init first!
  void draw_line(const Point2D& p1, const Point2D& p2) ;

  // Set the current colour
  void set_colour(const QColor& col);

  // Call this before you begin drawing. Width and height are the width
  // and height of the GL window.
  void draw_init();

 private:
  static const double SCALE_FACTOR;
  static const double ROTATE_FACTOR;
  static const double TRANSLATE_FACTOR;
  static const double FOV_FACTOR;
  static const double MIN_FOV;
  static const double MAX_FOV;
  static const double MIN_NEAR;

  static const double DEFAULT_NEAR;
  static const double DEFAULT_FAR;
  static const double DEFAULT_FOV;
  static const Point2D DEFAULT_VP1;
  static const Point2D DEFAULT_VP2;

  std::string getModeString();

  void scale(Model& model, int dx, bool L, bool M, bool R);
  void rotate(Movable& obj, int dx, bool L, bool M, bool R);
  void translate(Movable& obj, int dx, bool L, bool M, bool R);
  void changePerspective(int dx, bool L, bool M, bool R);
  void updateViewport(const Point2D& p1, const Point2D& p2);

  // Clip a line in 3D to a plane
  // Modifies the line in-place if necessary
  // Returns false if the whole line is clipped out
  bool clipLine(Line3D* line, const Vector3D& norm, const Point3D& pt);

  Matrix4x4 perspectiveMatrix();

  void getAdjustedViewportBounds(Point2D* p1, Point2D* p2);
  Point2D adjustForViewport(
      const Point2D& pt, const Point2D& vp1, const Point2D& vp2);

  double near = DEFAULT_NEAR;
  double far = DEFAULT_FAR;
  double fov = DEFAULT_FOV;

  QOpenGLBuffer mVertexBufferObject;
  QOpenGLVertexArrayObject mVertexArrayObject;
  QGLShaderProgram mProgram;
  int mColorLocation;

  AppWindow* appWindow;
  QTimer* refreshTimer;
  Mode mode = Mode::MODEL_ROTATE;

  // Last mouse coordinates received during drag
  int lastMouseX;

  // Whether or not we are dragging for viewport
  bool vpDrag;
  // Viewport points (in pixels)
  Point2D vpDragStart;
  Point2D vpDragEnd;
  Point2D vp1 = DEFAULT_VP1;
  Point2D vp2 = DEFAULT_VP2;

  ViewPoint viewPoint;

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
};
