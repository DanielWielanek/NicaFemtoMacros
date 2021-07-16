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


void SetMcCuts(NicaTrackAna* ana, NicaQACoreManager::ePidCut Pid, TString opt = "")
{
  NicaTrackPdgCut pid;

  NicaQACoreManager::eParticleType type = NicaQACoreManager::eParticleType::kPrimaryOnly;
  mngr->SetMcTrackCut(ana, Pid, type, "fast");

  NicaTrackPtCut pt;
  NicaTrackEtaCut eta;
  ana->AddCut(pt, opt);
  ana->AddCut(eta, opt);
  NicaCutMonitorXY mon(eta.CutName(opt), 0, pt.CutName(opt), 0);
  mon.SetXaxis(200, mngr->GetEtaMin(), mngr->GetEtaMax());
  mon.SetYaxis(200, mngr->GetPtMin(), mngr->GetPtMax());
  ana->AddCutMonitor(mon);
}

void primary_efficiency(TString outFile  = "data.root",
                        TString simFile  = "/media/daniel/Seagate_Expansion_Drive/mass_prod/cbm/merged.root",
                        TString recoFile = "", TString parFile = "")
{
  NicaQACoreManager::ePidCut PidType = NicaQACoreManager::ePidCut::kPionPlus;


  mngr = GetCoreManager();

  FairRunAna* run = mngr->GetRunAna(outFile, simFile, recoFile, parFile);

  NicaTrackAna* simProd = new NicaTrackAna();
  simProd->SetFormat(mngr->GetFormat(NicaQACoreManager::eFormatType::kSim));
  SetMcCuts(simProd, PidType);

  NicaTrackAna* recoProd = new NicaTrackAna();
  recoProd->SetFormat(mngr->GetFormat(NicaQACoreManager::eFormatType::kComplex));
  SetMcCuts(recoProd, PidType, "im");

  run->AddTask(simProd);
  run->AddTask(recoProd);
  run->Init();
  run->Run();
}
