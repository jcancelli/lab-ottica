#include <TFile.h>
#include <TH1D.h>
#include <TRandom.h>
#include <TStopwatch.h>

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
  TStopwatch timer;

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
  Double_t edgesParticleTypesHisto[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  TH1D particleTypesHisto(                                                   //
      "particle-types",                                                      //
      "Particle types;Type;Entries",                                         //
      7, edgesParticleTypesHisto),                                           //
      zenithDist(                                                            //
          "zenith",                                                          //
          "Zenith;Radians;Entries",                                          //
          1000, 0., M_PI),                                                   //
      azimuthDist(                                                           //
          "azimuth",                                                         //
          "Azimuth;Radians;Entries",                                         //
          1000, 0., PI2),                                                    //
      pulseDist(                                                             //
          "pulse",                                                           //
          "Pulse;Pulse;Entries",                                             //
          1000, 0, 11),                                                      //
      traversePulseDist(                                                     //
          "traverse-pulse",                                                  //
          "Traverse pulse;Traverse pulse;Entries",                           //
          1000, 0., 10.),                                                    //
      particleEnergyDist(                                                    //
          "particle-energy",                                                 //
          "Particle energy;Total energy;Entries",                            //
          1000, 0., 10.),                                                    //
      invMassDist(                                                           //
          "inv-mass",                                                        //
          "Inv. mass;Invariant mass;Entries",                                //
          1000, 0, 10),                                                      //
      invMassDiffChargeDist(                                                 //
          "inv-mass-discordant",                                             //
          "Inv. mass discrodant charge;Invariant mass;Entries",              //
          1000, 0, 10),                                                      //
      invMassSameChargeDist(                                                 //
          "inv-mass-concordant",                                             //
          "Inv. mass concordant charge;Invariant mass;Entries",              //
          1000, 0, 10),                                                      //
      invMassPioneKaoneDiscordantDist(                                       //
          "inv-mass-discordant-pk",                                          //
          "Inv. mass pione kaone discordant charge;Invariant mass;Entries",  //
          1000, 0, 10),                                                      //
      invMassPioneKaoneConcordantDist(                                       //
          "inv-mass-concordant-pk",                                          //
          "Inv. mass pione kaone concordant charge;Invariant mass;Entries",  //
          1000, 0, 10),                                                      //
      invMassSibDecayDist(                                                   //
          "inv-mass-siblings",                                               //
          "Inv. mass siblings;Invariant mass;Entries",                       //
          1000, 0, 2);
  auto* typesXAxis = particleTypesHisto.GetXaxis();
  typesXAxis->SetBinLabel(1, "pione+");
  typesXAxis->SetBinLabel(2, "pione-");
  typesXAxis->SetBinLabel(3, "kaone+");
  typesXAxis->SetBinLabel(4, "kaone-");
  typesXAxis->SetBinLabel(5, "protone+");
  typesXAxis->SetBinLabel(6, "protone-");
  typesXAxis->SetBinLabel(7, "K*");

  // init histos' weights
  invMassDist.Sumw2();
  invMassDiffChargeDist.Sumw2();
  invMassSameChargeDist.Sumw2();
  invMassPioneKaoneDiscordantDist.Sumw2();
  invMassPioneKaoneConcordantDist.Sumw2();
  invMassSibDecayDist.Sumw2();

  section("Simulation");
  timer.Start();
  double completion = 0.0;
  for (int i = 1; i <= N_EVENTS; i++) {
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
            (aType == pioneN && bType == kaoneP) ||
            (aType == kaoneP && bType == pioneN) ||
            (aType == kaoneN && bType == pioneP)) {
          invMassPioneKaoneDiscordantDist.Fill(invMass);
        } else if ((aType == pioneP && bType == kaoneP) ||
                   (aType == pioneN && bType == kaoneN) ||
                   (aType == kaoneP && bType == pioneP) ||
                   (aType == kaoneN && bType == pioneN)) {
          invMassPioneKaoneConcordantDist.Fill(invMass);
        }
      }
    }

    eventParticles.clear();

    completion = i / N_EVENTS * 100.;
    printf("\r%.0f%% completed in %.3fs", completion, timer.RealTime());
    timer.Continue();
  }
  std::cout << "\n";

  // save histos to file
  section("Saving to file");
  TFile saveFile(SAVE_FILE, "RECREATE");
  if (!saveFile.IsOpen()) {
    std::cout << "Unable to open " << SAVE_FILE << " file\n";
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
  std::cout << "Saved to " << SAVE_FILE << "\n";
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