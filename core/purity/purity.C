/*
 * purity.C
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

#define pRange 10.0

NicaQACoreManager* mngr;

void SetRecoCuts(NicaTrackAna* ana, Bool_t raw)
{
  if (raw) { mngr->UsePidCut(kFALSE); }
  else {
    mngr->UsePidCut(kTRUE);
  }
  NicaTrackPdgBinCut binCut;
  ana->AddCut(binCut, "im+{6x1}");
  NicaTrackBasicToFCut tof;
  tof.SetMinimum(-1E+9, tof.Mass2());
  ana->AddCut(tof, "{6x1}+re");
  NicaTrackPSignedCut p;
  ana->AddCut(p, "{6x1}+re");
  NicaCutMonitorXYZ mon3d(p.CutName("re"), 0, tof.CutName("re"), tof.Mass2(), binCut.CutName("im"), 0);

  mon3d.SetXaxis(200, -pRange, pRange);
  mon3d.SetYaxis(200, -0.2, 1.2);
  mon3d.SetZaxis(14, -6.5, 7.5);
  ana->AddCutMonitor(mon3d);

  NicaQACoreManager::eParticleType type = NicaQACoreManager::eParticleType::kPrimaryOnly;
  for (int i = 0; i < 6; i++) {
    mngr->SetRecoTrackCut(ana, mngr->ToPid(i), type, Form("{%i}+re", i));
  }
}

void purity(TString outFile = "data.root",
            TString simFile = "/media/daniel/Seagate_Expansion_Drive/mass_prod/cbm/merged.root", TString recoFile = "",
            TString parFile = "")
{

  mngr            = GetCoreManager();
  FairRunAna* run = mngr->GetRunAna(outFile, simFile, recoFile, parFile);

  NicaTrackAna* recoRaw = new NicaTrackAna();
  SetRecoCuts(recoRaw, kTRUE);
  NicaTrackAna* recoCorrect = new NicaTrackAna();
  SetRecoCuts(recoCorrect, kFALSE);

  recoRaw->SetFormat(mngr->GetFormat(NicaQACoreManager::eFormatType::kComplex, NicaQACoreManager::eAnaType::kDefault));
  recoCorrect->SetFormat(
    mngr->GetFormat(NicaQACoreManager::eFormatType::kComplex, NicaQACoreManager::eAnaType::kDefault));


  run->AddTask(recoRaw);
  run->AddTask(recoCorrect);

  run->Init();
  run->Run(8000);
}
