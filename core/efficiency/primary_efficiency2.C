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
 * macro to show efficiency of primary tracks reconstruction, takes into account detector's effects
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
void SetRecoCuts(NicaTrackAna* ana, NicaQACoreManager::ePidCut Pid)
{
  SetMcCuts(ana, Pid, "im");
  mngr->SetRecoTrackCut(ana, Pid, NicaQACoreManager::eParticleType::kPrimaryOnly, "re");
}
void primary_efficiency2(TString outFile  = "data.root",
                         TString simFile  = "/media/daniel/Seagate_Expansion_Drive/mass_prod/cbm/merged.root",
                         TString recoFile = "", TString parFile = "")
{

  NicaQACoreManager::ePidCut Pid = NicaQACoreManager::ePidCut::kPionPlus;

  mngr            = GetCoreManager();
  FairRunAna* run = mngr->GetRunAna(outFile, simFile, recoFile, parFile);

  NicaTrackAna* simProd = new NicaTrackAna();
  simProd->SetFormat(mngr->GetFormat(NicaQACoreManager::eFormatType::kSim));
  SetMcCuts(simProd, Pid);

  NicaTrackAna* recoProd = new NicaTrackAna();
  recoProd->SetFormat(mngr->GetFormat(NicaQACoreManager::eFormatType::kComplex));
  SetRecoCuts(recoProd, Pid);

  run->AddTask(simProd);
  run->AddTask(recoProd);
  run->Init();
  run->Run();
}
