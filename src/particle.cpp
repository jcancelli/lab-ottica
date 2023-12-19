#include "particle.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>

#include "resonance_type.hpp"

ParticleType* Particle::fParticleTypes[Particle::fMaxNumParticleType];
int Particle::fNParticleType = 0;

Particle::Particle()
    : fIndex{Particle::INVALID_TYPE}, fPx{0.}, fPy{0.}, fPz{0.} {
}

Particle::Particle(std::string name, double px, double py, double pz)
    : fIndex{FindParticle(name)}, fPx{px}, fPy{py}, fPz{pz} {
}

int Particle::AddParticleType(std::string name, double mass, int charge,
                              double width) {
  int index = FindParticle(name);
  if (index == Particle::INVALID_TYPE) {  // particle does not exist
    if (fNParticleType == fMaxNumParticleType) {
      throw std::runtime_error("Maximum amount of particle types reached.\n");
    }
    index = fNParticleType;
    fNParticleType++;
    std::cout << "Adding new particle type named: \"" << name << "\"\n";
  } else {
    delete fParticleTypes[index];
    std::cout << "Editing particle type named: \"" << name << "\"\n";
  }
  fParticleTypes[index] = width == 0.0
                              ? new ParticleType(name, mass, charge)
                              : new ResonanceType(name, mass, charge, width);
  return index;
}

void Particle::PrintParticleTypes() {
  for (auto const& type : fParticleTypes) {
    std::cout << "--------------\n";
    type->Print();
  }
  std::cout << "--------------\n";
}

void Particle::Decay2body(Particle& dau1, Particle& dau2) const {
  if (GetMass() == 0.0) {
    throw std::runtime_error("Decayment cannot be preformed if mass is zero");
  }

  double massMot = GetMass();
  double massDau1 = dau1.GetMass();
  double massDau2 = dau2.GetMass();

  if (IsOfValidType()) {  // add width effect
    // gaussian random numbers
    float x1, x2, w, y1;

    double invnum = 1. / RAND_MAX;
    do {
      x1 = 2.0 * rand() * invnum - 1.0;
      x2 = 2.0 * rand() * invnum - 1.0;
      w = x1 * x1 + x2 * x2;
    } while (w >= 1.0);

    w = sqrt((-2.0 * log(w)) / w);
    y1 = x1 * w;

    massMot += fParticleTypes[fIndex]->GetWidth() * y1;
  }

  if (massMot < massDau1 + massDau2) {
    throw std::runtime_error(
        "Decayment cannot be preformed because mass is too low in this "
        "channel");
  }

  double pout =
      sqrt(
          (massMot * massMot - (massDau1 + massDau2) * (massDau1 + massDau2)) *
          (massMot * massMot - (massDau1 - massDau2) * (massDau1 - massDau2))) /
      massMot * 0.5;

  double norm = 2 * M_PI / RAND_MAX;

  double phi = rand() * norm;
  double theta = rand() * norm * 0.5 - M_PI / 2.;
  dau1.SetP(pout * sin(theta) * cos(phi), pout * sin(theta) * sin(phi),
            pout * cos(theta));
  dau2.SetP(-pout * sin(theta) * cos(phi), -pout * sin(theta) * sin(phi),
            -pout * cos(theta));

  double energy = sqrt(fPx * fPx + fPy * fPy + fPz * fPz + massMot * massMot);

  double bx = fPx / energy;
  double by = fPy / energy;
  double bz = fPz / energy;

  dau1.Boost(bx, by, bz);
  dau2.Boost(bx, by, bz);
}

double Particle::TotalEnergy() const {
  const auto p = std::hypot(fPx, fPy, fPz);
  const auto m = GetMass();
  return std::hypot(m, p);
}

double Particle::InvMass(Particle const& p) const {
  const auto eSumSquared = std::pow(TotalEnergy() + p.TotalEnergy(), 2);
  const auto pX = fPx + p.fPx, pY = fPy + p.fPy, pZ = fPz + p.fPz;
  const auto pulseSquared = pX * pX + pY * pY + pZ * pZ;
  return std::sqrt(eSumSquared - pulseSquared);
}

void Particle::Print() const {
  std::cout << "Type index: " << fIndex << "\n";
  if (IsOfValidType()) {
    std::cout << "Name: \"" << fParticleTypes[fIndex]->GetName() << "\"\n";
  } else {
    std::cout << "Name: <invalid-particle-type>\n";
  }
  std::cout << "Pulse: { x: " << fPx << ", y: " << fPy << ", z: " << fPz
            << " }\n";
}

bool Particle::IsOfValidType() const {
  return fIndex != Particle::INVALID_TYPE;
}

int Particle::GetParticleType() const {
  return fIndex;
}

void Particle::SetParticleType(std::string name) {
  int index = FindParticle(name);
  if (index == Particle::INVALID_TYPE) {
    std::cout << "No particle type named: \"" << name << "\"\n";
    return;
  }
  fIndex = index;
}

void Particle::SetParticleType(int index) {
  if (index < 0 || index >= fNParticleType) {
    throw std::invalid_argument("No particle type with specified index\n");
  }
  fIndex = index;
}

double Particle::GetPulseX() const {
  return fPx;
}

double Particle::GetPulseY() const {
  return fPy;
}

double Particle::GetPulseZ() const {
  return fPz;
}

void Particle::SetP(double x, double y, double z) {
  fPx = x;
  fPy = y;
  fPz = z;
}

double Particle::GetMass() const {
  if (!IsOfValidType()) {
    throw std::runtime_error("Cannot read mass of invalid particle type");
  }
  return fParticleTypes[fIndex]->GetMass();
}

double Particle::GetCharge() const {
  if (!IsOfValidType()) {
    throw std::runtime_error("Cannot read charge of invalid particle type");
  }
  return fParticleTypes[fIndex]->GetCharge();
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
  return Particle::INVALID_TYPE;
}

void Particle::Boost(double bx, double by, double bz) {
  double energy = TotalEnergy();

  // Boost this Lorentz vector
  double b2 = bx * bx + by * by + bz * bz;
  double gamma = 1.0 / sqrt(1.0 - b2);
  double bp = bx * fPx + by * fPy + bz * fPz;
  double gamma2 = b2 > 0 ? (gamma - 1.0) / b2 : 0.0;

  fPx += gamma2 * bp * bx + gamma * bx * energy;
  fPy += gamma2 * bp * by + gamma * by * energy;
  fPz += gamma2 * bp * bz + gamma * bz * energy;
}