/*
 * mpd_base_macro.C
 *
 *  Created on: 31 mar 2020
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */

#ifndef __CLING__
#include "../../nicafemto_includes.h"
#include "NicaMpdEvent.h"
#include "NicaMpdDstMCEvent.h"
#include "NicaMpdMcEvent.h"
#include "NicaTrackTpcToFCut.h"

#endif

NicaEvent *GetBasicFormat(){
    return NicaMpdEvent();
}
NicaEvent *GetComplexFormat(){
    return new NicaMpdDstMCEvent();
}
NicaEvent GetMCFormat(){
    return new NicaMpdMcEvent();
}
void SetEventCuts(NicaEventAna *ana, Bool_t complex = kFALSE,Bool_t monitor=kTRUE){
    TString addOpt = "";
    Int_t par_shift=0;
    if(complex){
        par_shift = 1;
        addOpt = "re";
    }
    NicaEventVertexCut vertex;
    ana->AddCut(vertex,addOpt);
    NicaCutMonitorXY mon(vertex.CutName(addOpt),vertex.Z(),vertex.CutName(addOpt),vertex.Rt());
    mon.SetXaxis(100,-50, 50);
    mon.SetYaxis(100, 0, 5);
    ana->AddCutMonitor(mon, addOpt);
}

void SetTrackCutsMC(NicaEventAna *ana, Int_t pid = 211, Bool_t complex = kFALSE,Bool_t monitor=kTRUE){
    TString addOpt = "";
    Int_t par_shift=0;
    if(complex){
        par_shift = 1;
        addOpt = "im";
    }
    NicaTrackBasicMCCut mc;
    mc.SetPdgCut(pid);
    ana->Add(mc,addOpt);
    NicaCutMonitorXY kin(mc.CutName(addOpt),mc.Eta()+par_shift,
            mc.CutName(addOpt),mc.Pt()+par_shift);
    kin.SetXaxis(200, 0, 4);
    kin.SetYaxis(200, -2, 2);
    ana->AddCutMonitor(kin);
}

void SetTrackCutsAnyPion(NicaEventAna *ana, Bool_t complex=kFALSE,Bool_t monitored=kTRUE){
    TString addOpt = "";
    Int_t par_shift=0;
    if(complex){
        par_shift = 1;
        addOpt = "re";
    }
    NicaTrackTpcToFCut basic;
    basic.SetActiveSigma(NicaTrackTpcToFCut::PionSigma());
    basic.SetSigma(-2, 2, NicaTrackTpcToFCut::PionSigma());
    basic.SetModeNotBad();
    basic.SetNHits(30, 90);
    basic.SetThreshold(0.5);
    basic.SetToFPolyLIneUp(-0.3, 0, 0);
    basic.SetToFPolyLineDown(0.15, 0, 0);

    NicaTrackPCut p;
    NicaCutMonitorXY dedx(p.CutName(addOpt),0,basic.CutName(addOpt),basic.DeDx());
    dedx.SetXaxis(100, 0, 3);
    dedx.SetYaxis(100, 1E+3, 6E+4);
    NicaTrackPtCut pt;
    NicaTrackEtaCut eta;
    NicaCutMonitorXY kin(pt.CutName(addOpt),0,eta.CutName(addOpt),0);
    kin.SetXaxis(200, 0, 4);
    kin.SetYaxis(200, -2, 2);

    ana->AddCut(basic, addOpt);
    ana->AddCut(p, addOpt);
    ana->AddCut(pt, addOpt);
    ana->AddCut(eta, addOpt);
    ana->AddCutMonitor(dedx);
    ana->AddCutMonitor(kin);


}



