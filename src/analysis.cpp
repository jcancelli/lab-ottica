#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TH1D.h>

#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "constants.hpp"
#include "table.hpp"
#include "util.hpp"

std::unordered_map<const char*, TH1D*> histos{
    {"particle-types", nullptr},
    {"zenith", nullptr},
    {"azimuth", nullptr},
    {"pulse", nullptr},
    {"traverse-pulse", nullptr},
    {"particle-energy", nullptr},
    {"inv-mass", nullptr},
    {"inv-mass-discordant", nullptr},
    {"inv-mass-concordant", nullptr},
    {"inv-mass-discordant-pk", nullptr},
    {"inv-mass-concordant-pk", nullptr},
    {"inv-mass-siblings", nullptr}};

TF1 fit(TH1D* dist, const char* fitFunc, double xMin, double xMax);
void loadHistos(TFile& file);
void checkHistosEntries();
void checkParticleTypesDistribution();
void extractKStar();
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
  TFile file(SAVE_FILE);
  TCanvas canvas("canvas", "", 400, 400);
  loadHistos(file);
  checkHistosEntries();
  checkParticleTypesDistribution();
  section("Zenith fit");
  fit(histos["zenith"], "pol0", 0, M_PI);
  section("Azimuth fit");
  fit(histos["azimuth"], "pol0", 0, M_PI * 2);
  section("Pulse fit");
  fit(histos["pulse"], "expo", 0, 7);
  extractKStar();
  saveToPdf();
  file.Close();
}

void loadHistos(TFile& file) {
  section("Loading histograms");
  if (!file.IsOpen()) {
    throw std::runtime_error("Unable to open histograms file");
  }
  for (auto& histo : histos) {
    std::cout << "Loading " << histo.first << "\n";
    histo.second = (TH1D*)file.Get(histo.first);
  }
}

void checkHistosEntries() {
  const int expectedParticlesTotal = N_EVENTS * N_PARTICLES;

  double invMassEntries = 0.0;
  for (int i = 0; i <= N_PARTICLES; i++) {
    invMassEntries += N_PARTICLES - i;
  }
  invMassEntries *= N_EVENTS;

  const int expectedKP =
      (N_PARTICLES * N_PARTICLES / 2) * (0.8 + 0.01) * (0.1 + 0.01) * N_EVENTS;

  section("Histograms entries");
  Table<const char*, int, int>()
      .headers({"HISTOGRAM", "EXPECTED", "ACTUAL"})
      .row("particle-types",        //
           expectedParticlesTotal,  //
           histos["particle-types"]->GetEntries())
      .row("zenith",                //
           expectedParticlesTotal,  //
           histos["zenith"]->GetEntries())
      .row("azimuth",               //
           expectedParticlesTotal,  //
           histos["azimuth"]->GetEntries())
      .row("pulse",                 //
           expectedParticlesTotal,  //
           histos["pulse"]->GetEntries())
      .row("traverse-pulse",        //
           expectedParticlesTotal,  //
           histos["traverse-pulse"]->GetEntries())
      .row("particle-energy",       //
           expectedParticlesTotal,  //
           histos["particle-energy"]->GetEntries())
      .row("invariant-mass",  //
           invMassEntries,    //
           histos["inv-mass"]->GetEntries())
      .row("invariant-mass-discordant-charge",  //
           invMassEntries / 2,                  //
           histos["inv-mass-discordant"]->GetEntries())
      .row("invariant-mass-concordant-charge",  //
           invMassEntries / 2,                  //
           histos["inv-mass-concordant"]->GetEntries())
      .row("invariant-mass-pione-kaone-discordant-charge",  //
           expectedKP,                                      //
           histos["inv-mass-discordant-pk"]->GetEntries())
      .row("invariant-mass-pione-kaone-concordant-charge",  //
           expectedKP,                                      //
           histos["inv-mass-concordant-pk"]->GetEntries())
      .row("invariant-mass-decay-siblings",  //
           expectedParticlesTotal * 0.01,    //
           histos["inv-mass-siblings"]->GetEntries())
      .spacing(7)
      .print();
}

void checkParticleTypesDistribution() {
  section("Particle types distributions");
  const auto computeBinPercentage = [](int binIndex, TH1D* dist) {
    return dist->GetBinContent(binIndex) / dist->GetEntries() * 100;
  };
  auto histo = histos["particle-types"];
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

TF1 fit(TH1D* dist, const char* fitFormula, double xMin, double xMax) {
  auto fitFuncName = concat(dist->GetName(), "-fit");
  TF1 fitFunc(fitFuncName.c_str(), fitFormula, xMin, xMax);
  dist->Fit(fitFuncName.c_str(), "Q");
  std::cout << "Function\t\t" << fitFormula << "\n";
  std::cout << "Parameters:\n";
  for (int i = 0; i < fitFunc.GetNpar(); i++) {
    std::cout << "\t" << fitFunc.GetParName(i) << ": "
              << fitFunc.GetParameter(i) << "\n";
  }
  std::cout << "Reduced chi squared\t"
            << (fitFunc.GetChisquare() / fitFunc.GetNDF()) << "\n";
  std::cout << "Fit probability\t\t" << fitFunc.GetProb() << "\n";
  return fitFunc;
}

void extractKStar() {
  section("Extract k*");
  TH1D* diffDiscConc = (TH1D*)histos["inv-mass-discordant"]->Clone(
      "diff-inv-mass-discordant-concordant");
  diffDiscConc->Add(histos["inv-mass-concordant"], -1);
  TH1D* diffPKDiscConc = (TH1D*)histos["inv-mass-discordant-pk"]->Clone(
      "diff-inv-mass-pk-discordant-concordant");
  diffPKDiscConc->Add(histos["inv-mass-concordant-pk"], -1);
  std::cout << "\n-- Fit difference inv. mass discordant concordant\n";
  TF1 fitDiscConc = fit(diffDiscConc, "gaus", 0, 10);
  std::cout
      << "\n-- Fit difference inv. mass discordant concordant pione-kaone "
         "pairs\n";
  TF1 fitPKDiscConc = fit(diffPKDiscConc, "gaus", 0, 10);
  auto avgMass =
      (fitDiscConc.GetParameter("Mean") + fitPKDiscConc.GetParameter("Mean")) /
      2.;
  auto avgWidth = (fitDiscConc.GetParameter("Sigma") +
                   fitPKDiscConc.GetParameter("Sigma")) /
                  2.;
  std::cout << "\n-- Average K* values\n";
  Table<const char*, double, double>()
      .headers({"", "EXPECTED", "FOUND"})
      .row("Mass", 0.89166, avgMass)
      .row("Width", 0.05, avgWidth)
      .spacing(7)
      .print();
}

void saveToPdf() {
  section("Saving histograms to PDF");

  if (!std::filesystem::exists("histos")) {
    std::filesystem::create_directory("histos");
  }

  TCanvas canvas("pdf-canvas", "", 700, 700);
  for (auto& histo : histos) {
    histo.second->Draw("HIST");
    canvas.SaveAs(concat("histos/", histo.first, ".pdf").c_str(), "Q");
  }
}
