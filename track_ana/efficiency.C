/*
 * efficiency.C
 *
 *  Created on: 31 mar 2020
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */


#ifndef __CLING__
#define __MPD__
#include "../nicafemto_includes.h"
#include "../includes/exp_includes.h"
#endif

void efficiency(TString outFile, TString simFile, TString recoFile, TString parFile=""){
    FairRunAna *run  = new FairRunAna();
    FairFileSource *file = new FairFileSource(simFile);
    if(recoFile!=""){
        file->AddFriend(recoFile);
    }
    if(parFile!=""){
        FairRuntimeDb* rtdb = run->GetRuntimeDb();
        FairParRootFileIo* parIo1 = new FairParRootFileIo();
        parIo1->open(parFile.Data());
        rtdb->setFirstInput(parIo1);
    }
    NicaTrackAna *ana = new NicaTrackAna();
    SetEventCuts(ana, kTRUE, kTRUE);
    ana->SetFormat(GetComplexFormat());




}
