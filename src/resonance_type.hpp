#pragma once

#include "particle_type.hpp"

class ResonanceType : public ParticleType {
 private:
  const double fWidth;

 public:
  ResonanceType(std::string name, double mass, int charge, double width);
  virtual ~ResonanceType();
  virtual void Print() const;
  virtual double GetWidth() const;
};