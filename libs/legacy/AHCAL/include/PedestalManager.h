#ifndef PEDESTALMANAGER_HH
#define PEDESTALMANAGER_HH

#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"

#include <TH2D.h>
#include <fstream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class PedestalManager
{
public:
  TFile* fin;
  TTree* tin;
  TFile* fout;
  TTree* tout;

  Double_t                  cycleID;
  Double_t                  triggerID;
  vector<int>*              cellIDs;
  vector<int>*              BCIDs;
  vector<int>*              hitTags;
  vector<int>*              gainTags;
  vector<double>*           charges;
  vector<double>*           times;
  vector<int>               vec_cellid;
  std::unique_ptr<TH2D>     htimepeak;
  std::unique_ptr<TH2D>     htimerms;
  std::unique_ptr<TH2D>     hchargepeak;
  std::unique_ptr<TH2D>     hchargerms;
  unordered_map<int, TH2D*> map_layer_timepeak;
  unordered_map<int, TH2D*> map_layer_timerms;
  unordered_map<int, TH2D*> map_layer_chargepeak;
  unordered_map<int, TH2D*> map_layer_chargerms;
  unordered_map<int, TH1D*> map_cellid_htime;
  unordered_map<int, TH1D*> map_cellid_hcharge;
  //map<int,int> map_cellid_exist;
  double                    time_min = 1000., time_max = 0.;
  double                    charge_min = 1000., charge_max = 0.;
  double                    time_peak = 0., time_rms = 0., charge_peak = 0., charge_rms = 0.;
  int                       _cellid;
  string                    input_list;

  PedestalManager(const TString& outname);
  virtual ~PedestalManager();
  virtual void Initialization();
  virtual int  AnaPedestal(const std::string& list, const int& sel_hittag);
  virtual void ReadTree(const TString& fname);
  virtual void SaveCanvas(TH2D* h, const TString& name);
};

#endif
