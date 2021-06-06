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
void SetMcCuts(NicaTrackAna* ana, TString opt = "")
{
  NicaTrackPdgCut pid;

  NicaQACoreManager::eParticleType type = NicaQACoreManager::eParticleType::kPrimaryOnly;

  for (int i = 0; i < 6; i++) {
    mngr->SetMcTrackCut(ana, mngr->ToPid(i), type, Form("{%i}+fast+%s", i, opt.Data()));
  }

  NicaTrackPtCut pt;
  NicaTrackEtaCut eta;
  TString patt = "{6x1}";
  if (opt == "im") patt = "{6x1}+im";
  ana->AddCut(pt, patt);
  ana->AddCut(eta, patt);
  NicaCutMonitorXY mon(eta.CutName(opt), 0, pt.CutName(opt), 0);
  mon.SetXaxis(200, mngr->GetEtaMin(), mngr->GetEtaMax());
  mon.SetYaxis(200, mngr->GetPtMin(), mngr->GetPtMax());
  ana->AddCutMonitor(mon);
}

void primary_efficiency(TString outFile  = "data.root",
                        TString simFile  = "/media/daniel/Seagate_Expansion_Drive/mass_prod/cbm/merged.root",
                        TString recoFile = "", TString parFile = "")
{
  mngr            = GetCoreManager();
  FairRunAna* run = mngr->GetRunAna(outFile, simFile, recoFile, parFile);

  NicaTrackAna* simProd = new NicaTrackAna();
  simProd->SetFormat(mngr->GetFormat(NicaQACoreManager::eFormatType::kSim));
  SetMcCuts(simProd);

  NicaTrackAna* recoProd = new NicaTrackAna();
  recoProd->SetFormat(mngr->GetFormat(NicaQACoreManager::eFormatType::kComplex));
  SetMcCuts(recoProd, "im");

  run->AddTask(simProd);
  run->AddTask(recoProd);
  run->Init();
  run->Run();
}
