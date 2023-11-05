#pragma once

#include <string>

class ParticleType {
 private:
  const std::string fName;
  const double fMass;
  const int fCharge;

 public:
  ParticleType(std::string name, double mass, int charge);
  virtual ~ParticleType();
  virtual void Print() const;
  virtual double GetWidth() const;
  std::string GetName() const;
  double GetMass() const;
  int GetCharge() const;
};