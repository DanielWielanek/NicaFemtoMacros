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


TH1D* projPrimaryXY;
TH1D* projPrimaryZ;
TH1D* projSecondaryXY;
TH1D* projSecondaryZ;
TH1D** projRecoXYPid;
TH1D** projRecoZPid;


void DrawPrimaryEfficiency();
void DrawPrimarySecondaryEfficiency();
void DrawPIDPurity();
TH1D* IntegrateHistogram(TH1D* h, Bool_t scale = kFALSE);
TH1D* NormalizeTo100(TH1D* h, Bool_t scale = kFALSE);
//===============================================================================
void show_primary_purity(TString inFile = "data.root")
{
  NicaAnaFile* file = new NicaAnaFile(inFile);

  TH2D* primaryMc = (TH2D*) file->GetHistogramPassed(ENicaCutUpdate::kTrackUpdate, 0, 0);

  file->SwitchPackage(1);
  TH2D* secondaryMc = (TH2D*) file->GetHistogramPassed(ENicaCutUpdate::kTrackUpdate, 0, 0);

  file->SwitchPackage(2);
  TH3D* reco = (TH3D*) file->GetHistogramPassed(ENicaCutUpdate::kTrackUpdate, 0, 0);


  projPrimaryXY   = NicaStd::GetProjection1D(primaryMc, -1, -1, "x+bins+randname");
  projPrimaryZ    = NicaStd::GetProjection1D(primaryMc, -1, -1, "y+bins+randname");
  projSecondaryXY = NicaStd::GetProjection1D(secondaryMc, -1, -1, "x+bins+randname");
  projSecondaryZ  = NicaStd::GetProjection1D(secondaryMc, -1, -1, "y+bins+randname");

  projRecoXYPid = new TH1D*[15];
  projRecoZPid  = new TH1D*[15];
  for (int j = 1; j <= 14; j++) {
    projRecoXYPid[j] = NicaStd::GetProjection1D(reco, -1, -1, j, j, "x+bins+randname");
    projRecoZPid[j]  = NicaStd::GetProjection1D(reco, -1, -1, j, j, "y+bins+randname");
  }

  gStyle->SetOptStat(0);

  DrawPrimaryEfficiency();
  DrawPrimarySecondaryEfficiency();
  DrawPIDPurity();
}
//===============================================================================
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


  auto draw_primary_effic = [](TH1D* h, Bool_t integrate, TString label) {
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
    integratedPrimary->SetLineColor(kBlue);
    integratedPrimary->SetMarkerColor(kBlue);
    integratedPrimary->SetMarkerStyle(kFullCircle);
    integratedPrimary->SetYTitle("Efficiency [%]");
    integratedPrimary->Draw("SAME");
  };

  TCanvas* c1 = new TCanvas();
  c1->Divide(2, 2);

  c1->cd(1);
  draw_primary_effic(projPrimaryXY, kTRUE, "Total % of primary particles passed by DCA XY cut");
  c1->cd(2);
  draw_primary_effic(projPrimaryZ, kTRUE, "Total % of primary particles passed by DCA Z cut");

  c1->cd(3);
  draw_primary_effic(projPrimaryXY, kFALSE, "% of primary particles with DCA XY");
  c1->cd(4);
  draw_primary_effic(projPrimaryZ, kFALSE, "% of primary particles with DCA Z");
}

void DrawPrimarySecondaryEfficiency()
{
  auto display_primary_secondary = [](TH1D* prim, TH1D* sec, TString desc, Bool_t integrate) {
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
    Primary->SetLineColor(kBlue);
    Primary->SetMarkerColor(kBlue);
    Primary->SetMarkerStyle(kFullCircle);
    Primary->SetYTitle("Fraction [%]");
    Primary->Scale(100);
    Primary->SetMaximum(105);
    Primary->Draw("SAME");
  };

  TCanvas* c1 = new TCanvas();
  c1->Divide(2, 2);

  c1->cd(1);
  display_primary_secondary(projPrimaryXY, projSecondaryXY, "Fraction of primary particles as a function of DCA XY",
                            kFALSE);
  c1->cd(2);
  display_primary_secondary(projPrimaryZ, projSecondaryZ, "Fraction of primary particles as a function of DCA Z",
                            kFALSE);
  c1->cd(3);
  display_primary_secondary(projPrimaryXY, projSecondaryXY, "Fraction of primary with given DCA xy max", kTRUE);
  c1->cd(4);
  display_primary_secondary(projPrimaryZ, projSecondaryZ, "Fraction of primary with given DCA Z max", kTRUE);
}

void DrawPIDPurity()
{

  TString names[6] = {"#pi^{+}", "#pi^{-}", "K^{+}", "K^{-}", "p", "#bar{p}"};
  TCanvas* c2      = new TCanvas();
  c2->Divide(1, 3);
  Color_t spieceColor[15] = {kGray, kGreen + 2, kGreen, kRed,      kCyan,        kBlue,       kGreen + 2, kGreen,
                             kRed,  kCyan,      kBlue,  kGray + 2, kMagenta + 4, kViolet - 3, kBlack};

  Int_t val          = 0;
  TString labels[15] = {"",        "#mu^{+}", "e^{+}",   "#pi^{+}", "K^{+}",
                        "p",  //5
                        "#mu^{-}", "e^{-}",   "#pi^{-}", "K^{-}",
                        "#bar{p}",  //10
                        "0",       "+",       "-",       "other"};

  //    case -11: val = 2; break;


  c2->cd(1);
  THStack* st  = new THStack("hs", "");
  TH1D* sumH   = nullptr;
  TLegend* leg = new TLegend(0.1, 0.1, 0.9, 0.9);
  for (int j = 1; j <= 14; j++) {
    TH1D* integratedSpiece = (TH1D*) projRecoXYPid[j]->Clone();
    if (j == 1) sumH = integratedSpiece;
    else {
      sumH->Add(integratedSpiece);
      delete integratedSpiece;
    }
  }
  for (int j = 1; j <= 14; j++) {
    TH1D* integratedSpiece = (TH1D*) projRecoXYPid[j]->Clone();
    integratedSpiece->Divide(sumH);
    integratedSpiece->SetFillColor(spieceColor[j]);
    integratedSpiece->SetLineColor(spieceColor[j]);
    if (j <= 5) integratedSpiece->SetFillStyle(3145);
    if (j > 5 && j <= 10) integratedSpiece->SetFillStyle(3154);
    if (j > 10) integratedSpiece->SetFillStyle(1);
    leg->AddEntry(integratedSpiece, labels[j], "F");

    st->Add(integratedSpiece, "hist");
  }
  st->Draw("");
  st->GetXaxis()->SetTitle(sumH->GetXaxis()->GetTitle());
  st->GetYaxis()->SetTitle("Fraction");
  delete sumH;

  c2->cd(2);
  THStack* st2 = new THStack("hs", "");
  sumH         = nullptr;
  for (int j = 1; j <= 14; j++) {
    TH1D* integratedSpiece = (TH1D*) projRecoZPid[j]->Clone();
    if (j == 1) sumH = integratedSpiece;
    else {
      sumH->Add(integratedSpiece);
      delete integratedSpiece;
    }
  }

  for (int j = 1; j <= 14; j++) {
    TH1D* integratedSpiece = (TH1D*) projRecoZPid[j]->Clone();
    integratedSpiece->Divide(sumH);
    integratedSpiece->SetFillColor(spieceColor[j]);
    integratedSpiece->SetLineColor(spieceColor[j]);
    if (j <= 5) integratedSpiece->SetFillStyle(3145);
    if (j > 5 && j <= 10) integratedSpiece->SetFillStyle(3154);
    if (j > 10) integratedSpiece->SetFillStyle(1);
    st2->Add(integratedSpiece, "his2");
  }
  st2->Draw("");
  st2->GetXaxis()->SetTitle(sumH->GetXaxis()->GetTitle());
  st2->GetYaxis()->SetTitle("Fraction");
  delete sumH;

  c2->cd(3);
  leg->Draw();
}
