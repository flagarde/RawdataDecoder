#include "DacManager.h"

#include "TStyle.h"

#include <TCanvas.h>
#include <TH2.h>
#include <iostream>
#include <sstream>

using namespace std;

DacManager::DacManager(const TString& outname)
{
  fout     = new TFile(TString(outname), "RECREATE");
  tout     = new TTree("dac", "Dac");
  cellIDs  = 0;
  BCIDs    = 0;
  hitTags  = 0;
  gainTags = 0;
  charges  = 0;
  times    = 0;
  tout->Branch("cellid", &_cellid);
  tout->Branch("slope", &_slope);
  hdacslope          = new TH2D("hdacslope", "HighGain/LowGain", 360, 0, 360, 36, 0, 36);
  hfit               = new TH2D("hfit", "Fitting Goodness", 360, 0, 360, 36, 0, 36);
  hhighgain_platform = new TH2D("hhighgain_platform", "High Gain Platform", 360, 0, 360, 36, 0, 36);
  f1                 = new TF1("f1", "[0]*x+[1]");  // Function used to fit the slope (High_gain / Low_gain)
  f2                 = new TF1("f2", "[0]");        // Function used to fetch the high gain platform
                                                    //cout<<"Initialization done"<<endl;
}

void DacManager::SetPedestal(const TString& pedname)
{
  TFile* ftmp      = TFile::Open(TString(pedname));
  TH2D*  htmp_high = (TH2D*)ftmp->Get("htimepeak");
  TH2D*  htmp_low  = (TH2D*)ftmp->Get("hchargepeak");
  hped_high        = (TH2D*)htmp_high->Clone("hped_high");
  hped_low         = (TH2D*)htmp_low->Clone("hped_low");
  //ftmp->Close();
}
int DacManager::AnaDac(const std::string& list, const TString& mode)
{
  //Initialization
  for(int i = 0; i < 40; i++)
  {
    TString name_dacslope         = "hdacslope_" + TString(to_string(i).c_str());
    TString name_fit              = "hfit_" + TString(to_string(i).c_str());
    TString name_highgainplatform = "hhighgainplatform_" + TString(to_string(i).c_str());
    map_layer_dacslope[i]         = new TH2D(name_dacslope, name_dacslope, 9, 0, 9, 36, 0, 36);
    map_layer_fit[i]              = new TH2D(name_fit, name_fit, 9, 0, 9, 36, 0, 36);
    map_layer_highgainplatform[i] = new TH2D(name_highgainplatform, name_highgainplatform, 9, 0, 9, 36, 0, 36);
  }
  for(int l = 0; l < 40; l++)
  {
    for(int c = 0; c < 9; c++)
    {
      for(int chn = 0; chn < 36; chn++)
      {
        int     tmp_cellid = l * 1e5 + c * 1e4 + chn;
        TString tmp_name   = "hdac_" + TString(to_string(tmp_cellid).c_str());
        if(mode == "dac") map_cellid_calib[tmp_cellid] = new TH2D(tmp_name, tmp_name, 200, -200, 300, 200, -100, 3400);  // input high gain
        else if(mode == "cosmic")
          map_cellid_calib[tmp_cellid] = new TH2D(tmp_name, tmp_name, 200, -100, 3000, 200, -200, 3200);  // input high gain
      }
    }
  }
  //cout<<"Ana preparation done"<<endl;
  input_list = list;
  ifstream data(input_list);
  while(!data.eof())
  {
    string tmp;
    data >> tmp;
    if(tmp == "") continue;
    cout << tmp << endl;
    int    int_input_dac = -1;
    int    sel_channel   = -1;
    string input_dac     = tmp;
    string skipchannel   = tmp;
    if(mode == "dac")
    {
      skipchannel   = skipchannel.substr(skipchannel.find_last_of('/') + 1);
      skipchannel   = skipchannel.substr(skipchannel.find("chn") + 3);
      skipchannel   = skipchannel.substr(0, skipchannel.find_last_of('_'));
      sel_channel   = stoi(skipchannel);
      input_dac     = input_dac.substr(input_dac.find_last_of('/') + 1);
      input_dac     = input_dac.substr(input_dac.find("dac") + 3);
      input_dac     = input_dac.substr(0, input_dac.find_last_of('.'));
      int_input_dac = stoi(input_dac);
    }
    //cout<<skipchn<<endl;
    this->ReadTree(TString(tmp.c_str()));
    int Nentry = tin->GetEntries();
    for(int ientry = 0; ientry < Nentry; ientry++)
    {
      if(ientry < 5) continue;  // Skip the first 5 events from Hao Liu
      tin->GetEntry(ientry);
      for(int i = 0; i < hitTags->size(); i++)
      {
        if(mode == "dac" && hitTags->at(i) != 1) continue;     // For DAC we set =1 , for cosmic rays we skip 0
        if(mode == "cosmic" && hitTags->at(i) == 0) continue;  // For DAC we set =1 for cosmic rays we skip 0
        int cellid  = cellIDs->at(i);
        int channel = cellid % 100;
        int memo    = (cellid % 10000) / 100;
        if(memo != 0) continue;
        if(mode == "dac" && channel != sel_channel) continue;  // if channel number != dac channel, skip it!
        if(map_cellid_exist[cellid] == 0)
        {
          vec_cellid.push_back(cellid);
          map_cellid_exist[cellid] = 1;
        }
        int    layer        = cellid / 1e5;
        int    chip         = (cellid % 100000) / 10000;
        double tmp_lowgain  = charges->at(i) - hped_low->GetBinContent(layer * 9 + chip + 1, channel + 1);
        double tmp_highgain = times->at(i) - hped_high->GetBinContent(layer * 9 + chip + 1, channel + 1);
        if(tmp_highgain < time_min) time_min = tmp_highgain;
        if(tmp_highgain > time_max) time_max = tmp_highgain;
        if(tmp_lowgain < charge_min) charge_min = tmp_lowgain;
        if(tmp_lowgain > charge_max) charge_max = tmp_lowgain;
        //if(!map_cellid_calib[cellid])
        //{
        //	TString tmp_name="hdac_"+TString(to_string(cellid));
        //	//if(mode=="dac")map_cellid_calib[cellid]=new TH2D(tmp_name,tmp_name,200,-200,300,200,-100,3400); // input high gain
        //	//else if(mode=="cosmic")map_cellid_calib[cellid]=new TH2D(tmp_name,tmp_name,200,-100,3000,200,-200,3200); // input high gain
        //}
        map_cellid_calib[cellid]->Fill(tmp_lowgain, tmp_highgain);  // Fill low gain high gain with pedestal subtracted
      }
    }
    fin->Close();
    delete fin;
  }

  cout << "time min: " << time_min << " max: " << time_max << endl;
  cout << "charge min: " << charge_min << " max: " << charge_max << endl;
  fout->mkdir("calib");
  fout->cd("calib");
  for(int i = 0; i < 40; i++) gDirectory->mkdir(TString("layer_") + TString(to_string(i).c_str()));
  //for(auto i:map_cellid_calib)
  for_each(map_cellid_calib.begin(), map_cellid_calib.end(),
           [this, mode](pair<int, TH2D*> i)
           {
             int    cellid              = i.first;
             int    layer               = cellid / 1e5;
             int    channel             = cellid % 100;
             int    chip                = (cellid % 100000) / 10000;
             double fit_goodness        = 10000.;  // Initialize a large fit value
             bool   fit_goodvalue_found = false, found_start = false;
             double fitstart = -100., fitend = 3000.;
             double highgain_platform = 10000.;
             double slope             = -10.;  // Slope after fitting
             if(mode == "dac")
             {
               fitstart = -200.;
               fitend   = 300.;
             }
             i.second->Fit("f1", "q", "", fitstart, fitend);
             double fg0 = f1->GetChisquare() / f1->GetNDF();  //Fit goodness at largest range
             for(int xmax = fitend; xmax >= 0; xmax -= 50)    // TODO
             {
               i.second->Fit("f1", "q", "", fitstart, xmax);
               fit_goodness = f1->GetChisquare() / f1->GetNDF();
               slope        = f1->GetParameter(0);  // Slope after fitting
               //cout<<fitstart<<" "<<xmax<<" "<<fit_goodness<<endl;
               if((fit_goodness < 400. || fit_goodness < (0.5 * fg0)) && slope > 10. && slope < 50.)
               {
                 fit_goodvalue_found = true;
                 fitend              = xmax;
                 break;
               }
             }
             if(fit_goodvalue_found) { cout << "fit good value found: " << cellid << " " << fitstart << " " << fitend << " " << fit_goodness << " " << fg0 << endl; }
             else
             {
               i.second->Fit("f1", "q", "", fitstart, fitend);
               fit_goodness = f1->GetChisquare() / f1->GetNDF();
             }
             // Find the maximum to be the platform
             bool found_max = false;
             for(int jbin = i.second->GetNbinsY(); jbin > 0; jbin--)
             {
               for(int ibin = 0; ibin < i.second->GetNbinsX(); ibin++)
               {
                 if(i.second->GetBinContent(ibin, jbin) > 0.1)
                 {
                   found_max = true;
                   i.second->Fit("f2", "q+", "", i.second->GetXaxis()->GetBinCenter(ibin) - 10, i.second->GetXaxis()->GetBinCenter(ibin) + 10);
                   highgain_platform = f2->GetParameter(0);
                 }
                 if(found_max) break;
               }
               if(found_max) break;
             }
             slope = f1->GetParameter(0);  // Slope after fitting
             //
             //2D for each layer following
             map_layer_dacslope[layer]->Fill(chip, channel, slope);
             map_layer_fit[layer]->Fill(chip, channel, fit_goodness);
             map_layer_highgainplatform[layer]->Fill(chip, channel, highgain_platform);  // High gain Platform map

             // 2D for all layers following
             hdacslope->Fill(layer * 9 + chip, channel, slope);
             hfit->Fill(layer * 9 + chip, channel, fit_goodness);
             hhighgain_platform->Fill(layer * 9 + chip, channel, highgain_platform);

             //Save histograms
             TString dir_name = TString("calib/layer_") + TString(to_string(layer).c_str());
             fout->cd(dir_name);
             i.second->Write();

             //Fill tree
             _cellid = cellid;
             _slope  = slope;
             tout->Fill();
           });
  fout->cd();
  tout->Write();
  cout << "Out Tree Saved" << endl;
  for(int i = 0; i < 40; i++)
  {
    TString dir_name = TString("calib/layer_") + TString(to_string(i).c_str());
    fout->cd(dir_name);
    map_layer_dacslope[i]->Write();
    map_layer_fit[i]->Write();
    map_layer_highgainplatform[i]->Write();
    this->SaveCanvas(map_layer_dacslope[i], (string("dacslope_") + to_string(i)).c_str());
    //this->SaveCanvas(map_layer_fit[i],string("fit_")+to_string(i));
    //this->SaveCanvas(map_layer_highgainplatform[i],string("highgainplatform_")+to_string(i));
  }
  fout->cd("");
  hdacslope->Write();
  hfit->Write();
  hhighgain_platform->Write();
  fout->Close();
  return 0;
}

void DacManager::ReadTree(TString fname)
{
  //cout<<"Reading tree "<<fname<<endl;
  fin = TFile::Open(TString(fname), "READ");
  //fin = std::make_shared<TFile>(TString(fname),"READ");
  tin = (TTree*)fin->Get("Cosmic_Event");
  tin->SetBranchAddress("cycleID", &cycleID);
  tin->SetBranchAddress("triggerID", &triggerID);
  tin->SetBranchAddress("cellIDs", &cellIDs);
  tin->SetBranchAddress("BCIDs", &BCIDs);
  tin->SetBranchAddress("hitTags", &hitTags);
  tin->SetBranchAddress("gainTags", &gainTags);
  tin->SetBranchAddress("charges", &charges);
  tin->SetBranchAddress("times", &times);
  //cout<<"Read tree done!"<<fname<<endl;
}
void DacManager::SaveCanvas(TH2D* h, TString name)
{
  gStyle->SetPaintTextFormat("4.1f");
  TCanvas* c1 = new TCanvas(TString(name), TString(name), 1024, 768);
  c1->cd();
  gStyle->SetOptStat("");
  h->Draw("colztext");
  TString outname = name + ".png";
  c1->SaveAs(TString(outname));
  delete c1;
}

DacManager::~DacManager() {}
