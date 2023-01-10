#include "CEPCCDRstyle.h"

#include "TGraph.h"
#include "TH1.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TVirtualPad.h"

void SetCEPCCDRStyle()
{
  TStyle* CEPCCDRStyle = new TStyle("CEPCCDRStyle", "Style for CEPC CDR by Kaili");

  // For the canvas:
  CEPCCDRStyle->SetCanvasBorderMode(0);
  CEPCCDRStyle->SetCanvasColor(kWhite);
  CEPCCDRStyle->SetCanvasDefH(800);  //Height of canvas
  CEPCCDRStyle->SetCanvasDefW(800);  //Width of canvas
  CEPCCDRStyle->SetCanvasDefX(0);    //POsition on screen
  CEPCCDRStyle->SetCanvasDefY(0);

  // For the Pad:
  CEPCCDRStyle->SetPadBorderMode(0);
  // CEPCCDRStyle->SetPadBorderSize(Width_t size = 1);
  CEPCCDRStyle->SetPadColor(kWhite);
  CEPCCDRStyle->SetGridStyle(3);
  CEPCCDRStyle->SetGridWidth(1);

  // For the frame:
  CEPCCDRStyle->SetFrameBorderMode(0);
  CEPCCDRStyle->SetFrameBorderSize(1);
  CEPCCDRStyle->SetFrameFillColor(0);
  CEPCCDRStyle->SetFrameFillStyle(0);
  CEPCCDRStyle->SetFrameLineColor(1);
  CEPCCDRStyle->SetFrameLineStyle(1);
  CEPCCDRStyle->SetFrameLineWidth(2);

  // For the histo:
  // CEPCCDRStyle->SetHistFillColor(1);
  // CEPCCDRStyle->SetHistFillStyle(0);
  // CEPCCDRStyle->SetHistLineColor(1);
  CEPCCDRStyle->SetHistLineStyle(0);
  // CEPCCDRStyle->SetHistLineWidth(2);
  // CEPCCDRStyle->SetLegoInnerR(0.8);
  // CEPCCDRStyle->SetNumberContours(Int_t number = 20);

  CEPCCDRStyle->SetEndErrorSize(2);
  //CEPCCDRStyle->SetErrorMarker(20);
  //CEPCCDRStyle->SetErrorX(0.);

  CEPCCDRStyle->SetMarkerStyle(20);

  //For the fit/function:
  CEPCCDRStyle->SetOptFit(0);
  CEPCCDRStyle->SetFitFormat("5.4g");
  CEPCCDRStyle->SetFuncColor(2);
  CEPCCDRStyle->SetFuncStyle(1);
  CEPCCDRStyle->SetFuncWidth(2);

  //For the date:
  CEPCCDRStyle->SetOptDate(0);
  // CEPCCDRStyle->SetDateX(Float_t x = 0.01);
  // CEPCCDRStyle->SetDateY(Float_t y = 0.01);

  // For the statistics box:
  CEPCCDRStyle->SetOptFile(0);
  CEPCCDRStyle->SetOptStat(0);  // To display the mean and RMS:   SetOptStat("mr");
  CEPCCDRStyle->SetStatColor(kWhite);
  CEPCCDRStyle->SetStatTextColor(1);
  CEPCCDRStyle->SetStatFormat("6.4g");
  CEPCCDRStyle->SetStatBorderSize(1);
  CEPCCDRStyle->SetStatH(0.1);
  CEPCCDRStyle->SetStatW(0.15);
  // CEPCCDRStyle->SetStatStyle(Style_t style = 1001);
  // CEPCCDRStyle->SetStatX(Float_t x = 0);
  // CEPCCDRStyle->SetStatY(Float_t y = 0);

  // Margins:
  // canvas->SetMargin(0.16, 0.04, 0.11, 0.02); // left, right, bottom, top
  CEPCCDRStyle->SetPadLeftMargin(0.16);
  CEPCCDRStyle->SetPadRightMargin(0.04);
  CEPCCDRStyle->SetPadBottomMargin(0.11);
  CEPCCDRStyle->SetPadTopMargin(0.02);

  // For the Global title:

  CEPCCDRStyle->SetOptTitle(0);
  CEPCCDRStyle->SetTitleFont(43);
  CEPCCDRStyle->SetTitleColor(1);
  CEPCCDRStyle->SetTitleTextColor(1);
  CEPCCDRStyle->SetTitleFillColor(0);
  CEPCCDRStyle->SetTitleFontSize(36);

  // For the axis titles:

  CEPCCDRStyle->SetTitleColor(1, "XYZ");
  CEPCCDRStyle->SetTitleFont(43, "XYZ");
  CEPCCDRStyle->SetTitleSize(36, "XYZ");

  CEPCCDRStyle->SetTitleXOffset(0.9);
  CEPCCDRStyle->SetTitleYOffset(1.5);
  // CEPCCDRStyle->SetTitleOffset(1.1, "Y"); // Another way to set the Offset

  // For the axis labels:

  CEPCCDRStyle->SetLabelColor(1, "XYZ");
  CEPCCDRStyle->SetLabelFont(43, "XYZ");
  CEPCCDRStyle->SetLabelOffset(0.005, "XYZ");
  CEPCCDRStyle->SetLabelSize(34, "XYZ");

  // For the axis:

  CEPCCDRStyle->SetAxisColor(1, "XYZ");
  CEPCCDRStyle->SetStripDecimals(kTRUE);
  CEPCCDRStyle->SetTickLength(0.02, "XYZ");
  CEPCCDRStyle->SetNdivisions(508, "XYZ");
  CEPCCDRStyle->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
  CEPCCDRStyle->SetPadTickY(1);
  // CEPCCDRStyle->SetPadTickZ(1);

  // Change for log plots:
  CEPCCDRStyle->SetOptLogx(0);
  CEPCCDRStyle->SetOptLogy(0);
  CEPCCDRStyle->SetOptLogz(0);

  // Postscript options:
  CEPCCDRStyle->SetPaperSize(20., 20.);
  // CEPCCDRStyle->SetLineScalePS(Float_t scale = 3);
  // CEPCCDRStyle->SetLineStyleString(Int_t i, const char* text);
  // CEPCCDRStyle->SetHeaderPS(const char* header);
  // CEPCCDRStyle->SetTitlePS(const char* pstitle);

  // CEPCCDRStyle->SetBarOffset(Float_t baroff = 0.5);
  // CEPCCDRStyle->SetBarWidth(Float_t barwidth = 0.5);
  // CEPCCDRStyle->SetPaintTextFormat(const char* format = "g");
  // CEPCCDRStyle->SetPalette(Int_t ncolors = 0, Int_t* colors = 0);
  // CEPCCDRStyle->SetTimeOffset(Double_t toffset);
  // CEPCCDRStyle->SetHistMinimumZero(kTRUE);

  CEPCCDRStyle->SetHatchesLineWidth(5);
  CEPCCDRStyle->SetHatchesSpacing(0.05);

  // CEPCCDRStyle->SetCanvasPreferGL(kTRUE); // ok in Mac or lxslc6, not in local windows/linux.

  CEPCCDRStyle->cd();
}

void FormatLatex(TLatex* ch1)
{
  ch1->SetNDC(kTRUE);
  ch1->SetTextFont(43);
  ch1->SetTextSize(28);
  // ch1->SetTextAlign(31);
  // ch1->DrawLatex(X_position, Y_position, text.c_str());
}

void FormatAxis(TAxis* axis, double offset)
{
  axis->SetNdivisions(508);

  axis->SetLabelFont(43);
  axis->SetLabelSize(34);
  axis->SetLabelOffset(0.005);

  axis->SetTitleFont(43);
  axis->SetTitleColor(1);
  axis->SetTitleSize(36);
  axis->SetTitleOffset(offset);
  //    axis->SetTickLength(0.02);
  axis->CenterTitle();
}

void FormatAxis(TAxis* axis, double offset, TString title)
{
  axis->SetNdivisions(508);

  axis->SetLabelFont(43);
  axis->SetLabelSize(34);
  axis->SetLabelOffset(0.005);
  //    axis->SetLabelOffset(1);

  axis->SetTitleFont(43);
  axis->SetTitleColor(1);
  axis->SetTitleSize(36);
  axis->SetTitleOffset(offset);
  axis->SetTitle(title);
  //    axis->SetTickLength(0.02);
  //    axis->CenterTitle();
}

void FormatLegend(TLegend* legend)
{
  legend->SetBorderSize(0);
  legend->SetTextFont(43);
  legend->SetTextSize(28);
  legend->SetFillStyle(0);
  legend->SetFillColor(0);
  legend->SetLineColor(0);
}

void FormatH1(TH1* H1, int n)
{
  H1->SetTitle("");
  // if(n==1) H1->SetFillColorAlpha(kBlue,0.35);
  H1->SetFillColor(n);

  // if(n==1) H1->SetFillColor(kCyan);
  // if(n==1) H1->SetLineColor(kCyan);
  //         if(n==1) H1->SetFillColor(kBlack);
  // if(n==1) H1->SetLineColor(kBlack);
  if(n == 1) H1->SetFillStyle(1001);
}

void FormatG1(TGraph* g, int n)
{
  int color[7] = {1, 2, 4, 6, 8, 21, 41};
  int style[7] = {20, 21, 22, 23, 24, 25, 26};
  g->SetMarkerSize(1.1);
  g->SetLineWidth(1);
  g->SetMarkerColor(color[n]);
  g->SetLineColor(color[n]);
  g->SetMarkerStyle(style[n]);
}
