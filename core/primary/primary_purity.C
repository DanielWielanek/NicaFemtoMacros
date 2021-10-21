#ifndef __CLING__
#include "CbmQACoreManager.h"

#include "../../includes/nicafemto_includes.h"
#endif
#include "../../includes/framework_def.h"


class NicaTrackPdgBinPrimCut : public NicaTrackCut {

public:
  NicaTrackPdgBinPrimCut() : NicaTrackCut(1) { SetUnitName("PdgPrimBin [AU]"); };
  Bool_t Pass(NicaTrack* track)
  {
    NicaMCTrack* tr = (NicaMCTrack*) track;
    Int_t pdg       = tr->GetPdg();
    Int_t val       = 0;
    switch (pdg) {
      case -13: val = 1; break;
      case -11: val = 2; break;
      case 211: val = 3; break;
      case 321: val = 4; break;
      case 2212: val = 5; break;

      case 13: val = 6; break;
      case 11: val = 7; break;
      case -211: val = 8; break;
      case -321: val = 9; break;
      case -2212: val = 10; break;
      default: {
        Int_t charge = track->GetCharge();
        switch (charge) {
          case 0: val = 11; break;
          case 1: val = 12; break;
          case -1: val = 13; break;
          default: val = 14; break;
        }
      } break;
    };
    //  if (!track->IsPrimary()) { val = -val; }
    SetValue(val);
    return Validate();
  };
  TString GetBinName(Int_t bin)
  {
    Bool_t sec = kFALSE;
    if (bin < 0) {
      sec = kTRUE;
      bin = -bin;
    }
    TString name;
    switch (bin) {
      case 1: name = "#mu^{+}"; break;
      case 2: name = "#e^{+}"; break;
      case 3: name = "#pi^{+}"; break;
      case 4: name = "K^{+}"; break;
      case 5: name = "p"; break;
      case 6: name = "#mu^{-}"; break;
      case 7: name = "e^{-}"; break;
      case 8: name = "#pi^{-}"; break;
      case 9: name = "K^{-}"; break;
      case 10: name = "#bar{p}"; break;
      case 11: name = "neutral"; break;
      case 12: name = "positive"; break;
      case 13: name = "negative"; break;
      case 14: name = "unknown"; break;
    }
    if (sec) { return name + "_{sec}"; }
    return name + "_{prim}";
  }
  virtual ~NicaTrackPdgBinPrimCut() {};
  ClassDef(NicaTrackPdgBinPrimCut, 1)
};

class NicaTrackDCACutDummy : public NicaTrackDCACut {
public:
  NicaTrackDCACutDummy() {};
  virtual ~NicaTrackDCACutDummy() {};
  ClassDef(NicaTrackDCACutDummy, 1);
};


/**
 * macro to show efficiency of primary tracks reconstruction
 */
NicaQACoreManager* mngr;

Int_t dcaXYbins = 200;
Int_t dcaZbins  = 200;


void SetMcCuts(NicaTrackAna* ana, NicaQACoreManager::ePidCut PidType, Bool_t primary)
{
  NicaTrackPdgCut pid;
  NicaQACoreManager::eParticleType type = NicaQACoreManager::eParticleType::kAll;
  for (int i = 0; i < 6; i++) {
    mngr->SetMcTrackCut(ana, mngr->ToPid(i), type, Form("{%i}+fast+im", i));
  }

  NicaTrackOnlyPrimariesCut prim;
  if (primary) { ana->AddCut(prim, "{6x1}+fast+im"); }
  else {
    prim.AcceptOnlyNonPrimaries();
    ana->AddCut(prim, "{6x1}+fast+im");
  }

  NicaTrackDCACut dca;
  ana->AddCut(dca, "{6x1}+re");

  NicaDCAPropertyMonitor mon(kTRUE);
  mon.SetXaxis(dcaXYbins, 0, mngr->GetDcaXY());
  mon.SetYaxis(dcaZbins, mngr->GetDcaZMin(), mngr->GetDcaZMax());
  ana->AddCutMonitor(mon);
}

void SetRecoCuts(NicaTrackAna* ana, NicaQACoreManager::ePidCut PidType)
{
  NicaTrackPdgCut pid;
  NicaQACoreManager::eParticleType type = NicaQACoreManager::eParticleType::kPrimaryOnly;
  mngr->SetRecoTrackCut(ana, PidType, type, "fast+re");
  NicaTrackDCACutDummy dca;
  ana->AddCut(dca, "re+double");
  NicaTrackPdgBinPrimCut pdgCut;
  ana->AddCut(pdgCut, "im");
  NicaCutMonitorXYZ mon(dca.CutName("re"), dca.DCAxy(), dca.CutName("re"), dca.DCAz(), pdgCut.CutName("im"), 0);
  mon.SetXaxis(dcaXYbins, 0, mngr->GetDcaXY());
  mon.SetYaxis(dcaZbins, mngr->GetDcaZMin(), mngr->GetDcaZMax());
  mon.SetZaxis(14, 0.5, 14.5);
  ana->AddCutMonitor(mon);
}


void primary_purity(TString outFile  = "data.root",
                    TString simFile  = "/media/daniel/Seagate_Expansion_Drive/mass_prod/cbm/merged.root",
                    TString recoFile = "", TString parFile = "")
{
  NicaQACoreManager::ePidCut PidType = NicaQACoreManager::ePidCut::kPionPlus;

  mngr = GetCoreManager();
  mngr->UsePrimSecCut(kFALSE);
  FairLogger::GetLogger()->SetColoredLog(kTRUE);
  FairRunAna* run = mngr->GetRunAna(outFile, simFile, recoFile, parFile);

  NicaTrackAna* simProdPrim = new NicaTrackAna();
  simProdPrim->SetFormat(mngr->GetFormat(NicaQACoreManager::eFormatType::kComplex));
  SetMcCuts(simProdPrim, PidType, kTRUE);

  NicaTrackAna* simProdSec = new NicaTrackAna();
  simProdSec->SetFormat(mngr->GetFormat(NicaQACoreManager::eFormatType::kComplex));
  SetMcCuts(simProdSec, PidType, kFALSE);

  NicaTrackAna* recoProd = new NicaTrackAna();
  recoProd->SetFormat(mngr->GetFormat(NicaQACoreManager::eFormatType::kComplex));
  SetRecoCuts(recoProd, PidType);

  run->AddTask(simProdPrim);
  run->AddTask(simProdSec);
  run->AddTask(recoProd);
  run->Init();
  run->Run();
}
