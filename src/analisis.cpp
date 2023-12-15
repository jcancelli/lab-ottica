#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TH1D.h>

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "constants.hpp"
#include "table.hpp"
#include "util.hpp"

std::unordered_map<const char*, TH1D*> histos{
    {"particle-types-distribution", nullptr},
    {"zenith-distribution", nullptr},
    {"azimuth-distribution", nullptr},
    {"pulse-distribution", nullptr},
    {"traverse-pulse-distribution", nullptr},
    {"particle-energy-distribution", nullptr},
    {"invariant-mass-distribution", nullptr},
    {"invariant-mass-discordant-charge-distribution", nullptr},
    {"invariant-mass-concordant-charge-distribution", nullptr},
    {"invariant-mass-discordant-charge-pione-kaone-distribution", nullptr},
    {"invariant-mass-concordant-charge-pione-kaone-distribution", nullptr},
    {"invariant-mass-siblings-distribution", nullptr}};

void fit(TH1D* dist, const char* fitFunc, double xMin, double xMax);
void loadHistos(TFile& file);
void checkHistosEntries();
void checkParticleTypesDistribution();
void saveToPdf();

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
  TCanvas canvas("canvas", "", 400, 400);
  loadHistos(file);
  checkHistosEntries();
  checkParticleTypesDistribution();
  section("Zenith fit");
  fit(histos["zenith-distribution"], "pol0", 0, M_PI);
  section("Azimuth fit");
  fit(histos["azimuth-distribution"], "pol0", 0, M_PI * 2);
  section("Pulse fit");
  fit(histos["pulse-distribution"], "expo", 0, 7);
  saveToPdf();
  file.Close();
}

void loadHistos(TFile& file) {
  if (!file.IsOpen()) {
    throw std::runtime_error("Unable to open histos.root");
  }
  section("Loading histograms");
  for (auto& histo : histos) {
    std::cout << "Loading " << histo.first << "\n";
    histo.second = (TH1D*)file.Get(histo.first);
  }
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
           histos["particle-types-distribution"]->GetEntries())
      .row("zenith",                //
           N_EVENTS * N_PARTICLES,  //
           histos["zenith-distribution"]->GetEntries())
      .row("azimuth",               //
           N_EVENTS * N_PARTICLES,  //
           histos["azimuth-distribution"]->GetEntries())
      .row("pulse",                 //
           N_EVENTS * N_PARTICLES,  //
           histos["pulse-distribution"]->GetEntries())
      .row("traverse-pulse",        //
           N_EVENTS * N_PARTICLES,  //
           histos["traverse-pulse-distribution"]->GetEntries())
      .row("particle-energy",       //
           N_EVENTS * N_PARTICLES,  //
           histos["particle-energy-distribution"]->GetEntries())
      .row("invariant-mass",  //
           invMassEntries,    //
           histos["invariant-mass-distribution"]->GetEntries())
      .row(
          "invariant-mass-discordant-charge",  //
          invMassEntries / 2,                  //
          histos["invariant-mass-discordant-charge-distribution"]->GetEntries())
      .row(
          "invariant-mass-concordant-charge",  //
          invMassEntries / 2,                  //
          histos["invariant-mass-concordant-charge-distribution"]->GetEntries())
      .row("invariant-mass-pione-kaone-discordant-charge",  //
           0,                                               //
           histos["invariant-mass-discordant-charge-pione-kaone-distribution"]
               ->GetEntries())
      .row("invariant-mass-pione-kaone-concordant-charge",  //
           0,                                               //
           histos["invariant-mass-concordant-charge-pione-kaone-distribution"]
               ->GetEntries())
      .row("invariant-mass-decay-siblings",  //
           0,                                //
           histos["invariant-mass-siblings-distribution"]->GetEntries())
      .spacing(7)
      .print();
}

void checkParticleTypesDistribution() {
  section("Particle types distributions");
  const auto computeBinPercentage = [](int binIndex, TH1D* dist) {
    return dist->GetBinContent(binIndex) / dist->GetEntries() * 100;
  };
  auto histo = histos["particle-types-distribution"];
  Table<const char*, double, double>()
      .headers({"PARTICLE", "EXPECTED (%)", "ACTUAL (%)"})
      .row("PIONE+", 40, computeBinPercentage(PIONE_P, histo))
      .row("PIONE-", 40, computeBinPercentage(PIONE_N, histo))
      .row("KAONE+", 5, computeBinPercentage(KAONE_P, histo))
      .row("KAONE-", 5, computeBinPercentage(KAONE_N, histo))
      .row("PROTONE+", 4.5, computeBinPercentage(PROTONE_P, histo))
      .row("PROTONE-", 4.5, computeBinPercentage(PROTONE_N, histo))
      .row("K*", 1, computeBinPercentage(K_STAR, histo))
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

void saveToPdf() {
  section("Saving histograms to PDF");

  if (!std::filesystem::exists("histos")) {
    std::filesystem::create_directory("histos");
  }

  for (auto& histo : histos) {
    TCanvas canvas("pdf-canvas", "", 700, 700);
    histo.second->Draw("HIST");
    canvas.SaveAs(concat("histos/", histo.first, ".pdf").c_str(), "Q");
  }
}
