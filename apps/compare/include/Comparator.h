#pragma once

#include "Ploter.h"
#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"

#include <memory>
#include <string>

class Comparator
{
public:
  Comparator(const std::string& old, const std::string& nouveau);
  void Run();
  void SaveAsPDF(const std::string& file);

private:
  std::unique_ptr<ROOT::RDataFrame> m_oldData{nullptr};
  std::unique_ptr<ROOT::RDataFrame> m_newData{nullptr};
  std::string                       m_TreeName   = "Raw_Hit";
  std::string                       BR_CELL_ID_A = "CellID";
  std::string                       BR_TAG_A     = "HitTag";

  template<typename T> void generateCanvas(ROOT::RDF::RResultPtr<T>& a, ROOT::RDF::RResultPtr<T>& b, const std::string titles = "A;B;C", bool loga = true)
  {
    double x_min{TMath::Max(a->GetXaxis()->GetXmin(), b->GetXaxis()->GetXmin())};
    double x_max{TMath::Max(a->GetXaxis()->GetXmax(), b->GetXaxis()->GetXmax())};

    a->SetLineWidth(2);
    b->SetLineWidth(2);

    a->SetLineColor(kRed);
    b->SetLineColor(kBlue);

    b->SetLineStyle(7);

    /********  Draw  ********/
    gStyle->SetOptStat(0);
    TCanvas c;
    c.SetWindowSize(1200, 800);
    TPad top_pad("top_pad", "", 0, 0.4, 1, 1);
    TPad bottom_pad("bottom_pad", "", 0, 0, 1, 0.4);
    top_pad.Draw();
    bottom_pad.Draw();
    // - Spectrum
    top_pad.cd();
    gPad->SetRightMargin(0.05);
    gPad->SetLeftMargin(0.12);
    gPad->SetTopMargin(0.10);
    gPad->SetBottomMargin(0.05);
    a->GetXaxis()->SetLabelOffset(99);
    a->GetXaxis()->SetTitleOffset(99);
    a->SetMaximum(TMath::Max(a->GetMaximum() * 1.1, b->GetMaximum() * 1.1));
    a->Draw("");
    b->Draw("same");
    if(loga) gPad->SetLogy();
    TLegend* top_lg = gPad->BuildLegend(0.2, 0.15, 0.2, 0.15);
    top_lg->SetBorderSize(0);
    top_lg->SetFillStyle(0);
    gPad->SetGrid();

    // - Residual
    bottom_pad.cd();
    TH1D  delta = (*a - *b);
    TH1D* cumu  = reinterpret_cast<TH1D*>(delta.GetCumulative());
    gPad->SetRightMargin(0.05);
    gPad->SetLeftMargin(0.12);
    gPad->SetTopMargin(0.11);
    gPad->SetBottomMargin(0.25);
    double y_min = TMath::Min(cumu->GetMinimum(), delta.GetMinimum());
    double y_max = TMath::Max(cumu->GetMaximum(), delta.GetMaximum());
    if(y_max < TMath::Abs(y_min) * 1e-3) { y_max = TMath::Abs(y_min); }
    auto frame = gPad->DrawFrame(x_min, 1.1 * y_min, x_max, 1.1 * y_max, titles.c_str());
    frame->GetXaxis()->SetTitleOffset(0.8);
    delta.SetLineColor(kRed);
    cumu->SetLineStyle(9);
    cumu->SetLineColorAlpha(kBlue, 0.4);
    auto    l1 = delta.DrawClone("same");
    auto    l2 = cumu->DrawClone("same");
    TLegend lg(0.4, 0.25);
    lg.AddEntry(l1, "residual", "l");
    lg.AddEntry(l2, "cumulative residual", "l");
    lg.SetFillStyle(0);
    lg.SetBorderSize(0);
    lg.Draw();
    gPad->SetGrid();
    m_Ploter.addCanvas(c);
  }

  void   ADC_Low();
  void   ADC_High();
  void   num_hits();
  void   layer();
  void   chip();
  void   memory();
  void   channel();
  void   cellID();
  Ploter m_Ploter;
};
