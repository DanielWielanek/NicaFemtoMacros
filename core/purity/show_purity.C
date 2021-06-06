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


void show_purity(TString inFile = "data.root")
{
  gStyle->SetOptStat(0);
  gStyle->SetPalette(kRainBow);
  const Int_t mid   = 7;
  Int_t enumPids[6] = {3, -3, 4, -4, 5, -5};
  for (int i = 0; i < 6; i++)
    enumPids[i] = enumPids[i] + mid;

  NicaAnaFile* file       = new NicaAnaFile(inFile);
  ExtractedPids** RawPids = new ExtractedPids*[6];
  ExtractedPids** IdPids  = new ExtractedPids*[6];
  for (int i = 0; i < 6; i++) {
    TH3D* t = (TH3D*) file->GetHistogramPassed(ENicaCutUpdate::kTrackUpdate, i, 0);

    RawPids[i] = new ExtractedPids(t);
  }
  file->SwitchPackage(1);
  for (int i = 0; i < 6; i++) {
    TH3D* t   = (TH3D*) file->GetHistogramPassed(ENicaCutUpdate::kTrackUpdate, i, 0);
    IdPids[i] = new ExtractedPids(t);
  }

  TCanvas* c = new TCanvas();
  c->Divide(3, 2);
  for (int i = 0; i < 6; i++) {
    c->cd(i + 1);
    TH2D* sum = (TH2D*) RawPids[i]->GetTH2(1)->Clone();
    for (int iPid = 2; iPid <= 14; iPid++)
      sum->Add(RawPids[i]->GetTH2(iPid));
    TH2D* correct = (TH2D*) RawPids[i]->GetTH2(enumPids[i])->Clone();
    correct->Divide(sum);
    correct->Scale(100);
    correct->SetMinimum(0);
    correct->SetMaximum(100);
    TString title = Form("Raw purity of %s", NamesHadrons[i].Data());
    correct->SetTitle(title);
    correct->Draw("colz");
  }

  c = new TCanvas();
  c->Divide(3, 2);
  for (int i = 0; i < 6; i++) {
    c->cd(i + 1);
    TH2D* sum = (TH2D*) IdPids[i]->GetTH2(1)->Clone();
    for (int iPid = 2; iPid <= 14; iPid++)
      sum->Add(IdPids[i]->GetTH2(iPid));
    TH2D* correct = (TH2D*) IdPids[i]->GetTH2(enumPids[i])->Clone();
    correct->Divide(sum);
    correct->Scale(100);
    correct->SetMinimum(0);
    correct->SetMaximum(100);
    TString title = Form("Cutted purity of %s", NamesHadrons[i].Data());
    correct->SetTitle(title);
    correct->Draw("colz");
  }


  TCanvas* c2 = new TCanvas();
  c2->Divide(3, 2);
  TLegend* leg = new TLegend(0.1, 0.1, 0.9, 0.9);
  for (int i = 0; i < 6; i++) {
    c2->cd(i + 1);
    TH1D* sum = IdPids[i]->GetTH1(1);
    for (int iPid = 2; iPid <= 14; iPid++)
      sum->Add(IdPids[i]->GetTH1(iPid));
    for (int iPid = 1; iPid <= 14; iPid++) {
      TH1D* slice = IdPids[i]->GetTH1(iPid);
      slice->Divide(sum);
      slice->Scale(100);
      slice->SetMarkerColor(colz[iPid]);
      slice->SetLineColor(colz[iPid]);
      slice->SetMarkerStyle(marker[iPid]);
      slice->SetMinimum(0);
      slice->SetMaximum(100);
      slice->Draw("SAME");
      slice->SetTitle(NamesHadrons[i]);
      if (i == 0) { leg->AddEntry(slice, NamesAll[iPid], "LP"); }
    }  //
  }
  TCanvas* l = new TCanvas();
  leg->Draw();
}
