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
 * show efficiency after applying of cuts
 * @param inFile
 */

void show_primary_efficiency2(TString inFile = "data.root")
{
  gStyle->SetOptStat(0);
  gStyle->SetPalette(kRainBow);

  NicaAnaFile* file = new NicaAnaFile(inFile);
  TH2D* mcHisto     = (TH2D*) file->GetHistogramPassed(ENicaCutUpdate::kTrackUpdate, 0, 0);
  file->SwitchPackage(1);
  TH2D* recoHisto = (TH2D*) file->GetHistogramPassed(ENicaCutUpdate::kTrackUpdate, 0, 0);

  TCanvas* c = new TCanvas();
  recoHisto->Divide(mcHisto);
  recoHisto->SetMinimum(0);
  recoHisto->SetMaximum(1.1);
  recoHisto->SetTitle("Efficiency");
  recoHisto->Draw("colz");
}
