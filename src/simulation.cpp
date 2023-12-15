#include <TFile.h>
#include <TH1D.h>
#include <TRandom.h>

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "constants.hpp"
#include "particle.hpp"
#include "particle_type.hpp"
#include "resonance_type.hpp"
#include "util.hpp"

const double PI2 = 2 * M_PI;

inline const char* determineParticleType();

int main() {
  section("Initializing");
  // create particle types and cache their index/id localy
  const int pioneP = Particle::AddParticleType("pione+", 0.13957, 1);
  const int pioneN = Particle::AddParticleType("pione-", 0.13957, -1);
  const int kaoneP = Particle::AddParticleType("kaone+", 0.49367, 1);
  const int kaoneN = Particle::AddParticleType("kaone-", 0.49367, -1);
  const int protoneP = Particle::AddParticleType("protone+", 0.93827, 1);
  const int protoneN = Particle::AddParticleType("protone-", 0.93827, -1);
  const int kStar = Particle::AddParticleType("k*", 0.89166, 0, 0.05);

  // get rid of unused variable warning
  (void)protoneP;
  (void)protoneN;

  gRandom->SetSeed();

  std::vector<Particle> eventParticles;
  double phi, theta, pulse;
  double px, py, pz;
  TH1D particleTypesHisto(                                              //
      "particle-types-distribution",                                    //
      "Particle types distribution",                                    //
      10, 0, 10),                                                       //
      zenithDist(                                                       //
          "zenith-distribution",                                        //
          "Zenith distribution",                                        //
          180, 0., M_PI),                                               //
      azimuthDist(                                                      //
          "azimuth-distribution",                                       //
          "Azimuth distribution",                                       //
          360, 0., PI2),                                                //
      pulseDist(                                                        //
          "pulse-distribution",                                         //
          "Pulse distribution",                                         //
          100, 0, 11),                                                  //
      traversePulseDist(                                                //
          "traverse-pulse-distribution",                                //
          "Traverse pulse distribution",                                //
          100, 0., 10.),                                                //
      particleEnergyDist(                                               //
          "particle-energy-distribution",                               //
          "Particle energy distribution",                               //
          100, 0., 10.),                                                //
      invMassDist(                                                      //
          "invariant-mass-distribution",                                //
          "Invariant mass distribution",                                //
          50, 0, 10),                                                   //
      invMassDiffChargeDist(                                            //
          "invariant-mass-discordant-charge-distribution",              //
          "Invariant mass discrodant charge distribution",              //
          50, 0, 10),                                                   //
      invMassSameChargeDist(                                            //
          "invariant-mass-concordant-charge-distribution",              //
          "Invariant mass concordant charge distribution",              //
          50, 0, 10),                                                   //
      invMassPioneKaoneDiscordantDist(                                  //
          "invariant-mass-discordant-charge-pione-kaone-distribution",  //
          "Invariant mass discordant charge pione kaone distribution",  //
          50, 0, 10),                                                   //
      invMassPioneKaoneConcordantDist(                                  //
          "invariant-mass-concordant-charge-pione-kaone-distribution",  //
          "Invariant mass concordant charge pione kaone distribution",  //
          50, 0, 10),                                                   //
      invMassSibDecayDist(                                              //
          "invariant-mass-siblings-distribution",                       //
          "Invariant mass siblings distribution",                       //
          100, 0, 7);

  // init histos' weights
  invMassDist.Sumw2();
  invMassDiffChargeDist.Sumw2();
  invMassSameChargeDist.Sumw2();
  invMassPioneKaoneDiscordantDist.Sumw2();
  invMassPioneKaoneConcordantDist.Sumw2();
  invMassSibDecayDist.Sumw2();

  section("Simulation");
  for (int i = 0; i < N_EVENTS; i++) {
    (i % 10000 == 0) && std::cout << (i / N_EVENTS * 100) << "% completed\n";
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
        auto& a = eventParticles[eventParticles.size() - 1];
        auto& b = eventParticles[eventParticles.size() - 2];
        particle.Decay2body(a, b);
        invMassSibDecayDist.Fill(a.InvMass(b));
      } else {
        eventParticles.push_back(particle);
      }
    }

    // fill histos
    const int n = eventParticles.size();
    for (int i = 0; i < n - 1; i++) {
      const auto& a = eventParticles[i];
      for (int j = i + 1; j < n; j++) {
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
        const int aType = a.GetParticleType(), bType = b.GetParticleType();
        if ((aType == pioneP && bType == kaoneN) ||
            (aType == pioneN && bType == kaoneP)) {
          invMassPioneKaoneDiscordantDist.Fill(invMass);
        } else if ((aType == pioneP && bType == kaoneP) ||
                   (aType == pioneN && bType == kaoneN)) {
          invMassPioneKaoneConcordantDist.Fill(invMass);
        }
      }
    }

    eventParticles.clear();
  }

  // save histos to file
  section("Saving to file");
  TFile saveFile("histos.root", "RECREATE");
  if (!saveFile.IsOpen()) {
    std::cout << "Unable to open histos.root file\n";
    return EXIT_FAILURE;
  }
  saveFile.Save();
  particleTypesHisto.Write();
  zenithDist.Write();
  azimuthDist.Write();
  pulseDist.Write();
  traversePulseDist.Write();
  particleEnergyDist.Write();
  invMassDist.Write();
  invMassDiffChargeDist.Write();
  invMassSameChargeDist.Write();
  invMassPioneKaoneDiscordantDist.Write();
  invMassPioneKaoneConcordantDist.Write();
  invMassSibDecayDist.Write();
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