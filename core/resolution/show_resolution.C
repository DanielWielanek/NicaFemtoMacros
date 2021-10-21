/*
 * show_primary_efficiency.C
 *
 *  Created on: 4 maj 2021
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#ifndef __CLING__
#define __DUMMY__
#include <TF1.h>

#include "../../includes/exp_includes.h"
#include "../../includes/nicafemto_includes.h"
#endif


NicaDividedHisto1D* u;  //to load NicaStd


// RMS scale


class ResoFitter {
  TF1* FGaus;

  static const Int_t NORM = {0};
  static const Int_t SIG  = {1};
  static const Int_t MU   = {2};
  Double_t ENTRIES[2]     = {0.85, 1.25};
  Bool_t fDump            = {kFALSE};

public:
  enum kResoOpt
  {
    kFitSigma,
    kFitRMS
  };
  static Double_t Func(Double_t* x, Double_t* par)
  {
    Double_t N        = par[NORM];
    Double_t sig      = par[SIG];
    Double_t mu       = par[MU];
    const Double_t NA = N / (sig * TMath::Sqrt(2.0 * TMath::Pi()));
    Double_t y        = (x[0] - mu) / sig;
    return NA * TMath::Exp(-0.5 * y * y);
  }
  ResoFitter() { FGaus = new TF1("gaus", &ResoFitter::Func, -1, 1, 3); };
  void SetEntriesScale(Double_t min, Double_t max)
  {
    ENTRIES[0] = min;
    ENTRIES[1] = max;
  }
  /**
   *
   * @param dump if true dump given fit
   */
  void SetDump(Bool_t dump) { fDump = dump; }
  /**
   *
   * @param h histogram
   * @param opt option of resultion calculation
   * @param cent_scale if true - divide by x-bin center
   * @return
   */
  TH1D* GetHisto(TH2D* h, kResoOpt opt, Bool_t cent_scale = kFALSE)
  {
    TString titleX    = h->GetXaxis()->GetTitle();
    TString titleY    = h->GetYaxis()->GetTitle();
    TString unitlessX = NicaStd::RemoveUnits(titleX);
    TString unitlessY = NicaStd::RemoveUnits(titleY);
    TString title     = Form("%s(%s)", unitlessY.Data(), unitlessX.Data());
    Int_t bins;
    Double_t min, max;
    NicaStd::GetAxisPar(*h, bins, min, max, "x");
    TH1D* res = new TH1D(h->GetName(), title, bins, min, max);
    res->GetXaxis()->SetTitle(titleX);
    res->GetYaxis()->SetTitle(Form("%s", titleY.Data()));
    const Double_t width = h->GetYaxis()->GetBinWidth(1);
    for (int i = 1; i <= h->GetNbinsX(); i++) {
      TH1D* proj = NicaStd::GetProjection1D(h, i, i, "y+bins");
      if (proj->GetEntries() < 10) continue;
      Double_t rms  = proj->GetRMS();
      Double_t rmsE = proj->GetRMSError();
      if (opt == kResoOpt::kFitSigma) {
        Double_t ent = proj->GetEntries() * width;
        FGaus->SetParameter(MU, 0);
        FGaus->SetParameter(SIG, rms);
        FGaus->SetParameter(NORM, ent);
        FGaus->SetParLimits(NORM, ent * ENTRIES[0], ent * ENTRIES[1]);
        FGaus->SetParLimits(SIG, rms * 0.25, rms * 2.0);
        FGaus->SetParLimits(MU, -0.2, 0.2);

        proj->Fit(FGaus, "NQ");
        res->SetBinContent(i, FGaus->GetParameter(1));
        res->SetBinError(i, FGaus->GetParError(1));

        if (fDump) {
          gROOT->SetBatch(kTRUE);
          TCanvas* cx = new TCanvas();
          proj->Draw();
          gSystem->mkdir("dump");
          FGaus->Draw("SAME");
          cx->SaveAs(Form("dump/%i.png", i));
          gROOT->SetBatch(kFALSE);
        }

        if (cent_scale) {
          Double_t cent = h->GetXaxis()->GetBinCenter(i);
          res->SetBinContent(i, FGaus->GetParameter(1) / cent);
          res->SetBinError(i, FGaus->GetParError(1) / cent);
        }
      }
      else {
        res->SetBinContent(i, rms);
        res->SetBinError(i, rmsE);
        if (cent_scale) {
          Double_t cent = h->GetXaxis()->GetBinCenter(i);
          res->SetBinContent(i, rms / cent);
          res->SetBinError(i, rmsE / cent);
        }
      }
    }

    return res;
  }
  virtual ~ResoFitter() {};
};


void SetHistos(TH1D* hx1, TH1D* hx2)
{
  hx1->SetMarkerColor(kRed);
  hx1->SetLineColor(kRed);
  hx1->SetMarkerStyle(kFullCircle);
  hx2->SetMarkerColor(kBlue);
  hx2->SetLineColor(kBlue);
  hx2->SetMarkerStyle(kFullCircle);
  if (hx1->GetMaximum() > 1) hx1->SetMaximum(1);
  hx1->Draw();
  hx2->Draw("SAME");
  gPad->SetGridx();
  gPad->SetGridy();
}


void show_resolution(TString inFile = "data.root")
{
  gStyle->SetOptStat(0);
  gStyle->SetPalette(kRainBow);
  ResoFitter* fitter = new ResoFitter();
  NicaAnaFile* file  = new NicaAnaFile(inFile);

  TString names[6] = {"#pi^{+}", "#pi^{-}", "K^{+}", "K^{-}", "p", "#bar{p}"};
  for (int i = 0; i < 6; i++) {
    TCanvas* c = new TCanvas(names[i]);
    c->Divide(2, 3);
    for (int j = 0; j < 4; j++) {
      c->cd(j + 1);
      TH2D* h   = (TH2D*) file->GetHistogramPassed(ENicaCutUpdate::kTrackUpdate, i, j);
      TH1D* hx1 = fitter->GetHisto(h, ResoFitter::kResoOpt::kFitSigma);
      TH1D* hx2 = fitter->GetHisto(h, ResoFitter::kResoOpt::kFitRMS);
      SetHistos(hx1, hx2);

    }  //
    c->cd(5);
    TH2D* h   = (TH2D*) file->GetHistogramPassed(ENicaCutUpdate::kTrackUpdate, i, 2);
    TH1D* hx1 = fitter->GetHisto(h, ResoFitter::kResoOpt::kFitSigma, kTRUE);
    TH1D* hx2 = fitter->GetHisto(h, ResoFitter::kResoOpt::kFitRMS, kTRUE);
    hx1->GetYaxis()->SetTitle("#Delta p/p");
    SetHistos(hx1, hx2);

    c->cd(6);
    h   = (TH2D*) file->GetHistogramPassed(ENicaCutUpdate::kTrackUpdate, i, 0);
    hx1 = fitter->GetHisto(h, ResoFitter::kResoOpt::kFitSigma, kTRUE);
    hx2 = fitter->GetHisto(h, ResoFitter::kResoOpt::kFitRMS, kTRUE);
    hx1->GetYaxis()->SetTitle("#Delta p_{T}/p_{T}");
    SetHistos(hx1, hx2);


    TLegend* leg = new TLegend(0.6, 0.6, 0.95, 0.95);
    leg->SetHeader(names[i]);
    leg->AddEntry(hx1, "sigma fit");
    leg->AddEntry(hx2, "RMS");
    leg->Draw("SAME");
  }
}
