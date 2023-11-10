#include <TFile.h>
#include <TH1D.h>
#include <TRandom.h>

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "particle.hpp"
#include "particle_type.hpp"
#include "resonance_type.hpp"

#define N_EVENTS 1E5
#define N_PARTICLES 100

const double PI2 = 2 * M_PI;

inline const char* determineParticleType();

int main() {
  const int pioneP = Particle::AddParticleType("pione+", 0.13957, 1);
  const int pioneN = Particle::AddParticleType("pione-", 0.13957, -1);
  const int kaoneP = Particle::AddParticleType("kaone+", 0.49367, 1);
  const int kaoneN = Particle::AddParticleType("kaone-", 0.49367, -1);
  const int protoneP = Particle::AddParticleType("protone+", 0.93827, 1);
  const int protoneN = Particle::AddParticleType("protone-", 0.93827, -1);
  const int kStar = Particle::AddParticleType("k*", 0.89166, 0, 0.05);

  gRandom->SetSeed();

  std::vector<Particle> eventParticles;
  double phi, theta, pulse;
  double px, py, pz;
  TH1D particleTypesHisto("particle-types-distribution",
                          "Particle types distribution", 10, -1, 10);
  TH1D zenithDist("zenith-distribution", "Zenith distribution", 180, 0., M_PI),
      azimuthDist("azimuth-distribution", "Azimuth distribution", 360, 0., PI2);
  TH1D pulseDist("pulse-distribution", "Pulse distribution", 100, 0, 11),
      traversePulseDist("traverse-pulse-distribution",
                        "Traverse pulse distribution", 100, 0., 10.),
      particleEnergyDist("particle-energy-distribution",
                         "Particle energy distribution", 100, 0., 10.);
  TH1D invMassDist("invariant-mass-distribution", "Invariant mass distribution",
                   40, 0, 10);
  TH1D invMassDiffChargeDist;
  TH1D invMassSameChargeDist;
  TH1D invMassPioneKaoneDiscordantDist;
  TH1D invMassPioneKaoneConcordantDist;
  TH1D invMassSibDecayDist("invariant-mass-siblings-distribution",
                           "Invariant mass siblings distribution", 100, 0, 7);

  invMassDist.Sumw2();
  invMassDiffChargeDist.Sumw2();
  invMassSameChargeDist.Sumw2();
  invMassPioneKaoneDiscordantDist.Sumw2();
  invMassPioneKaoneConcordantDist.Sumw2();
  invMassSibDecayDist.Sumw2();

  std::cout << "Running simulation\n";
  for (int i = 0; i < N_EVENTS; i++) {
    while (eventParticles.size() <= N_PARTICLES) {
      phi = gRandom->Uniform(0., PI2);
      theta = gRandom->Uniform(0., M_PI);
      pulse = gRandom->Exp(1);

      // compute pulse components
      px = pulse * sin(theta) * cos(phi);
      py = pulse * sin(theta) * sin(phi);
      pz = pulse * cos(theta);

      Particle particle(determineParticleType(), px, py, pz);

      // fill histos
      particleTypesHisto.Fill(particle.GetParticleType());
      zenithDist.Fill(theta);
      azimuthDist.Fill(phi);
      pulseDist.Fill(pulse);
      traversePulseDist.Fill(hypot(px, py));
      particleEnergyDist.Fill(particle.TotalEnergy());

      // handle eventual decay and add particle(s) to vector
      if (particle.GetParticleType() == kStar) {
        if (gRandom->Rndm() < 0.5) {
          eventParticles.push_back(Particle("pione+", px, py, pz));
          eventParticles.push_back(Particle("kaone-", px, py, pz));
        } else {
          eventParticles.push_back(Particle("pione-", px, py, pz));
          eventParticles.push_back(Particle("kaone+", px, py, pz));
        }
        const auto& a = eventParticles[eventParticles.size() - 1];
        const auto& b = eventParticles[eventParticles.size() - 2];
        invMassSibDecayDist.Fill(a.InvMass(b));
      } else {
        eventParticles.push_back(particle);
      }
    }

    // fill histos
    const int n = eventParticles.size();
    for (int i = 0; i < n; i++) {
      const auto& a = eventParticles[i];
      for (int j = i + 1; i < n; i++) {
        const auto& b = eventParticles[j];

        // compute invariant mass
        const double invMass = a.InvMass(b);
        invMassDist.Fill(invMass);

        // fill inv mass histos based on discordant/concordant charge
        if (a.GetCharge() == -b.GetCharge()) {
          invMassDiffChargeDist.Fill(invMass);
        } else {
          invMassSameChargeDist.Fill(invMass);
        }

        // fill inv mass histos for pione-kaone pairs
        if ((a.GetParticleType() == pioneP && b.GetParticleType() == kaoneN) ||
            (a.GetParticleType() == pioneN && b.GetParticleType() == kaoneP)) {
          invMassPioneKaoneDiscordantDist.Fill(invMass);
        } else if ((a.GetParticleType() == pioneP &&
                    b.GetParticleType() == kaoneP) ||
                   (a.GetParticleType() == pioneN &&
                    b.GetParticleType() == kaoneN)) {
          invMassPioneKaoneConcordantDist.Fill(invMass);
        }
      }
    }

    eventParticles.clear();
  }

  // save histos to file
  std::cout << "Saving to file\n";
  TFile saveFile("histos.root", "RECREATE");

  particleTypesHisto.Write("particle-types-distribution");
  zenithDist.Write("zenith-distribution");
  azimuthDist.Write("azimuth-distribution");
  pulseDist.Write("pulse-distribution");
  traversePulseDist.Write("traverse-pulse-distribution");
  particleEnergyDist.Write("particle-energy-distribution");
  invMassDist.Write("invariant-mass-distribution");
  invMassDiffChargeDist.Write("invariant-mass-discordant-charge-distribution");
  invMassSameChargeDist.Write("invariant-mass-concordant-charge-distribution");
  invMassPioneKaoneDiscordantDist.Write(
      "invariant-mass-discordant-charge-pione-kaone-distribution");
  invMassPioneKaoneConcordantDist.Write(
      "invariant-mass-discordant-charge-pione-kaone-distribution");
  invMassSibDecayDist.Write("invariant-mass-siblings-distribution");

  saveFile.Close();
}

inline const char* determineParticleType() {
  double particleTypeProbability = gRandom->Rndm();
  if (particleTypeProbability < 0.4) {
    return "pione+";
  } else if (particleTypeProbability < 0.8) {
    return "pione-";
  } else if (particleTypeProbability < 0.85) {
    return "kaone+";
  } else if (particleTypeProbability < 0.9) {
    return "kaone-";
  } else if (particleTypeProbability < 0.945) {
    return "protone+";
  } else if (particleTypeProbability < 0.99) {
    return "protone-";
  } else {
    return "k*";
  }
}