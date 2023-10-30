#include "particle_type.hpp"

#include <iostream>
#include <stdexcept>

ParticleType::ParticleType(std::string name, double mass, int charge)
    : fName{name}, fMass{mass}, fCharge{charge} {
  if (mass <= 0) {
    throw std::invalid_argument("mass cannot be negative");
  }
  if (name == "") {
    throw std::invalid_argument("name cannot be empty string");
  }
}

ParticleType::~ParticleType() {
}

void ParticleType::Print() const {
  std::cout << "name: " << fName << "\n";
  std::cout << "mass: " << fMass << "\n";
  std::cout << "charge: " << fCharge << "\n";
}

std::string ParticleType::GetName() const {
  return fName;
}

double ParticleType::GetMass() const {
  return fMass;
}

int ParticleType::GetCharge() const {
  return fCharge;
}
