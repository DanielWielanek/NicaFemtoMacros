/*
 * nicafemto_includes.h
 *
 *  Created on: 31 mar 2020
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */

#include "FairFileSource.h"
#include "FairParRootFileIo.h"
#include "FairRunAna.h"

#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <THStack.h>
#include <TLegend.h>
#include <TPad.h>
#include <TString.h>
#include <TStyle.h>

#include "NicaAnaFile.h"
#include "NicaComplexTrack.h"
#include "NicaConst.h"
#include "NicaCutMonitor.h"
#include "NicaCutMonitorX.h"
#include "NicaCutMonitorXY.h"
#include "NicaCutMonitorXYZ.h"
#include "NicaDCAPropertyMonitor.h"
#include "NicaDataFormatManager.h"
#include "NicaDividedHisto.h"
#include "NicaEvent.h"
#include "NicaEventAna.h"
#include "NicaEventVertexCut.h"
#include "NicaFemtoBasicAna.h"
#include "NicaFemtoConst.h"
#include "NicaFemtoPair.h"
#include "NicaFemtoPairKinematics.h"
#include "NicaMCTrack.h"
#include "NicaQACoreManager.h"
#include "NicaTofPropertyMonitor.h"
#include "NicaTrackAna.h"
#include "NicaTrackBasicMCCut.h"
#include "NicaTrackBasicToFCut.h"
#include "NicaTrackDCACut.h"
#include "NicaTrackDeltaMomCut.h"
#include "NicaTrackEtaCut.h"
#include "NicaTrackOnlyPrimariesCut.h"
#include "NicaTrackPCut.h"
#include "NicaTrackPdgBinCut.h"
#include "NicaTrackPdgCut.h"
#include "NicaTrackPhiCut.h"
#include "NicaTrackPtCut.h"
#include "NicaTrackPzCut.h"
#include "NicaTwoTrackAna.h"
#include "NicaUnigenEvent.h"
#include "NicaUnigenSource.h"
#include "NicaXMLNode.h"
