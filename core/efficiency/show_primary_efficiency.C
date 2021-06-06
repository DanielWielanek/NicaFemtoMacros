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

/**
 * show raw efficiency
 * @param inFile
 */

void show_primary_efficiency(TString inFile = "data.root")
{
  gStyle->SetOptStat(0);
  gStyle->SetPalette(kRainBow);
  NicaAnaFile* file = new NicaAnaFile(inFile);

  TH2D** mcHisto   = new TH2D*[6];
  TH2D** recoHisto = new TH2D*[6];

  for (int i = 0; i < 6; i++) {
    mcHisto[i] = (TH2D*) file->GetHistogramPassed(ENicaCutUpdate::kTrackUpdate, i, 0);
  }
  file->SwitchPackage(1);
  for (int i = 0; i < 6; i++) {
    recoHisto[i] = (TH2D*) file->GetHistogramPassed(ENicaCutUpdate::kTrackUpdate, i, 0);
  }
  TString names[6] = {"#pi^{+}", "#pi^{-}", "K^{+}", "K^{-}", "p", "#bar{p}"};

  TCanvas* c = new TCanvas();
  c->Divide(3, 2);
  for (int i = 0; i < 6; i++) {
    c->cd(i + 1);
    recoHisto[i]->Divide(mcHisto[i]);
    recoHisto[i]->SetMinimum(0);
    recoHisto[i]->SetMaximum(1.1);
    recoHisto[i]->SetTitle(Form("%s raw efficiency", names[i].Data()));
    recoHisto[i]->Draw("colz");
  }
}
