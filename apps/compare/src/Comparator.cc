#include "Comparator.h"

#include "TCanvas.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TPad.h"
#include "TStyle.h"

Comparator::Comparator(const std::string& old, const std::string& nouveau)
{
  ROOT::EnableImplicitMT();
  m_oldData = std::make_unique<ROOT::RDataFrame>(m_TreeName, old);
  m_newData = std::make_unique<ROOT::RDataFrame>(m_TreeName, nouveau);
}

void Comparator::Run()
{
  en_spectrum("H");
  en_spectrum("L");
  num_hits();
  layer();
}

void Comparator::SaveAs(const std::string& file) { m_Ploter.save(file.c_str()); }

void Comparator::en_spectrum(std::string hl_gain = "H")
{
  double      x_min = 0;
  double      x_max = 4096;
  int         nbins = x_max - x_min;
  std::string branch_name_a;
  std::string branch_name_b;

  if(hl_gain == "H" or hl_gain == "h")
  {
    branch_name_a = BR_HIGH_GAIN_A;
    branch_name_b = BR_HIGH_GAIN_A;
  }
  else
  {
    branch_name_a = BR_LOW_GAIN_A;
    branch_name_b = BR_LOW_GAIN_A;
  }

  auto a = m_oldData->Define("tmp", branch_name_a + "[" + BR_TAG_A + " == 1]").Histo1D({"Legacy", ";ADC;Count", nbins, x_min, x_max}, "tmp");
  auto b = m_newData->Define("tmp", branch_name_b + "[" + BR_TAG_A + " == 1]").Histo1D({"New", ";ADC;Count", nbins, x_min, x_max}, "tmp");

  a->SetLineWidth(2);
  b->SetLineWidth(2);

  a->SetLineColor(kRed);
  b->SetLineColor(kBlue);

  b->SetLineStyle(7);

  /********  Draw  ********/
  gStyle->SetOptStat(0);
  TCanvas c;
  c.SetWindowSize(800, 800);
  TPad top_pad("top_pad", "", 0, 0.4, 1, 1);
  TPad bottom_pad("bottom_pad", "", 0, 0, 1, 0.4);
  top_pad.Draw();
  bottom_pad.Draw();
  // - Spectrum
  top_pad.cd();
  gPad->SetTopMargin(0.12);
  gPad->SetBottomMargin(0.03);
  a->GetXaxis()->SetLabelOffset(99);
  a->GetXaxis()->SetTitleOffset(99);
  a->SetMaximum(TMath::Max(a->GetMaximum(), b->GetMaximum()));
  a->Draw("");
  b->Draw("same");
  gPad->SetLogy();
  TLegend* top_lg = gPad->BuildLegend(0.2, 0.15, 0.2, 0.15);
  top_lg->SetBorderSize(0);
  top_lg->SetFillStyle(0);
  gPad->SetGrid();

  // - Residual
  bottom_pad.cd();
  TH1D  delta = (*a - *b);
  TH1D* cumu  = (TH1D*)delta.GetCumulative();
  gPad->SetTopMargin(0.13);
  gPad->SetBottomMargin(0.22);
  double y_min = TMath::Min(cumu->GetMinimum(), delta.GetMinimum());
  double y_max = TMath::Max(cumu->GetMaximum(), delta.GetMaximum());
  if(y_max < TMath::Abs(y_min) * 1e-3) { y_max = TMath::Abs(y_min); }
  auto frame = gPad->DrawFrame(x_min, 1.2 * y_min, x_max, 1.2 * y_max, ";ADC; #Delta");
  frame->GetXaxis()->SetTitleOffset(1.8);
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

void Comparator::num_hits()
{
  double x_min = 0;
  double x_max = 200;
  int    nbins = x_max - x_min;

  auto a = m_oldData->Define("tmp", "Sum(" + BR_TAG_A + " == 1)").Histo1D({"Legacy", ";Num. of Hits;Count", nbins, x_min, x_max}, "tmp");
  auto b = m_newData->Define("tmp", "Sum(" + BR_TAG_A + " == 1)").Histo1D({"New", ";Num. of Hits;Count", nbins, x_min, x_max}, "tmp");

  a->SetMarkerStyle(20);
  b->SetMarkerStyle(5);

  a->SetMarkerColor(kRed);
  b->SetMarkerColor(kBlue);

  a->SetLineWidth(2);
  b->SetLineWidth(2);

  a->SetLineColor(kRed);
  b->SetLineColor(kBlue);

  //b->SetLineStyle(7);

  /********  Draw  ********/
  gStyle->SetOptStat(0);
  TCanvas c;
  c.SetWindowSize(800, 800);
  TPad top_pad("top_pad", "", 0, 0.4, 1, 1);
  TPad bottom_pad("bottom_pad", "", 0, 0, 1, 0.4);
  top_pad.Draw();
  bottom_pad.Draw();
  // - Spectrum
  top_pad.cd();
  gPad->SetTopMargin(0.12);
  gPad->SetBottomMargin(0.03);
  a->GetXaxis()->SetLabelOffset(99);
  a->GetXaxis()->SetTitleOffset(99);
  a->SetMaximum(TMath::Max(a->GetMaximum(), b->GetMaximum()));
  a->Draw("E0");
  b->Draw("E0 same");
  gPad->SetLogy();
  TLegend* top_lg = gPad->BuildLegend(0.2, 0.15, 0.2, 0.15, "", "pe");
  top_lg->SetBorderSize(0);
  top_lg->SetFillStyle(0);
  gPad->SetGrid();

  // - Residual
  bottom_pad.cd();
  TH1D  delta = (*a - *b);
  TH1D* cumu  = (TH1D*)delta.GetCumulative();
  gPad->SetTopMargin(0.0);
  gPad->SetBottomMargin(0.22);
  double y_min = TMath::Min(delta.GetMinimum(), cumu->GetMinimum());
  double y_max = TMath::Max(delta.GetMaximum(), cumu->GetMaximum());
  auto   frame = gPad->DrawFrame(x_min, 1.2 * y_min, x_max, 1.2 * y_max, ";Num. of Hits; #Delta");
  frame->GetXaxis()->SetTitleOffset(1.8);
  delta.SetLineColor(kRed);
  auto l1 = delta.DrawClone("same");
  cumu->SetLineStyle(9);
  cumu->SetLineColorAlpha(kBlue, 0.4);
  auto    l2 = cumu->DrawClone("same");
  TLegend lg(0.4, 0.25, 0.4, 0.25);
  lg.AddEntry(l1, "residual", "l");
  lg.AddEntry(l2, "cumulative residual", "l");
  lg.SetFillStyle(0);
  lg.SetBorderSize(0);
  lg.Draw();
  gPad->SetGrid();

  m_Ploter.addCanvas(c);
}

void Comparator::layer()
{
  double x_min = 0;
  double x_max = 42;
  int    nbins = x_max - x_min;

  auto a = m_oldData->Define("tmp", "ROOT::RVec<int>(" + BR_CELL_ID_A + "[" + BR_TAG_A + " == 1]" + "/1e5)").Histo1D({"Legacy", ";Layer Index;Count", nbins, x_min, x_max}, "tmp");
  auto b = m_newData->Define("tmp", "ROOT::RVec<int>(" + BR_CELL_ID_A + "[" + BR_TAG_A + " == 1]" + "/1e5)").Histo1D({"New", ";Layer Index;Count", nbins, x_min, x_max}, "tmp");

  a->SetMarkerStyle(20);
  b->SetMarkerStyle(5);

  a->SetMarkerColor(kRed);
  b->SetMarkerColor(kBlue);

  a->SetLineWidth(2);
  b->SetLineWidth(2);

  a->SetLineColor(kRed);
  b->SetLineColor(kBlue);

  //b->SetLineStyle(7);

  /********  Draw  ********/
  gStyle->SetOptStat(0);
  TCanvas c;
  c.SetWindowSize(800, 800);
  TPad top_pad("top_pad", "", 0, 0.4, 1, 1);
  TPad bottom_pad("bottom_pad", "", 0, 0, 1, 0.4);
  top_pad.Draw();
  bottom_pad.Draw();
  // - Spectrum
  top_pad.cd();
  gPad->SetTopMargin(0.12);
  gPad->SetBottomMargin(0.03);
  a->GetXaxis()->SetLabelOffset(99);
  a->GetXaxis()->SetTitleOffset(99);
  a->SetMaximum(TMath::Max(a->GetMaximum(), b->GetMaximum()));
  a->Draw("E0");
  b->Draw("E0 same");
  //gPad->SetLogy();
  TLegend* top_lg = gPad->BuildLegend(0.2, 0.15, 0.2, 0.15, "", "pe");
  top_lg->SetBorderSize(0);
  top_lg->SetFillStyle(0);
  gPad->SetGrid();

  // - Residual
  bottom_pad.cd();
  TH1D  delta = (*a - *b);
  TH1D* cumu  = (TH1D*)delta.GetCumulative();
  gPad->SetTopMargin(0.13);
  gPad->SetBottomMargin(0.22);
  double y_min = TMath::Min(delta.GetMinimum(), cumu->GetMinimum());
  double y_max = TMath::Max(delta.GetMaximum(), cumu->GetMaximum());
  auto   frame = gPad->DrawFrame(x_min, 1.2 * y_min, x_max, 1.2 * y_max, ";Layer Index; #Delta");
  frame->GetXaxis()->SetTitleOffset(1.8);
  delta.SetLineColor(kRed);
  auto l1 = delta.DrawClone("same");
  cumu->SetLineStyle(9);
  cumu->SetLineColorAlpha(kBlue, 0.4);
  auto l2 = cumu->DrawClone("same");
  gPad->RedrawAxis();
  TLegend lg(0.4, 0.25, 0.4, 0.25);
  lg.AddEntry(l1, "residual", "l");
  lg.AddEntry(l2, "cumulative residual", "l");
  lg.SetFillStyle(0);
  lg.SetBorderSize(0);
  lg.Draw();
  gPad->SetGrid();

  m_Ploter.addCanvas(c);
}
