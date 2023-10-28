#include "resonance_type.hpp"

#include <iostream>

ResonanceType::ResonanceType(std::string name, double mass, int charge,
                             double width)
    : ParticleType(name, mass, charge), fWidth{width} {
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