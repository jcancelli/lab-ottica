#include "particle_type.hpp"

#include <iostream>

ParticleType::ParticleType(std::string name, double mass, int charge)
    : fName{name}, fMass{mass}, fCharge{charge} {
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
