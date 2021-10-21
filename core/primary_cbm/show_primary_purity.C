/*
 * show_primary_purity.C
 *
 *  Created on: 4 maj 2021
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#ifndef __CLING__
#define __DUMMY__
#include "THStack.h"

#include "../../includes/exp_includes.h"
#include "../../includes/nicafemto_includes.h"
#endif
#define SUPRESS_STAT_ERRORS


TH1D** projPrimaryXY;
TH1D** projPrimaryZ;
TH1D** projSecondaryXY;
TH1D** projSecondaryZ;
TH1D*** projRecoXYPid;
TH1D*** projRecoZPid;


void DrawPrimaryEfficiency();
void DrawPrimarySecondaryEfficiency();
void DrawPIDPurity();
TH1D* IntegrateHistogram(TH1D* h, Bool_t scale = kFALSE);
TH1D* NormalizeTo100(TH1D* h, Bool_t scale = kFALSE);

void show_primary_purity(TString inFile = "data.root")
{
  NicaAnaFile* file = new NicaAnaFile(inFile);

  TH2D** primaryMc   = new TH2D*[6];
  TH2D** secondaryMc = new TH2D*[6];
  TH3D** reco        = new TH3D*[6];
  for (int i = 0; i < 6; i++) {
    primaryMc[i] = (TH2D*) file->GetHistogramPassed(ENicaCutUpdate::kTrackUpdate, i, 0);
  }
  file->SwitchPackage(1);
  for (int i = 0; i < 6; i++) {
    secondaryMc[i] = (TH2D*) file->GetHistogramPassed(ENicaCutUpdate::kTrackUpdate, i, 0);
  }


  file->SwitchPackage(2);
  for (int i = 0; i < 6; i++) {
    reco[i] = (TH3D*) file->GetHistogramPassed(ENicaCutUpdate::kTrackUpdate, i, 0);
  }
  projPrimaryXY   = new TH1D*[6];
  projPrimaryZ    = new TH1D*[6];
  projSecondaryXY = new TH1D*[6];
  projSecondaryZ  = new TH1D*[6];

  for (int i = 0; i < 6; i++) {
    projPrimaryXY[i]   = NicaStd::GetProjection1D(primaryMc[i], -1, -1, "x+bins+randname");
    projPrimaryZ[i]    = NicaStd::GetProjection1D(primaryMc[i], -1, -1, "y+bins+randname");
    projSecondaryXY[i] = NicaStd::GetProjection1D(secondaryMc[i], -1, -1, "x+bins+randname");
    projSecondaryZ[i]  = NicaStd::GetProjection1D(secondaryMc[i], -1, -1, "y+bins+randname");
  }
  projRecoXYPid = new TH1D**[6];
  projRecoZPid  = new TH1D**[6];
  for (int i = 0; i < 6; i++) {
    projRecoXYPid[i] = new TH1D*[15];
    projRecoZPid[i]  = new TH1D*[15];
    for (int j = 1; j <= 14; j++) {
      projRecoXYPid[i][j] = NicaStd::GetProjection1D(reco[i], -1, -1, j, j, "x+bins+randname");
      projRecoZPid[i][j]  = NicaStd::GetProjection1D(reco[i], -1, -1, j, j, "y+bins+randname");
    }
  }

  gStyle->SetOptStat(0);

  DrawPrimaryEfficiency();
  DrawPrimarySecondaryEfficiency();
  DrawPIDPurity();
}

TH1D* IntegrateHistogram(TH1D* h, Bool_t scale)
{
  TH1D* res = (TH1D*) h->Clone();
  res->Reset();
  Double_t sum = 0;
  for (int i = 1; i <= h->GetNbinsX(); i++) {
    sum += h->GetBinContent(i);
    res->SetBinContent(i, sum);
    Double_t error = TMath::Sqrt(sum);
#ifdef SUPRESS_STAT_ERRORS
    if (error > 0.25 * sum) { res->SetBinError(i, 0); }
    else {
      res->SetBinError(i, error);
    }
#else
    res->SetBinError(i, TMath::Sqrt(sum));
#endif
  }
  if (scale && sum != 0) { res->Scale(100.0 / sum); }
  return res;
}

TH1D* NormalizeTo100(TH1D* h, Bool_t scale)
{
  TH1D* res        = (TH1D*) h->Clone();
  Double_t entries = res->GetEntries();
  res->Scale(100.0 / entries);
  return res;
}

void DrawPrimaryEfficiency()
{


  auto draw_primary_effic = [](TH1D* h, TLegend* leg, Int_t i, Bool_t integrate, TString label) {
    TString names[6]  = {"#pi^{+}", "#pi^{-}", "K^{+}", "K^{-}", "p", "#bar{p}"};
    Int_t enumPids[6] = {3, 8, 4, 9, 5, 10};
    Color_t colz[6]   = {kRed, kRed + 2, kCyan, kCyan + 3, kBlue, kBlue + 2};
    Int_t marker[6]   = {kFullTriangleDown, kFullTriangleUp,   kFullTriangleDown,
                       kFullTriangleUp,   kFullTriangleDown, kFullTriangleUp};
    gPad->SetGridy();
    gPad->SetGridx();
    TH1D* integratedPrimary = nullptr;
    if (integrate) {
      integratedPrimary = IntegrateHistogram(h, kTRUE);
      TString xtit      = integratedPrimary->GetXaxis()->GetTitle();
      TString titCut    = "max cut on: ";
      integratedPrimary->SetXTitle(titCut + xtit);
    }
    else {
      integratedPrimary = NormalizeTo100(h, kTRUE);
      gPad->SetLogy();
    }
    integratedPrimary->SetTitle(label);
    integratedPrimary->SetLineColor(colz[i]);
    integratedPrimary->SetMarkerColor(colz[i]);
    integratedPrimary->SetMarkerStyle(marker[i]);
    integratedPrimary->SetYTitle("Efficiency [%]");
    integratedPrimary->Draw("SAME");
    if (leg != nullptr) leg->AddEntry(integratedPrimary, names[i]);
  };

  TCanvas* c1 = new TCanvas();
  c1->Divide(3, 2);
  TLegend* leg1 = new TLegend(0.1, 0.1, 0.95, 0.95);

  for (int i = 0; i < 6; i++) {
    c1->cd(1);
    draw_primary_effic(projPrimaryXY[i], leg1, i, kTRUE, "Total % of primary particles passed by DCA XY cut");
    c1->cd(2);
    draw_primary_effic(projPrimaryZ[i], nullptr, i, kTRUE, "Total % of primary particles passed by DCA Z cut");

    c1->cd(4);
    draw_primary_effic(projPrimaryXY[i], nullptr, i, kFALSE, "% of primary particles with DCA XY");
    c1->cd(5);
    draw_primary_effic(projPrimaryZ[i], nullptr, i, kFALSE, "% of primary particles with DCA Z");
  }
  c1->cd(3);
  leg1->Draw();
}

void DrawPrimarySecondaryEfficiency()
{
  auto display_primary_secondary = [](TH1D* prim, TH1D* sec, TLegend* leg, TString desc, Int_t i, Bool_t integrate) {
    gPad->SetGridy();
    gPad->SetGridx();
    TString names[6] = {"#pi^{+}", "#pi^{-}", "K^{+}", "K^{-}", "p", "#bar{p}"};
    Color_t colz[6]  = {kRed, kRed + 2, kCyan, kCyan + 2, kBlue, kBlue + 2};
    Int_t marker[6]  = {kFullTriangleDown, kFullTriangleDown, kFullTriangleDown,
                       kFullTriangleDown, kFullTriangleDown, kFullTriangleDown};
    TH1D* Primary    = nullptr;
    TH1D* Secondary  = nullptr;
    if (integrate) {
      Primary   = IntegrateHistogram(prim);
      Secondary = IntegrateHistogram(sec);
    }
    else {
      Primary   = (TH1D*) prim->Clone();
      Secondary = (TH1D*) sec->Clone();
    }
    Primary->Sumw2();
    Secondary->Sumw2();
    Secondary->Add(Primary);
    Primary->Divide(Secondary);
    Primary->SetTitle(desc);
    Primary->SetLineColor(colz[i]);
    Primary->SetMarkerColor(colz[i]);
    Primary->SetMarkerStyle(marker[i]);
    Primary->SetYTitle("Fraction [%]");
    Primary->Scale(100);
    Primary->SetMaximum(105);
    Primary->Draw("SAME");

    if (leg != nullptr) leg->AddEntry(Primary, names[i]);
  };

  TCanvas* c1 = new TCanvas();
  c1->Divide(3, 2);
  TLegend* leg1 = new TLegend(0.1, 0.1, 0.95, 0.95);

  for (int i = 0; i < 6; i++) {
    c1->cd(1);
    display_primary_secondary(projPrimaryXY[i], projSecondaryXY[i], leg1,
                              "Fraction of primary particles as a function of DCA XY", i, kFALSE);
    c1->cd(2);
    display_primary_secondary(projPrimaryZ[i], projSecondaryZ[i], nullptr,
                              "Fraction of primary particles as a function of DCA Z", i, kFALSE);
    c1->cd(4);
    display_primary_secondary(projPrimaryXY[i], projSecondaryXY[i], nullptr,
                              "Fraction of primary with given DCA xy max", i, kTRUE);
    c1->cd(5);
    display_primary_secondary(projPrimaryZ[i], projSecondaryZ[i], nullptr, "Fraction of primary with given DCA Z max",
                              i, kTRUE);
  }
  c1->cd(3);
  leg1->Draw();
}

void DrawPIDPurity()
{

  TString names[6] = {"#pi^{+}", "#pi^{-}", "K^{+}", "K^{-}", "p", "#bar{p}"};
  TCanvas* c2      = new TCanvas();
  c2->Divide(2, 3);
  Color_t spieceColor[15] = {kGray, kGreen + 2, kGreen, kRed,      kCyan,        kBlue,       kGreen + 2, kGreen,
                             kRed,  kCyan,      kBlue,  kGray + 2, kMagenta + 4, kViolet - 3, kBlack};
  for (int i = 0; i < 6; i++) {
    c2->cd(i + 1);
    THStack* st = new THStack("hs", "");
    st->SetTitle(names[i]);
    TH1D* sumH = nullptr;
    for (int j = 1; j <= 14; j++) {
      TH1D* integratedSpiece = (TH1D*) projRecoXYPid[i][j]->Clone();
      if (j == 1) sumH = integratedSpiece;
      else {
        sumH->Add(integratedSpiece);
        delete integratedSpiece;
      }
    }
    for (int j = 1; j <= 14; j++) {
      TH1D* integratedSpiece = (TH1D*) projRecoXYPid[i][j]->Clone();
      integratedSpiece->Divide(sumH);
      integratedSpiece->SetFillColor(spieceColor[j]);
      integratedSpiece->SetLineColor(spieceColor[j]);
      if (j <= 5) integratedSpiece->SetFillStyle(3145);
      if (j > 5 && j <= 10) integratedSpiece->SetFillStyle(3154);
      if (j > 10) integratedSpiece->SetFillStyle(1);

      st->Add(integratedSpiece, "hist");
    }
    st->Draw("");
    st->GetXaxis()->SetTitle(sumH->GetXaxis()->GetTitle());
    st->GetYaxis()->SetTitle("Fraction");
    delete sumH;
  }
  c2->cd(1);
  c2 = new TCanvas();
  c2->Divide(2, 3);
  for (int i = 0; i < 6; i++) {
    c2->cd(i + 1);
    THStack* st = new THStack("hs", "");
    st->SetTitle(names[i]);
    TH1D* sumH = nullptr;
    for (int j = 1; j <= 14; j++) {
      TH1D* integratedSpiece = (TH1D*) projRecoZPid[i][j]->Clone();
      if (j == 1) sumH = integratedSpiece;
      else {
        sumH->Add(integratedSpiece);
        delete integratedSpiece;
      }
    }

    for (int j = 1; j <= 14; j++) {
      TH1D* integratedSpiece = (TH1D*) projRecoZPid[i][j]->Clone();
      integratedSpiece->Divide(sumH);
      integratedSpiece->SetFillColor(spieceColor[j]);
      integratedSpiece->SetLineColor(spieceColor[j]);
      if (j <= 5) integratedSpiece->SetFillStyle(3145);
      if (j > 5 && j <= 10) integratedSpiece->SetFillStyle(3154);
      if (j > 10) integratedSpiece->SetFillStyle(1);
      st->Add(integratedSpiece, "hist");
    }
    st->Draw("");
    st->GetXaxis()->SetTitle(sumH->GetXaxis()->GetTitle());
    st->GetYaxis()->SetTitle("Fraction");
    delete sumH;
  }
  c2->cd(1);
}
