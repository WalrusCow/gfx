#include "Primitive.hpp"

#include <algorithm>

namespace primitives {

bool isValid(double t) {
  return t > 0 && !isZero(t);
}

double mostValid(double t1, double t2) {
  if (!isValid(t1)) {
    return t2;
  }
  if (!isValid(t2)) {
    return t1;
  }
  return std::min(t1, t2);
}

} // primitives
