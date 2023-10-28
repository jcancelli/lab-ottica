#include "particle.hpp"

#include <cmath>
#include <iostream>

#include "resonance_type.hpp"

ParticleType* Particle::fParticleTypes[Particle::fMaxNumParticleType];
int Particle::fNParticleType = 0;

Particle::Particle(std::string name, double px, double py, double pz)
    : fIndex{FindParticle(name)}, fPx{px}, fPy{py}, fPz{pz} {
}

void Particle::AddParticleType(std::string name, double mass, int charge,
                               double width) {
  int index = FindParticle(name);
  if (index == -1) {  // particle does not exist
    if (fNParticleType == fMaxNumParticleType) {
      std::cout << "Maximum amount of particle types reached.\n";
      return;
    }
    index = fNParticleType;
    fNParticleType++;
  } else {
    delete fParticleTypes[index];
  }
  fParticleTypes[index] = width == 0.0
                              ? new ParticleType(name, mass, charge)
                              : new ResonanceType(name, mass, charge, width);
}

void Particle::PrintParticleTypes() {
  for (auto const& type : fParticleTypes) {
    std::cout << "--------------\n";
    type->Print();
  }
  std::cout << "--------------\n";
}

double Particle::TotalEnergy() const {
  const auto p = std::hypot(fPx, fPy, fPz);
  const auto m = GetMass();
  return std::hypot(m, p);
}

double Particle::InvMass(Particle const& p) const {
  const auto eSumSquared = std::pow(TotalEnergy() + p.TotalEnergy(), 2);
  const auto pX = fPx + p.GetPulseX(), pY = fPy + p.GetPulseY(),
             pZ = fPz + p.GetPulseZ();
  const auto pulseSquared = pX * pX + pY * pY + pZ * pZ;
  return std::sqrt(eSumSquared - pulseSquared);
}

void Particle::Print() const {
  std::cout << "Type index: " << fIndex << "\n";
  std::cout << "Name: \"" << fParticleTypes[fIndex]->GetName() << "\"\n";
  std::cout << "Pulse: { x: " << fPx << ", y: " << fPy << ", z: " << fPz
            << " }\n";
}

int Particle::GetParticleType() const {
  return fIndex;
}

void Particle::SetParticleType(std::string name) {
  int index = FindParticle(name);
  if (index == -1) {
    std::cout << "No particle type named: \"" << name << "\"\n";
    return;
  }
  fIndex = index;
}

void Particle::SetParticleType(int index) {
  if (index < 0 || index >= fNParticleType) {
    std::cout << "No particle type with index: " << index << "\n";
    return;
  }
  fIndex = index;
}

int Particle::GetPulseX() const {
  return fPx;
}

int Particle::GetPulseY() const {
  return fPy;
}

int Particle::GetPulseZ() const {
  return fPz;
}

void Particle::SetP(double x, double y, double z) {
  fPx = x;
  fPy = y;
  fPz = z;
}

double Particle::GetMass() const {
  return fParticleTypes[fIndex]->GetMass();
}

int Particle::FindParticle(std::string const& name) {
  ParticleType* type = nullptr;
  for (int i = 0; i < Particle::fNParticleType; i++) {
    type = Particle::fParticleTypes[i];
    if (type->GetName() == name) {
      return i;
    }
  }
  std::cout << "No particle named: " << name << "\n";
  return -1;
}