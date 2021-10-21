/*
 * efficiency_task.C
 *
 *  Created on: 4 maj 2021
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */

#ifndef __CLING__
#include "CbmQACoreManager.h"

#include "../../includes/nicafemto_includes.h"
#endif
#include "../../includes/framework_def.h"
/**
 * macro to show efficiency of primary tracks reconstruction
 */
NicaQACoreManager* mngr;

const Int_t nBins   = 100;
const Double_t pMin = 0;
const Double_t pMax = 4;

void SetRecoCuts(NicaTrackAna* ana, TString opt = "")
{

  NicaQACoreManager::eParticleType type = NicaQACoreManager::eParticleType::kPrimaryOnly;
  for (int i = 0; i < 6; i++) {
    mngr->SetRecoTrackCut(ana, mngr->ToPid(i), type, Form("{%i}+fast+%s", i, opt.Data()));
  }

  NicaTrackDeltaPtCut deltaPt;
  NicaTrackDeltaPCut deltaP;
  NicaTrackDeltaPzCut deltaPz;
  NicaTrackDeltaPhiCut deltaPhi;
  NicaTrackDeltaThetaCut deltaTheta;

  ana->AddCut(deltaPt, "{6x1}");
  ana->AddCut(deltaP, "{6x1}");
  ana->AddCut(deltaPz, "{6x1}");
  ana->AddCut(deltaPhi, "{6x1}");
  ana->AddCut(deltaTheta, "{6x1}");

  NicaTrackPtCut Pt;
  NicaTrackPCut P;
  NicaTrackPzCut Pz;
  ana->AddCut(Pt, "{6x1}+im+double");
  ana->AddCut(P, "{6x1}+im+double");
  ana->AddCut(Pz, "{6x1}+im+double");


  NicaCutMonitorXY PtMon(Pt.CutName("im"), 0, deltaPt.CutName(), 0);
  PtMon.SetXaxis(nBins, pMin, pMax);
  PtMon.SetYaxis(nBins, -0.2, 0.2);
  ana->AddCutMonitor(PtMon);


  NicaCutMonitorXY PzMon(Pz.CutName("im"), 0, deltaPz.CutName(), 0);
  PzMon.SetXaxis(nBins, pMin, pMax);
  PzMon.SetYaxis(nBins, -0.2, 0.2);
  ana->AddCutMonitor(PzMon);


  NicaCutMonitorXY PMon(P.CutName("im"), 0, deltaP.CutName(), 0);
  PMon.SetXaxis(nBins, pMin, pMax);
  PMon.SetYaxis(nBins, -0.5, 0.5);
  ana->AddCutMonitor(PMon);

  NicaCutMonitorXY PPhiMon(P.CutName("im"), 0, deltaPhi.CutName(), 0);
  PPhiMon.SetXaxis(nBins, pMin, pMax);
  PPhiMon.SetYaxis(nBins, -0.5, 0.5);
  ana->AddCutMonitor(PPhiMon);

  NicaCutMonitorXY PThetaMon(P.CutName("im"), 0, deltaTheta.CutName(), 0);
  PThetaMon.SetXaxis(nBins, pMin, pMax);
  PThetaMon.SetYaxis(nBins, -0.2, 0.2);
  ana->AddCutMonitor(PThetaMon);
}

void resolution(TString outFile  = "data.root",
                TString simFile  = "/media/daniel/Seagate_Expansion_Drive/mass_prod/cbm/merged.root",
                TString recoFile = "", TString parFile = "")
{
  mngr            = GetCoreManager();
  FairRunAna* run = mngr->GetRunAna(outFile, simFile, recoFile, parFile);

  NicaTrackAna* recoProd = new NicaTrackAna();
  recoProd->SetFormat(mngr->GetFormat(NicaQACoreManager::eFormatType::kComplex));
  SetRecoCuts(recoProd, "im");


  run->AddTask(recoProd);
  run->Init();
  run->Run();
}
