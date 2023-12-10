#include <TF1.h>
#include <TFile.h>
#include <TH1D.h>

#include <iostream>
#include <stdexcept>

#include "constants.hpp"

TH1D *particleTypesHisto, *zenithDist, *azimuthDist, *pulseDist,
    *traversePulseDist;
TH1D *particleEnergyDist, *invMassDist, *invMassDiffChargeDist;
TH1D *invMassSameChargeDist, *invMassPioneKaoneDiscordantDist;
TH1D *invMassPioneKaoneConcordantDist, *invMassSibDecayDist;

inline const char* boolToString(bool b);
inline bool equals(double a, double b, double epsilon);
bool checkFit(TH1D* dist, const char* fitFunc, double xMin, double xMax,
              double chiSquareTollerance);
void loadHistos(TFile& file);
void checkHistosEntries();
void checkParticleTypesDistribution();

enum ParticleIndex : int {
  INVALID_PARTICLE,
  PIONE_P,
  PIONE_N,
  KAONE_P,
  KAONE_N,
  PROTONE_P,
  PROTONE_N,
  K_STAR
};

int main() {
  TFile file("histos.root");
  loadHistos(file);
  checkHistosEntries();
  checkParticleTypesDistribution();
  //   checkFit(zenithDist, "[0] * x + [1]", 0, M_PI, 0.1);
  //   checkFit(azimuthDist, "[0] * x + [1]", 0, M_PI * 2, 0.1);
  file.Close();
}

void loadHistos(TFile& file) {
  if (!file.IsOpen()) {
    throw std::runtime_error("Unable to open histos.root");
  }
  std::cout << "Loading histograms\n";
  particleTypesHisto = (TH1D*)file.Get(  //
      "particle-types-distribution");
  zenithDist = (TH1D*)file.Get(  //
      "zenith-distribution");
  azimuthDist = (TH1D*)file.Get(  //
      "azimuth-distribution");
  pulseDist = (TH1D*)file.Get(  //
      "pulse-distribution");
  traversePulseDist = (TH1D*)file.Get(  //
      "traverse-pulse-distribution");
  particleEnergyDist = (TH1D*)file.Get(  //
      "particle-energy-distribution");
  invMassDist = (TH1D*)file.Get(  //
      "invariant-mass-distribution");
  invMassDiffChargeDist = (TH1D*)file.Get(  //
      "invariant-mass-discordant-charge-distribution");
  invMassSameChargeDist = (TH1D*)file.Get(  //
      "invariant-mass-concordant-charge-distribution");
  invMassPioneKaoneDiscordantDist = (TH1D*)file.Get(  //
      "invariant-mass-discordant-charge-pione-kaone-distribution");
  invMassPioneKaoneConcordantDist = (TH1D*)file.Get(  //
      "invariant-mass-concordant-charge-pione-kaone-distribution");
  invMassSibDecayDist = (TH1D*)file.Get(  //
      "invariant-mass-siblings-distribution");
}

void checkHistosEntries() {
  double invMassEntries = 0.0;
  for (int i = 0; i <= N_PARTICLES; i++) {
    invMassEntries += N_PARTICLES - i;
  }
  invMassEntries *= N_EVENTS;
  std::cout << "Checking histograms entries\n";
  std::cout << "HISTO\t\tEXPECTED ENTRIES\t\tACTUAL ENTRIES\n";
  std::cout << "particle-types\t\t" << (N_EVENTS * N_PARTICLES) << "\t\t"
            << particleTypesHisto->GetEntries() << "\n";
  std::cout << "zenith\t\t" << (N_EVENTS * N_PARTICLES) << "\t\t"
            << zenithDist->GetEntries() << "\n";
  std::cout << "azimuth\t\t" << (N_EVENTS * N_PARTICLES) << "\t\t"
            << azimuthDist->GetEntries() << "\n";
  std::cout << "pulse\t\t" << (N_EVENTS * N_PARTICLES) << "\t\t"
            << pulseDist->GetEntries() << "\n";
  std::cout << "traverse-pulse\t\t" << (N_EVENTS * N_PARTICLES) << "\t\t"
            << traversePulseDist->GetEntries() << "\n";
  std::cout << "particle-energy\t\t" << (N_EVENTS * N_PARTICLES) << "\t\t"
            << particleEnergyDist->GetEntries() << "\n";
  std::cout << "invariant-mass\t\t" << invMassEntries << "\t\t"
            << invMassDist->GetEntries() << "\n";

  //    invMassDiffChargeDist; invMassSameChargeDist,
  //    invMassPioneKaoneDiscordantDist; invMassPioneKaoneConcordantDist,
  //    invMassSibDecayDist;
}

void checkParticleTypesDistribution() {
  std::cout << "Checking particle types distributions\n";
  const auto computeBinPercentage = [](int binIndex, TH1D* dist) {
    return dist->GetBinContent(binIndex) / dist->GetEntries() * 100;
  };
  std::cout << "Expected PIONE+ probability: 40%\tFound: "
            << computeBinPercentage(PIONE_P, particleTypesHisto) << "%\n";
  std::cout << "Expected PIONE- probability: 40%\tFound: "
            << computeBinPercentage(PIONE_N, particleTypesHisto) << "%\n";
  std::cout << "Expected KAONE+ probability: 5% \tFound: "
            << computeBinPercentage(KAONE_P, particleTypesHisto) << "%\n";
  std::cout << "Expected KAONE- probability: 5% \tFound: "
            << computeBinPercentage(KAONE_N, particleTypesHisto) << "%\n";
  std::cout << "Expected PROTONE+ probability: 4.5%\tFound: "
            << computeBinPercentage(PROTONE_P, particleTypesHisto) << "%\n";
  std::cout << "Expected PROTONE- probability: 4.5%\tFound: "
            << computeBinPercentage(PROTONE_N, particleTypesHisto) << "%\n";
  std::cout << "Expected K* probability: 1% \t\tFound: "
            << computeBinPercentage(K_STAR, particleTypesHisto) << "%\n";
}

bool checkFit(TH1D* dist, const char* fitFunc, double xMin, double xMax,
              double chiSquareTollerance) {
  TF1 linear("fit-func", fitFunc, xMin, xMax);
  dist->Fit("fit-func", "Q");
  std::cout << "chi reduced: " << (linear.GetChisquare() / linear.GetNDF())
            << "\n";
  return (linear.GetChisquare() / linear.GetNDF()) < chiSquareTollerance;
}

inline bool equals(double a, double b, double epsilon) {
  return abs(a - b) < epsilon;
}

inline const char* boolToString(bool b) {
  return b ? "true" : "false";
}
