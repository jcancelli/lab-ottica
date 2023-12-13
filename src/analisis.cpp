#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TH1D.h>

#include <iostream>
#include <stdexcept>
#include <string>

#include "constants.hpp"
#include "table.hpp"

TH1D *particleTypesHisto, *zenithDist, *azimuthDist, *pulseDist,
    *traversePulseDist;
TH1D *particleEnergyDist, *invMassDist, *invMassDiffChargeDist;
TH1D *invMassSameChargeDist, *invMassPioneKaoneDiscordantDist;
TH1D *invMassPioneKaoneConcordantDist, *invMassSibDecayDist;

inline void section(const char* title);
inline const char* boolToString(bool b);
inline bool equals(double a, double b, double epsilon);
void fit(TH1D* dist, const char* fitFunc, double xMin, double xMax);
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
  TCanvas canvas("canvas", "Ciaccia", 400, 400);
  loadHistos(file);
  checkHistosEntries();
  checkParticleTypesDistribution();
  section("Zenith fit");
  fit(zenithDist, "pol0", 0, M_PI);
  section("Azimuth fit");
  fit(azimuthDist, "pol0", 0, M_PI * 2);
  section("Pulse fit");
  fit(pulseDist, "expo", 0, 7);
  file.Close();
}

void loadHistos(TFile& file) {
  if (!file.IsOpen()) {
    throw std::runtime_error("Unable to open histos.root");
  }
  section("Loading histograms");
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

  section("Histograms entries");
  Table<const char*, int, int>()
      .headers({"HISTOGRAM", "EXPECTED", "ACTUAL"})
      .row("particle-types",        //
           N_EVENTS * N_PARTICLES,  //
           particleTypesHisto->GetEntries())
      .row("zenith",                //
           N_EVENTS * N_PARTICLES,  //
           zenithDist->GetEntries())
      .row("azimuth",               //
           N_EVENTS * N_PARTICLES,  //
           azimuthDist->GetEntries())
      .row("pulse",                 //
           N_EVENTS * N_PARTICLES,  //
           pulseDist->GetEntries())
      .row("traverse-pulse",        //
           N_EVENTS * N_PARTICLES,  //
           traversePulseDist->GetEntries())
      .row("particle-energy",       //
           N_EVENTS * N_PARTICLES,  //
           particleEnergyDist->GetEntries())
      .row("invariant-mass",  //
           invMassEntries,    //
           invMassDist->GetEntries())
      .row("invariant-mass-discordant-charge",  //
           invMassEntries / 2,                  //
           invMassDiffChargeDist->GetEntries())
      .row("invariant-mass-concordant-charge",  //
           invMassEntries / 2,                  //
           invMassSameChargeDist->GetEntries())
      .row("invariant-mass-pione-kaone-discordant-charge",  //
           0,                                               //
           invMassPioneKaoneDiscordantDist->GetEntries())
      .row("invariant-mass-pione-kaone-concordant-charge",  //
           0,                                               //
           invMassPioneKaoneConcordantDist->GetEntries())
      .row("invariant-mass-decay siblings",  //
           0,                                //
           invMassSibDecayDist->GetEntries())
      .spacing(7)
      .print();
}

void checkParticleTypesDistribution() {
  section("Particle types distributions");
  const auto computeBinPercentage = [](int binIndex, TH1D* dist) {
    return dist->GetBinContent(binIndex) / dist->GetEntries() * 100;
  };
  Table<const char*, double, double>()
      .headers({"PARTICLE", "EXPECTED (%)", "ACTUAL (%)"})
      .row("PIONE+", 40, computeBinPercentage(PIONE_P, particleTypesHisto))
      .row("PIONE-", 40, computeBinPercentage(PIONE_N, particleTypesHisto))
      .row("KAONE+", 5, computeBinPercentage(KAONE_P, particleTypesHisto))
      .row("KAONE-", 5, computeBinPercentage(KAONE_N, particleTypesHisto))
      .row("PROTONE+", 4.5, computeBinPercentage(PROTONE_P, particleTypesHisto))
      .row("PROTONE-", 4.5, computeBinPercentage(PROTONE_N, particleTypesHisto))
      .row("K*", 1, computeBinPercentage(K_STAR, particleTypesHisto))
      .spacing(7)
      .print();
}

void fit(TH1D* dist, const char* fitFormula, double xMin, double xMax) {
  TF1 fitFunc("fit-func", fitFormula, xMin, xMax);
  dist->Fit("fit-func", "Q");
  std::cout << "Function\t\t" << fitFormula << "\n";
  std::cout << "Parameters:\n";
  for (int i = 0; i < fitFunc.GetNpar(); i++) {
    std::cout << "\t" << fitFunc.GetParName(i) << ": "
              << fitFunc.GetParameter(i) << "\n";
  }
  std::cout << "Reduced chi squared\t"
            << (fitFunc.GetChisquare() / fitFunc.GetNDF()) << "\n";
  std::cout << "Fit probability\t\t" << fitFunc.GetProb() << "\n";
}

inline bool equals(double a, double b, double epsilon) {
  return abs(a - b) < epsilon;
}

inline void section(const char* title) {
  std::cout << "\n----- " << title << " -----\n";
}