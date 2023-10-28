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
  Particle(std::string name, double fPx = 0.0, double fPy = 0.0,
           double fPz = 0.0);
  static void AddParticleType(std::string name, double mass, int charge,
                              double width = 0.0);
  static void PrintParticleTypes();
  double TotalEnergy() const;
  double InvMass(Particle const& p) const;
  void Print() const;
  int GetParticleType() const;
  void SetParticleType(std::string name);
  void SetParticleType(int index);
  int GetPulseX() const;
  int GetPulseY() const;
  int GetPulseZ() const;
  void SetP(double x, double y, double z);
  double GetMass() const;

 private:
  static int FindParticle(std::string const& name);
};
