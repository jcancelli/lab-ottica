#define PRINT_TEST_TITLE(text) \
  std::cout << "\n------------------\n" << text << "\n------------------\n";

#include <iostream>

#include "particle.hpp"
#include "particle_type.hpp"
#include "resonance_type.hpp"

int main() {
  PRINT_TEST_TITLE("Test getters, const correctness and Print")
  const ParticleType alessia("alessia", 1000, 1);
  std::cout << alessia.GetName() << "\n";
  std::cout << alessia.GetMass() << "\n";
  std::cout << alessia.GetCharge() << "\n";
  alessia.Print();

  const ResonanceType michele("michele", 75, 0, 20);
  std::cout << michele.GetName() << "\n";
  std::cout << michele.GetMass() << "\n";
  std::cout << michele.GetCharge() << "\n";
  std::cout << michele.GetWidth() << "\n";
  michele.Print();

  PRINT_TEST_TITLE("Test virtual");
  ParticleType const* party[2];
  party[0] = &alessia;
  party[1] = &michele;
  for (auto const& ptr : party) {
    ptr->Print();
    std::cout << "\n";
  }

  PRINT_TEST_TITLE("Test AddParticleType and FindPadrticleType");
  Particle::AddParticleType("J", 87, 60);
  Particle::AddParticleType("M", 11, 99, 85.3);
  Particle j("J", 12, 33, 55);
  Particle m("M", 67, 99, 77);
  Particle invalid("PIPPO", 22, 22, 22);
}