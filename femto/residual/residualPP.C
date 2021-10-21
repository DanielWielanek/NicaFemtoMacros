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

#define UNIGEN_PATCH

/**
 * macro to show efficiency of primary tracks reconstruction
 */
NicaQACoreManager* mngr;
NicaFemtoBasicAna* dummyHbt;

class ResidualEstimation : public NicaTwoTrackAna {
  NicaFemtoPair* fHbtPair;
  TH3D *fCF, *fCF2;
  Int_t fBins;
  Double_t fMin;
  Double_t fMax;
  Bool_t fIgnore;
  Bool_t fMC;
  Int_t fPid1, fPid2;

protected:
  virtual InitStatus Init();
  virtual void Finish();
  virtual void ProcessPair();
  NicaMCTrack* GetMother(NicaMCTrack* tr);
  NicaMCTrack* GetGrandMother(NicaMCTrack* tr);
  Int_t PdgToFlag(NicaMCTrack* tr) const;
  virtual NicaPackage* Report() const;

public:
  ResidualEstimation();
  void SetPid(Int_t pid1, Int_t pid2) { fPid1 = pid1, fPid2 = pid2; }
  void SetXaxis(Int_t bins, Double_t min, Double_t max, Bool_t ignore);
  virtual ~ResidualEstimation() {}
  ClassDef(ResidualEstimation, 1)
};


#ifdef UNIGEN_PATCH
class UrQMDPath : public FairTask {
  UEvent* fEvent;

protected:
  virtual InitStatus Init();

public:
  UrQMDPath() : fEvent(nullptr) {};
  virtual void Exec(Option_t* option);
  virtual ~UrQMDPath() {};
};

#endif


void residualPP(TString outFile  = "data.root",
                TString simFile  = "/media/daniel/Seagate_Expansion_Drive/mass_prod/uni_uboot/p37UnigenEvent1.root",
                TString recoFile = "", TString parFile = "")
{
  mngr            = GetCoreManager();
  simFile         = "/media/daniel/Seagate_Expansion_Drive/mass_prod/cbm/merged.root";
  FairRunAna* run = mngr->GetRunAna(outFile, simFile, recoFile, parFile);

  Bool_t mcData               = kTRUE;
  ResidualEstimation* simProd = new ResidualEstimation();
  if (mcData) {
    simProd->SetFormat(mngr->GetFormat(NicaQACoreManager::eFormatType::kSim));
    //simProd->SetFormat(new NicaUnigenEvent());
    mngr->SetMcTrackCut(simProd, NicaQACoreManager::ePidCut::kProton, NicaQACoreManager::eParticleType::kAll);
  }
  else {
    simProd->SetFormat(mngr->GetFormat(NicaQACoreManager::eFormatType::kComplex));
    mngr->SetRecoTrackCut(simProd, NicaQACoreManager::ePidCut::kProton, NicaQACoreManager::eParticleType::kPrimaryOnly,
                          "re");
  }
  simProd->SetPid(NicaConst::ProtonPID(), NicaConst::ProtonPID());
#ifdef UNIGEN_PATCH
  run->AddTask(new UrQMDPath());
#endif
  run->AddTask(simProd);
  run->Init();
  run->Run(10);
}

//===============================================================================


ResidualEstimation::ResidualEstimation()
  : NicaTwoTrackAna(kFALSE)
  , fHbtPair(nullptr)
  , fCF(nullptr)
  , fCF2(nullptr)
  , fBins(100)
  , fMin(0)
  , fMax(0.5)
  , fIgnore(kTRUE)
  , fMC(kFALSE)
  , fPid1(2212)
  , fPid2(2212) {};

inline InitStatus ResidualEstimation::Init()
{
  InitStatus stat     = NicaTwoTrackAna::Init();
  const NicaEvent* ev = NicaDataFormatManager::Instance()->GetFormat(GetTaskID(), ENicaFormatDepth::kBuffered);
  if (ev->InheritsFrom("NicaMCEvent")) fMC = kTRUE;
  fHbtPair = new NicaFemtoPairPRF(kFALSE);
  fHbtPair->SetPdg1(fPid1);
  fHbtPair->SetPdg2(fPid2);
  fHbtPair->Init(GetTaskID());
  if (fIgnore) fHbtPair->UseAbs();
  TString array[8] = {"p", "#bar{p}", "#Lambda", "#bar{#Lambda}", "#Sigma^{+}", "#Sigma^{-}", "other", "unknown"};
  fCF              = new TH3D("CF_residual", "Mothers", fBins, fMin, fMax, 8, -0.5, 7.5, 8, -0.5, 7.5);
  fCF2             = new TH3D("CF_residual2", "Grandmothers", fBins, fMin, fMax, 8, -0.5, 7.5, 8, -0.5, 7.5);
  fCF->GetXaxis()->SetTitle("k* [GeV/c]");
  fCF2->GetXaxis()->SetTitle("k* [GeV/c]");

  for (int i = 0; i < 8; i++) {
    fCF->GetYaxis()->SetBinLabel(i + 1, array[i]);
    fCF->GetZaxis()->SetBinLabel(i + 1, array[i]);
    // fCF2->GetYaxis()->SetBinLabel(i + 1, array[i]);
    // fCF2->GetZaxis()->SetBinLabel(i + 1, array[i]);
  }

  return stat;
};

inline void ResidualEstimation::Finish() {}

inline void ResidualEstimation::ProcessPair()
{
  fHbtPair->Build(fCurrentSignalPair);

  NicaMCTrack* tr1 = nullptr;
  NicaMCTrack* tr2 = nullptr;

  if (fMC) {
    tr1 = (NicaMCTrack*) fCurrentSignalPair->GetTrack1();
    tr2 = (NicaMCTrack*) fCurrentSignalPair->GetTrack2();
  }
  else {
    NicaComplexTrack* z1 = (NicaComplexTrack*) fCurrentSignalPair->GetTrack1();
    NicaComplexTrack* z2 = (NicaComplexTrack*) fCurrentSignalPair->GetTrack2();
    tr1                  = (NicaMCTrack*) z1->GetImgTrack();
    tr2                  = (NicaMCTrack*) z2->GetImgTrack();
  }
  if (tr1 == nullptr) return;
  if (tr2 == nullptr) return;
  NicaMCTrack* m1 = GetMother(tr1);
  NicaMCTrack* g1 = GetGrandMother(tr1);

  NicaMCTrack* m2 = GetMother(tr2);
  NicaMCTrack* g2 = GetGrandMother(tr2);

  fHbtPair->Compute();
  Int_t flagM1 = PdgToFlag(m1);
  Int_t flagG1 = PdgToFlag(g1);
  Int_t flagM2 = PdgToFlag(m2);
  Int_t flagG2 = PdgToFlag(g2);
  if (flagM1 > flagM2) {
    Int_t temp = flagM1;
    flagM1     = flagM2;
    flagM2     = flagM1;
  }
  if (flagG1 > flagG2) {
    Int_t temp = flagG1;
    flagG1     = flagG2;
    flagG2     = flagG1;
  }
  Double_t kstar = fHbtPair->GetT();
  if (fHbtPair->IsAbs()) { kstar = TMath::Abs(kstar); }
  fCF->Fill(kstar, flagM1, flagM2);
  fCF2->Fill(kstar, flagG1, flagG2);
}

inline Int_t ResidualEstimation::PdgToFlag(NicaMCTrack* tr) const
{
  if (tr == nullptr) return 7;
  Int_t pid = tr->GetPdg();
  switch (pid) {
    case 2212: return 0;
    case -2212: return 1;
    case 3122: return 2;
    case -3122: return 3;
    case 3222: return 4;
    case -3222: return 5;
    default: return 6;
  }
}

inline NicaMCTrack* ResidualEstimation::GetMother(NicaMCTrack* tr)
{
  if (tr->IsPrimary()) return tr;
  Int_t index      = tr->GetMotherIndex();
  NicaEvent* event = tr->GetEvent();
  if (index < 0 || index >= event->GetTotalTrackNo()) { return nullptr; }
  return (NicaMCTrack*) event->GetTrack(index);
}

inline NicaMCTrack* ResidualEstimation::GetGrandMother(NicaMCTrack* tr)
{
  if (tr->IsPrimary()) return tr;
  Int_t motherID = tr->GetMotherIndex();
  if (motherID < 0) return nullptr;
  NicaEvent* event = tr->GetEvent();
  if (motherID >= event->GetTotalTrackNo()) return nullptr;
  NicaMCTrack* mother = (NicaMCTrack*) fCurrentEvent->GetTrack(motherID);
  if (mother->IsPrimary()) return mother;
  return GetGrandMother(mother);
}

void ResidualEstimation::SetXaxis(Int_t bins, Double_t min, Double_t max, Bool_t ignore)
{
  fBins   = bins;
  fMin    = min;
  fMax    = max;
  fIgnore = ignore;
}

inline NicaPackage* ResidualEstimation::Report() const
{
  NicaPackage* pack = NicaTwoTrackAna::Report();
  pack->AddObject(fCF);
  pack->AddObject(fCF2);
  return pack;
}

//===============================================================================

virtual InitStatus UrQMDPath::Init()
{
  fEvent = (UEvent*) NicaDataManager::Instance()->GetManager()->GetObject("UEvent.");
  return kSUCCESS;
};

virtual void UrQMDPath::Exec(Option_t* option)
{
  for (int i = 0; i < fEvent->GetNpa(); i++) {
    UParticle* p = fEvent->GetParticle(i);
    if (p->GetParent() == 0) { p->SetParent(-1); }
  }
}
