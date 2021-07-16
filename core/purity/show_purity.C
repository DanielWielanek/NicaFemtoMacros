/*
 * show_purity.C
 *
 *  Created on: 26 maj 2021
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#ifndef __CLING__
#include "CbmQACoreManager.h"

#include "../../includes/nicafemto_includes.h"
#endif
#include "../../includes/framework_def.h"

// clang-format off
Color_t colz[15] = {kBlack,
        kGreen+2, kBlue,kMagenta+2,kRed,kGreen,kCyan,kYellow,
        kPink, kGreen, kRed,kMagenta+2,kBlue,kGreen+2,kBlack

};
Int_t marker[15] = {
        kDot,
        kOpenTriangleDown, kOpenCircle, kOpenCircle, kOpenCircle, kOpenTriangleUp, kFullSquare,kFullSquare,
        kFullSquare,kFullTriangleUp,kFullCircle,kFullCircle,kFullCircle,kFullTriangleDown,kFullStar};


TString NamesHadrons[6] = {"#pi^{+}", "#pi^{-}", "K^{+}", "K^{-}", "p", "#bar{p}"};
TString NamesAll[15]    = {"",
        //-6
        "#mu^{-}", "#bar{p}", "K^{-}",   "#pi^{-}",  "e^{-}", "negative", "neutral",
       "positive","e^{+}","#pi^{+}","K^{+}","p","#mu^{+}","unknown"};

// clang-format on
class ExtractedPids {

public:
  TH2D** fTH;

  ExtractedPids(TH3D* h = nullptr)
  {
    fTH = new TH2D*[15];
    for (int j = 1; j <= 14; j++) {
      fTH[j] = NicaStd::GetProjection2D(h, j, j, "yx+bins+randname");
    }
  }
  TH2D* GetTH2(Int_t pos) { return fTH[pos]; };
  TH1D* GetTH1(Int_t pos) { return NicaStd::GetProjection1D(fTH[pos], -1, -1, "x+randname+bins"); }
  virtual ~ExtractedPids() {};
};

void SetAxis(TH1* h, Int_t charge)
{
  Int_t bins;
  Double_t min, max;
  NicaStd::GetAxisPar(*h, bins, min, max, "x");
  if (charge == 1) { h->GetXaxis()->SetRangeUser(0, max); }
  else {
    h->GetXaxis()->SetRangeUser(min, 0);
  }
}


void show_purity(TString inFile = "data.root")
{

  NicaQACoreManager::ePidCut PidType = NicaQACoreManager::ePidCut::kPionPlus;

  Double_t min, max;
  Int_t bins;
  Int_t correctId = 0;
  Int_t charge    = 0;
  switch (PidType) {
    case NicaQACoreManager::ePidCut::kPionPlus: {
      correctId = 0;
      charge    = 1;
    } break;
    case NicaQACoreManager::ePidCut::kPionMinus: {
      correctId = 1;
      charge    = -1;
    } break;
    case NicaQACoreManager::ePidCut::kKaonPlus: {
      correctId = 2;
      charge    = 1;
    } break;
    case NicaQACoreManager::ePidCut::kKaonMinus: {
      correctId = 3;
      charge    = -1;
    } break;
    case NicaQACoreManager::ePidCut::kProton: {
      correctId = 4;
      charge    = 1;
    } break;
    case NicaQACoreManager::ePidCut::kAntiProton: {
      correctId = 5;
      charge    = -1;
    } break;
    default: break;
  }
  gStyle->SetOptStat(0);
  gStyle->SetPalette(kRainBow);
  const Int_t mid   = 7;
  Int_t enumPids[6] = {3, -3, 4, -4, 5, -5};
  for (int i = 0; i < 6; i++)
    enumPids[i] = enumPids[i] + mid;

  NicaAnaFile* file = new NicaAnaFile(inFile);

  TH3D* t                = (TH3D*) file->GetHistogramPassed(ENicaCutUpdate::kTrackUpdate, 0, 0);
  ExtractedPids* RawPids = new ExtractedPids(t);
  file->SwitchPackage(1);
  t                     = (TH3D*) file->GetHistogramPassed(ENicaCutUpdate::kTrackUpdate, 0, 0);
  ExtractedPids* IdPids = new ExtractedPids(t);


  TCanvas* c = new TCanvas();
  c->Divide(2, 2);

  c->cd(1);
  TH2D* sum = (TH2D*) RawPids->GetTH2(1)->Clone();
  for (int iPid = 2; iPid <= 14; iPid++)
    sum->Add(RawPids->GetTH2(iPid));
  TH2D* correct = (TH2D*) RawPids->GetTH2(enumPids[correctId])->Clone();
  correct->Divide(sum);
  correct->Scale(100);
  correct->SetMinimum(0);
  correct->SetMaximum(100);
  TString title = "Raw purity";
  correct->SetTitle(title);
  SetAxis(correct, charge);
  correct->Draw("colz");
  gPad->SetGridx();
  gPad->SetGridy();


  c->cd(2);
  sum = (TH2D*) IdPids->GetTH2(1)->Clone();
  for (int iPid = 2; iPid <= 14; iPid++)
    sum->Add(IdPids->GetTH2(iPid));
  correct = (TH2D*) IdPids->GetTH2(enumPids[correctId])->Clone();
  correct->Divide(sum);
  correct->Scale(100);
  correct->SetMinimum(0);
  correct->SetMaximum(100);
  title = "Cutted purity";
  correct->SetTitle(title);
  correct->Draw("colz");
  SetAxis(correct, charge);
  correct->Draw("colz");
  gPad->SetGridx();
  gPad->SetGridy();

  c->cd(3);
  TLegend* leg = new TLegend(0.1, 0.1, 0.9, 0.9);
  TH1D* sum1   = IdPids->GetTH1(1);
  for (int iPid = 2; iPid <= 14; iPid++)
    sum1->Add(IdPids->GetTH1(iPid));
  for (int iPid = 1; iPid <= 14; iPid++) {
    TH1D* slice = IdPids->GetTH1(iPid);
    slice->Divide(sum1);
    slice->Scale(100);
    slice->SetMarkerColor(colz[iPid]);
    slice->SetLineColor(colz[iPid]);
    slice->SetMarkerStyle(marker[iPid]);
    slice->SetMinimum(0);
    slice->SetMaximum(100);
    slice->Draw("SAME");
    slice->SetTitle(NamesHadrons[correctId]);
    leg->AddEntry(slice, NamesAll[iPid], "LP");
    if (iPid == 1) {
      gPad->SetGridx();
      gPad->SetGridy();
      SetAxis(slice, charge);
    }
  }  //
  c->cd(4);
  leg->Draw();
}
