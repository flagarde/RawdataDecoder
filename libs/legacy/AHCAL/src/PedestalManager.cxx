#include "PedestalManager.h"

#include "TStyle.h"

#include <TCanvas.h>
#include <TH2.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>

using namespace std;

PedestalManager::PedestalManager(const TString& outname)
{
  //fout = std::make_unique<TFile>(TString(outname),"RECREATE");
  //tout = std::make_unique<TTree>("pedestal","DAC Pedestal");
  fout     = new TFile(TString(outname), "RECREATE");
  tout     = new TTree("pedestal", "DAC Pedestal");
  cellIDs  = 0;
  BCIDs    = 0;
  hitTags  = 0;
  gainTags = 0;
  charges  = 0;
  times    = 0;
  tout->Branch("cellid", &_cellid);
  tout->Branch("time_peak", &time_peak);
  tout->Branch("time_rms", &time_rms);
  tout->Branch("charge_peak", &charge_peak);
  tout->Branch("charge_rms", &charge_rms);
  htimepeak   = std::make_unique<TH2D>("htimepeak", "HighGain Peak", 360, 0, 360, 36, 0, 36);
  htimerms    = std::make_unique<TH2D>("htimerms", "HighGain RMS", 360, 0, 360, 36, 0, 36);
  hchargepeak = std::make_unique<TH2D>("hchargepeak", "LowGain Peak", 360, 0, 360, 36, 0, 36);
  hchargerms  = std::make_unique<TH2D>("hchargerms", "LowGain RMS", 360, 0, 360, 36, 0, 36);
  cout << "Initialization done" << endl;
}

void PedestalManager::Initialization()
{
  int ini_cellid = 0;
  for(int i_layer = 0; i_layer < 40; i_layer++)
  {
    for(int i_chip = 0; i_chip < 9; i_chip++)
    {
      for(int i_chn = 0; i_chn < 36; i_chn++)
      {
        ini_cellid = i_layer * 100000 + i_chip * 10000 + i_chn;
        vec_cellid.push_back(ini_cellid);
        TString timename               = "htime_" + TString(to_string(ini_cellid).c_str());
        map_cellid_htime[ini_cellid]   = new TH1D(timename, timename, 1500, 0, 1500);
        TString chargename             = "hcharge_" + TString(to_string(ini_cellid).c_str());
        map_cellid_hcharge[ini_cellid] = new TH1D(chargename, chargename, 1600, 0, 1600);
      }
    }
  }
}

int PedestalManager::AnaPedestal(const std::string& list, const int& sel_hittag)
{
  cout << "Starting Ana" << endl;
  for(int i = 0; i < 40; i++)
  {
    TString name_timepeak   = "htimepeak_" + TString(to_string(i).c_str());
    map_layer_timepeak[i]   = new TH2D(name_timepeak, name_timepeak, 9, 0, 9, 36, 0, 36);
    TString name_timerms    = "htimerms_" + TString(to_string(i).c_str());
    map_layer_timerms[i]    = new TH2D(name_timerms, name_timerms, 9, 0, 9, 36, 0, 36);
    TString name_chargepeak = "hchargepeak_" + TString(to_string(i).c_str());
    map_layer_chargepeak[i] = new TH2D(name_chargepeak, name_chargepeak, 9, 0, 9, 36, 0, 36);
    TString name_chargerms  = "hchargerms_" + TString(to_string(i).c_str());
    map_layer_chargerms[i]  = new TH2D(name_chargerms, name_chargerms, 9, 0, 9, 36, 0, 36);
  }
  cout << "Ana preparation done" << endl;
  input_list = list;
  ifstream data(input_list);
  while(!data.eof())
  {
    string tmp;
    data >> tmp;
    if(tmp == "") continue;
    string skipchannel = tmp;
    int    dac_chn     = -1;  // Which channel should not be used here for pedestal analysis
    if(sel_hittag == 1)
    {
      skipchannel = skipchannel.substr(skipchannel.find_last_of('/') + 1);
      skipchannel = skipchannel.substr(skipchannel.find("chn") + 3);
      skipchannel = skipchannel.substr(0, skipchannel.find_last_of('_'));
      dac_chn     = stoi(skipchannel);
    }
    this->ReadTree(TString(tmp.c_str()));
    int Nentry = tin->GetEntries();
    for(int ientry = 0; ientry < Nentry; ientry++)
    {
      //if(ientry<5)continue; // Skip the first 5 events from Hao Liu
      //if(ientry%1000==0)cout<<ientry<<" / "<<Nentry<<endl;
      tin->GetEntry(ientry);
      for(int i = 0; i < hitTags->size(); i++)
      {
        if(hitTags->at(i) != sel_hittag) continue;
        int cellid  = cellIDs->at(i);
        int channel = cellid % 100;
        int memo    = (cellid % 10000) / 100;
        if(memo != 0) continue;
        if(dac_chn == channel) continue;
        //if(map_cellid_exist[cellid] != 1)
        //{
        //	vec_cellid.push_back(cellid);
        //	map_cellid_exist[cellid] =1;
        //}
        int layer = cellid / 1e5;
        if(times->at(i) < time_min) time_min = times->at(i);
        if(times->at(i) > time_max) time_max = times->at(i);
        if(charges->at(i) < charge_min) charge_min = charges->at(i);
        if(charges->at(i) > charge_max) charge_max = charges->at(i);
        //if(!map_cellid_htime[cellid])
        //{
        //	TString tmp_name="htime_"+TString(to_string(cellid).c_str());
        //	map_cellid_htime[cellid]=new TH1D(tmp_name,tmp_name,1500,0,1500);
        //	//map_cellid_htime[cellid]=new TH1D(tmp_name,tmp_name,200,300,500);
        //}
        //if(!map_cellid_hcharge[cellid])
        //{
        //	TString tmp_name="hcharge_"+TString(to_string(cellid).c_str());
        //	map_cellid_hcharge[cellid]=new TH1D(tmp_name,tmp_name,1600,0,1600);
        //	//map_cellid_hcharge[cellid]=new TH1D(tmp_name,tmp_name,200,300,500);
        //}
        map_cellid_htime[cellid]->Fill(times->at(i));
        map_cellid_hcharge[cellid]->Fill(charges->at(i));
      }
    }
    //fin->Close();
    cout << "Ana for file: " << tmp << " done!" << endl;
  }

  for_each(vec_cellid.begin(), vec_cellid.end(),
           [this](int i) -> void
           {
             _cellid     = i;
             time_peak   = map_cellid_htime[i]->GetBinCenter(map_cellid_htime[i]->GetMaximumBin());
             time_rms    = map_cellid_htime[i]->GetRMS();
             charge_peak = map_cellid_hcharge[i]->GetBinCenter(map_cellid_hcharge[i]->GetMaximumBin());
             charge_rms  = map_cellid_hcharge[i]->GetRMS();
             tout->Fill();
           });
  cout << "Out Tree Filled" << endl;
  fout->cd();
  tout->Write();
  cout << "time min: " << time_min << " max: " << time_max << endl;
  cout << "charge min: " << charge_min << " max: " << charge_max << endl;

  // Save hists into the output file:
  // mode_name = times or charges
  // tmp_map is the map from cellid to TH1D to loop over
  // tmp_layer_timepeak and tmp_layer_timerms is the map from layer to peak and rms
  // hpeak and hrms are the general TH2D for peak and rms
  // alias is the alternative name. high or low
  auto f_save = [this](TString mode_name, unordered_map<int, TH1D*> tmp_map, unordered_map<int, TH2D*> tmp_layer_timepeak, unordered_map<int, TH2D*> tmp_layer_timerms, std::unique_ptr<TH2D>& hpeak, std::unique_ptr<TH2D>& hrms, TString alias)
  {
    fout->mkdir(TString(mode_name));
    fout->cd(TString(mode_name));
    for(int i = 0; i < 40; i++) gDirectory->mkdir(TString("layer_") + TString(to_string(i).c_str()));
    for(auto i: tmp_map)
    {
      int cellid  = i.first;
      int layer   = cellid / 1e5;
      int channel = cellid % 100;
      int chip    = (cellid % 100000) / 10000;
      tmp_layer_timepeak[layer]->Fill(chip, channel, i.second->GetBinCenter(i.second->GetMaximumBin()));
      tmp_layer_timerms[layer]->Fill(chip, channel, i.second->GetRMS());
      hpeak->Fill(layer * 9 + chip, channel, i.second->GetBinCenter(i.second->GetMaximumBin()));
      hrms->Fill(layer * 9 + chip, channel, i.second->GetRMS());
      TString dir_name = TString(mode_name + "/layer_") + TString(to_string(layer).c_str());
      fout->cd(dir_name);
      i.second->Write();
    }
    for(int i = 0; i < 40; i++)
    {
      TString dir_name = TString(mode_name + "/layer_") + TString(to_string(i).c_str());
      fout->cd(dir_name);
      tmp_layer_timepeak[i]->Write();
      tmp_layer_timerms[i]->Write();
      this->SaveCanvas(tmp_layer_timepeak[i], alias + TString("gain_peak_") + to_string(i).c_str());
      this->SaveCanvas(tmp_layer_timerms[i], alias + TString("gain_rms_") + to_string(i).c_str());
    }
  };
  f_save("times", map_cellid_htime, map_layer_timepeak, map_layer_timerms, htimepeak, htimerms, "high");
  f_save("charges", map_cellid_hcharge, map_layer_chargepeak, map_layer_chargerms, hchargepeak, hchargerms, "low");
  fout->cd("");
  htimepeak->Write();
  htimerms->Write();
  hchargepeak->Write();
  hchargerms->Write();
  cout << "2D hists written" << endl;
  return 0;
}

void PedestalManager::ReadTree(const TString& fname)
{
  cout << "Reading tree " << fname << endl;
  fin = TFile::Open(TString(fname), "READ");
  tin = (TTree*)fin->Get("Cosmic_Event");
  tin->SetBranchAddress("cycleID", &cycleID);
  tin->SetBranchAddress("triggerID", &triggerID);
  tin->SetBranchAddress("cellIDs", &cellIDs);
  tin->SetBranchAddress("BCIDs", &BCIDs);
  tin->SetBranchAddress("hitTags", &hitTags);
  tin->SetBranchAddress("gainTags", &gainTags);
  tin->SetBranchAddress("charges", &charges);
  tin->SetBranchAddress("times", &times);
  cout << "Read tree done!" << fname << endl;
}
void PedestalManager::SaveCanvas(TH2D* h, const TString& name)
{
  gStyle->SetPaintTextFormat("4.1f");
  std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>(TString(name), TString(name), 1024, 768);
  c1->cd();
  gStyle->SetOptStat("");
  h->Draw("colztext");
  TString outname = name + ".png";
  c1->SaveAs(TString(outname));
}

PedestalManager::~PedestalManager() {}
