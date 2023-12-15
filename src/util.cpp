#include "util.hpp"

#include <cmath>

void section(const char* title) {
  std::cout << "\n----- " << title << " -----\n";
}

const char* boolToString(bool b) {
  return b ? "true" : "false";
}

bool equals(double a, double b, double epsilon) {
  return std::abs(a - b) < epsilon;
}