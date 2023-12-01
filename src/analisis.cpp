#include <TF1.h>
#include <TFile.h>
#include <TH1D.h>

#include <iostream>
#include <stdexcept>

inline const char* boolToString(bool b);
inline bool equals(double a, double b, double epsilon);
bool checkFit(TH1D* dist, const char* fitFunc, double xMin, double xMax);
void loadHistos();
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

TH1D *particleTypesHisto, *zenithDist, *azimuthDist, *pulseDist,
    *traversePulseDist;
TH1D *particleEnergyDist, *invMassDist, *invMassDiffChargeDist;
TH1D *invMassSameChargeDist, *invMassPioneKaoneDiscordantDist;
TH1D *invMassPioneKaoneConcordantDist, *invMassSibDecayDist;

int main() {
  loadHistos();
  checkHistosEntries();
  checkParticleTypesDistribution();
  checkFit(zenithDist, "[0] * x + [1]", 0, M_PI);
  checkFit(azimuthDist, "[0] * x + [1]", 0, M_PI * 2);
}

void loadHistos() {
  TFile file("histos.root");
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
  file.Close();
}

void checkHistosEntries() {
  std::cout << "Checking histograms entries\n";
  // TODO: Implement
}

void checkParticleTypesDistribution() {
  // TODO: Rewrite whole function using getError e getbin
  std::cout << "Checking particle types distributions\n";
  TH1D* dist = particleTypesHisto;
  const double epsilon = 0.00001;
  const int totalEntries =
      dist->GetEntries();  // FIXME: Causes segmentation violation
  const auto isProbabilityCompatible = [dist, totalEntries](
                                           ParticleIndex particle,
                                           double probability, double epsilon) {
    return equals(
        1, dist->GetBinContent((int)particle) / totalEntries * probability,
        epsilon);
  };

  std::cout << "Pione+ probability is 40%:\t"
            << boolToString(isProbabilityCompatible(PIONE_P, 0.4, epsilon))
            << "\n";
  std::cout << "Pione- probability is 40%:\t"
            << boolToString(isProbabilityCompatible(PIONE_N, 0.4, epsilon))
            << "\n";
  std::cout << "Kaone+ probability is 5%:\t"
            << boolToString(isProbabilityCompatible(KAONE_P, 0.05, epsilon))
            << "\n";
  std::cout << "Kaone- probability is 5%:\t"
            << boolToString(isProbabilityCompatible(KAONE_N, 0.05, epsilon))
            << "\n";
  std::cout << "Protone+ probability is 4.5%:\t"
            << boolToString(isProbabilityCompatible(PROTONE_P, 0.045, epsilon))
            << "\n";
  std::cout << "Protone- probability is 4.5%:\t"
            << boolToString(isProbabilityCompatible(PIONE_P, 0.045, epsilon))
            << "\n";
  std::cout << "K* probability is 1%:\t\t"
            << boolToString(isProbabilityCompatible(K_STAR, 0.01, epsilon))
            << "\n";
}

bool checkFit(TH1D* dist, const char* fitFunc, double xMin, double xMax) {
  TF1 linear("fit-func", fitFunc, xMin, xMax);
  dist->Fit("fit-func", "Q");
  return equals(1, linear.GetChisquare() / linear.GetNDF(), 0.1);
}

inline bool equals(double a, double b, double epsilon) {
  return abs(a - b) < epsilon;
}

inline const char* boolToString(bool b) {
  return b ? "true" : "false";
}
