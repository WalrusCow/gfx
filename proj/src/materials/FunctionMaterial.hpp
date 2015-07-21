#pragma once

#include <functional>
#include <map>
#include <string>

#include "Material.hpp"

class FunctionMaterial : public Material {
 public:
  typedef std::function<Colour(double, double)> ColourMap;

  FunctionMaterial(const ColourMap& mapFunction_,
                   const Colour& ks_,
                   double shininess_,
                   double alpha_, double idx_);

  static Colour bwSquares(double xPercent, double yPercent);
  static Colour blueGreenSquares(double xPercent, double yPercent);
  static std::map<std::string, ColourMap> functions;

 protected:
  Colour getKd(const HitRecord& hitRecord) const override;

 private:
  // Function to use
  ColourMap mapFunction;
};
