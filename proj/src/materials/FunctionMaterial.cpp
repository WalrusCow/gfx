#include "FunctionMaterial.hpp"

#include "HitRecord.hpp"

std::map<std::string, FunctionMaterial::ColourMap>
FunctionMaterial::functions = {
  {"bw_squares", FunctionMaterial::bwSquares},
  {"blue_green_squares", FunctionMaterial::blueGreenSquares},
};

FunctionMaterial::FunctionMaterial(
    const FunctionMaterial::ColourMap& mapFunction_,
    const Colour& ks_,
    double shininess_, double alpha_, double idx_)
    : Material(ks_, shininess_, alpha_, idx_), mapFunction(mapFunction_) {}

Colour FunctionMaterial::getKd(const HitRecord& hitRecord) const {
  return mapFunction(hitRecord.xPercent, hitRecord.yPercent);
}

Colour FunctionMaterial::bwSquares(double xPercent, double yPercent) {
  // We want squares of 10% with/height
  //  ---------
  //  | B | W |
  //  ---------
  //  | W | B |
  //  ---------
  int xp = (int) (xPercent * 100);
  int yp = (int) (yPercent * 100);
  int period = 6;
  bool left = (xp % period < period/2);
  bool bottom = (yp % period < period/2);
  if (left ^ bottom) {
    return Colour(1, 1, 1);
  }
  return Colour(0, 0, 0);
}

Colour FunctionMaterial::blueGreenSquares(double xPercent, double yPercent) {
  // We want squares of 10% with/height
  //  ---------
  //  | B | W |
  //  ---------
  //  | W | B |
  //  ---------
  int xp = (int) (xPercent * 100);
  int yp = (int) (yPercent * 100);
  int period = 6;
  bool left = (xp % period < period/2);
  bool bottom = (yp % period < period/2);
  if (left ^ bottom) {
    return Colour(0, 1, 0);
  }
  return Colour(0, 0, 1);
}
