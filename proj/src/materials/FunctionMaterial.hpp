#pragma once

#include <functional>

#include "Material.hpp"

class FunctionMaterial : public Material {
 public:
  typedef std::function<Colour(double, double)> ColourMap;

  FunctionMaterial(const ColourMap& mapFunction_,
                   const Colour& ks_,
                   double shininess_);
 protected:
  Colour getKd(const HitRecord& hitRecord);

 private:
  // Function to use
  ColourMap mapFunction;
};
