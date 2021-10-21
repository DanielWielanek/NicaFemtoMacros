/*
 * NicaTrackPdgBinPrimCut.C
 *
 *  Created on: 4 maj 2021
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#ifndef __CLING__
#include "../../includes/nicafemto_includes.h"
#endif

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
    if (!track->IsPrimary()) { val = -val; }
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
      case 1: name = "#mu^{-}"; break;
      case 2: name = "#e^{-}"; break;
      case 3: name = "#pi^{+}"; break;
      case 4: name = "K^{+}"; break;
      case 5: name = "p"; break;
      case 6: name = "e^{+}"; break;
      case 7: name = "e^{+}"; break;
      case 8: name = "#pi^{-}"; break;
      case 9: name = "K^{-}"; break;
      case 10: name = "#bar{p}"; break;
      case 11: name = "neutral"; break;
      case 12: name = "postive"; break;
      case 13: name = "negative"; break;
      case 14: name = "unknown"; break;
    }
    if (sec) { return name + "_{sec}"; }
    return name + "_{prim}";
  }
  virtual ~NicaTrackPdgBinPrimCut() {};
  ClassDef(NicaTrackPdgBinPrimCut, 1)
};
