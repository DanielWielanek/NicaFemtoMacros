/*
 * show_primary_efficiency.C
 *
 *  Created on: 4 maj 2021
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#ifndef __CLING__
#define __DUMMY__
#include "../../includes/exp_includes.h"
#include "../../includes/nicafemto_includes.h"
#endif
NicaDividedHisto1D* u;  //for NicaSTd

/**
 * show raw efficiency
 * @param inFile
 */

void Prepare(TH3D* h)
{
  const Int_t maxFrac = 8;  //muset be smaller than 10!
  Color_t colors[10]  = {kRed, kBlue, kGreen, kCyan, kMagenta, kGreen + 2, kRed, kGreen, kBlue, kCyan};
  Int_t markers[10]   = {kFullCircle, kFullCircle, kFullCircle, kOpenCircle, kOpenCircle,
                       kOpenCircle, kFullSquare, kFullSquare, kFullSquare, kOpenSquare};

  Int_t bins     = h->GetNbinsY();
  TString* names = new TString[bins];
  TH1D*** histos = new TH1D**[bins];

  for (int i = 0; i < bins; i++) {
    histos[i] = new TH1D*[bins];
    names[i]  = h->GetYaxis()->GetBinLabel(i + 1);
  }
  for (int i = 0; i < bins; i++) {
    for (int j = 0; j < bins; j++) {
      histos[i][j] = NicaStd::GetProjection1D(h, i + 1, i + 1, j + 1, j + 1, "x+randname+bins");
    }
  }
  cout << 3 << endl;
  TH1D* sum = (TH1D*) histos[0][0]->Clone();
  sum->Reset();
  //adding all to sum
  for (int i = 0; i < bins; i++) {
    for (int j = 0; j < bins; j++) {
      sum->Add(histos[i][j]);
    }
  }
  cout << 1 << endl;

  TH1D** chosenFrac = new TH1D*[maxFrac + 1];
  for (int i = 0; i < maxFrac; i++)
    chosenFrac[i] = nullptr;

  for (int k = 0; k < maxFrac; k++) {
    Int_t max_histo = -1;
    Double_t max    = 0;
    Int_t maxI = 0, maxJ = 0;
    for (int i = 0; i < bins; i++) {
      for (int j = 0; j < bins; j++) {
        TH1D* test = histos[i][j];
        if (test == nullptr) continue;
        Double_t ent = test->GetEntries();
        if (ent > max) {
          max  = ent;
          maxI = i;
          maxJ = j;
        }
      }
    }
    //maximum found
    if (max > 0) {
      cout << "FOUND " << k << " " << maxI << " " << maxJ << endl;
      chosenFrac[k] = histos[maxI][maxJ];
      chosenFrac[k]->SetYTitle(Form("%s %s", names[maxI].Data(), names[maxJ].Data()));
      histos[maxI][maxJ] = nullptr;
    }
  }
  chosenFrac[maxFrac] = (TH1D*) sum->Clone();
  chosenFrac[maxFrac]->Reset();
  for (int i = 0; i < bins; i++) {
    for (int j = 0; j < i; j++) {
      if (histos[i][j] != nullptr) { chosenFrac[maxFrac]->Add(histos[i][j]); }
    }
  }

  TLegend* leg = new TLegend(0.6, 0.6, 0.95, 0.95);
  for (int i = 0; i < maxFrac + 1; i++) {
    if (chosenFrac[i] != nullptr) {
      chosenFrac[i]->Divide(sum);
      chosenFrac[i]->Scale(100.0);
      chosenFrac[i]->SetMaximum(100);
      if (i != maxFrac) {
        chosenFrac[i]->SetLineColor(colors[i]);
        chosenFrac[i]->SetMarkerColor(colors[i]);
        chosenFrac[i]->SetMarkerStyle(markers[i]);
      }
      else {
        chosenFrac[i]->SetMarkerStyle(kFullCross);
        chosenFrac[i]->SetLineColor(kGray + 2);
        chosenFrac[i]->SetMarkerColor(kGray + 2);
        chosenFrac[i]->SetYTitle("other pairs");
      }

      chosenFrac[i]->Draw("SAME");
      leg->AddEntry(chosenFrac[i], chosenFrac[i]->GetYaxis()->GetTitle(), "LP");
      chosenFrac[i]->SetYTitle("Fraction [%])");
    }
  }
  leg->Draw("SAME");
  gPad->SetGridx();
}


void show_residual(TString inFile = "data.root")
{
  gStyle->SetOptStat(0);
  gStyle->SetPalette(kRainBow);
  NicaAnaFile* file = new NicaAnaFile(inFile);

  TH3D* mothers      = (TH3D*) file->GetMainObject(2);
  TH3D* grandmothers = (TH3D*) file->GetMainObject(2);
  Prepare(mothers);
  //Prepare(grandmothers);
}
