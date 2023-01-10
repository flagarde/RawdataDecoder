/*
CEPC CDR style guide
Version: 1.0
Author:  Zhang Kaili
Mail: zhangkl@ihep.ac.cn
Date: 2018.08.12

To use this script, include and use SetCEPCCDRStyle() before you plot. If you use several styles in the same time, you'd better know what you are doing. (Though harmless……)
Also, FormatLatex(), FormatLegend() should be used.

Some general standards for this style:
0. Root 6 recommended. May meet unexpected behavior in root 5.
1. 800*800px, No global title and stats/fit box, no x axis error bar if the bin is uniform;
2. sans-serif font with absolute font size font(43), Axis title size 36, label size 34, caption size 28.
3. Fix canvas box position and use left margin 0.16. So if for Y axis the label is long, change the Y axis title offset, do not edit the margin or label size.
4. If you need transparent color, you may set CEPCCDRStyle->SetCanvasPreferGL(kTRUE); But make sure your root support it. This option is defaultly on for Mac and lxslc6. See more in https://root.cern.ch/doc/master/classTColor.html
5. After use this style, you still manually set the following things to make your plot better:
 .1 Axis title. 						Do not center the title.
	For Y axis in 1d plot, the format should be like Entries or A.U. / XX [GeV].
	For X axis, use small m, m_H or m_Z for a specific particle, big M, for the combination. M_{#mu#mu}^{Recoil}. a spce between M and unit [GeV].
 .2 Legend position and content.		All align left.
	Notice Draw option p,l and f and set the correct name so that root can link them.
	For the order, data points, usually "CEPC simulation" is the first one.
 .3 Caption position and content. 		All align left.
	Use TLatex and tex->DrawLatexNDC(0.8, 0.9, "#bf{CEPC 2018}");
  In CEPC white paper now we use bold #bf{CEPC 2018}. In CEPC CDR we use #bf{CEPC CDR} in the first line.
	Then make clear the situation. #int L and #sqrt{s} is often ommited. So use like CEPC-v4, 5.6 ab^{-1}, or 240 GeV. If the plot is normalized then do not show the integral luminosity.
	Then make clear the process of the plot.

Other tips:
1. Make sure the error bar in your plot is for the total CEPC lumi or just your statstics, esp. for those plots with scale factor.  (RooAbsData::Poisson or RooAbsData::SumW2)
2. Sometimes the first X axis label and Y axis label would overlap. Try frame->GetYaxis()->ChangeLabel(1, -1, 0); //after ROOT 6
3. Use #kern[-0.8] to fine tune some space.
4. Enjoy you plots!
*/

#pragma once

#include "TString.h"
class TLatex;
class TAxis;
class TLegend;
class TH1;
class TGraph;

void SetCEPCCDRStyle();
void FormatLatex(TLatex* ch1);
void FormatAxis(TAxis* axis, double offset);
void FormatAxis(TAxis* axis, double offset, TString title);
void FormatLegend(TLegend* legend);
void FormatH1(TH1* H1, int n);
void FormatG1(TGraph* g, int n);
