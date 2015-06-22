#include "PixelTransformer.hpp"

#include <cmath>

#include "xform.hpp"
#include "ViewConfig.hpp"

PixelTransformer::PixelTransformer(int width, int height,
                                   const ViewConfig& viewConfig) {
  // How far we are away
  double d = viewConfig.view.length();
  // This is height in world units of near plane
  double h = 2 * d * std::tan(viewConfig.fov / 2);

  // Instead of going from (0, 0) to (width, height) we want
  // to go from (-width/2, -height/2) to (width/2, height/2)
  auto T1 = translationMatrix(-width/2.0, -height/2.0, d);

  // Now scale to world size from image size
  // Note: x is negative because we are going from a right handed system
  // to a left handed one
  auto S = scaleMatrix(-h/width, h/height, 1);
  // After doing S * T1 * pixel we get the coordinate in view coordinates
  // (plus a negative sign on x)

  // Now we rotate to superimpose WCS to VCS
  auto w = viewConfig.view;
  w.normalize();

  auto u = viewConfig.up.cross(w);
  u.normalize();

  auto v = w.cross(u);
  v.normalize();

  // Manual rotation matrix is easier :)
  Matrix4x4 R(
      {u[0], v[0], w[0], 0},
      {u[1], v[1], w[1], 0},
      {u[2], v[2], w[2], 0},
      {0,    0,    0,    1}
  );

  // Lastly, translate by the eye coordinates
  const auto& eye = viewConfig.eye;
  auto T2 = translationMatrix(eye[0], eye[1], eye[2]);
  transformMatrix = T2 * R * S * T1;
}

Point3D PixelTransformer::transform(int x, int y) {
  return transformMatrix * Point3D(x, y, 0);
}
