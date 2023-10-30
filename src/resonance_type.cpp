#include "resonance_type.hpp"

#include <iostream>
#include <stdexcept>

ResonanceType::ResonanceType(std::string name, double mass, int charge,
                             double width)
    : ParticleType(name, mass, charge), fWidth{width} {
  if (width < 0) {
    throw std::invalid_argument("width cannot be negative");
  }
}

ResonanceType::~ResonanceType() {
}

double ResonanceType::GetWidth() const {
  return fWidth;
}

void ResonanceType::Print() const {
  ParticleType::Print();
  std::cout << "width: " << fWidth << "\n";
}