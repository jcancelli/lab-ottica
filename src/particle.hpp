#pragma once

#include <string>

#include "particle_type.hpp"

class Particle {
 private:
  static const int fMaxNumParticleType = 10;
  static ParticleType* fParticleTypes[fMaxNumParticleType];
  static int fNParticleType;

  int fIndex;
  double fPx, fPy, fPz;

 public:
  static const int INVALID_TYPE = -1;

 public:
  Particle();
  Particle(std::string name, double fPx = 0.0, double fPy = 0.0,
           double fPz = 0.0);
  static int AddParticleType(std::string name, double mass, int charge,
                             double width = 0.0);
  static void PrintParticleTypes();
  int Decay2body(Particle& dau1, Particle& dau2) const;
  double TotalEnergy() const;
  double InvMass(Particle const& p) const;
  void Print() const;
  bool IsOfValidType() const;
  int GetParticleType() const;
  void SetParticleType(std::string name);
  void SetParticleType(int index);
  int GetPulseX() const;
  int GetPulseY() const;
  int GetPulseZ() const;
  void SetP(double x, double y, double z);
  double GetMass() const;
  double GetCharge() const;

 private:
  static int FindParticle(std::string const& name);
  void Boost(double bx, double by, double bz);
};
