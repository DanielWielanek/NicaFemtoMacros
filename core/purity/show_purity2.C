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


void show_purity2(TString inFile = "data.root")
{
  Int_t enumPids[6] = {3, 8, 4, 9, 5, 10};
  Color_t colz[6]   = {kRed, kRed + 2, kCyan, kCyan + 3, kBlue, kBlue + 2};
  Int_t marker[6]   = {kFullTriangleDown, kFullTriangleUp,   kFullTriangleDown,
                     kFullTriangleUp,   kFullTriangleDown, kFullTriangleUp};
  NicaAnaFile* file = new NicaAnaFile(inFile);

  TH3D* t = (TH3D*) file->GetHistogramPassed(ENicaCutUpdate::kTrackUpdate, 0, 0);
  cout << t->GetTitle() << endl;
  cout << t->GetName() << endl;
  TString tit = t->GetTitle();
  // tit.ReplaceAll("P_{signed} _{re}", "x");
  // tit.ReplaceAll("PdgPrimBin _{im}", "y");
  tit.ReplaceAll("m_{ToF}_^{2}_{re}", "oo");
  t->SetTitle(tit);
  t->SetName("tt");
  t->GetXaxis()->SetTitle("xx");
  t->GetYaxis()->SetTitle("yy");
  t->GetZaxis()->SetTitle("zz");
  for (int i = 1; i <= t->GetNbinsX(); i++) {
    for (int j = 1; j <= t->GetNbinsY(); j++) {
      for (int k = 1; k <= t->GetNbinsZ(); k++) {
        if (t->GetBinContent(i, j, k) != 0)
          cout << i << " " << j << " " << k << " " << t->GetBinContent(i, j, k) << endl;
      }
    }
  }

  t->Draw("box2");
}
