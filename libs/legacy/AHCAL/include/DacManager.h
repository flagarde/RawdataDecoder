#ifndef DACMANAGER_HH
#define DACMANAGER_HH

#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TTree.h"

#include <TH2D.h>
#include <algorithm>
#include <fstream>
#include <map>
#include <string>
#include <vector>

using namespace std;

class DacManager
{
public:
  TFile* fin;
  //std::shared_ptr<TFile> fin;
  TTree* tin;
  TTree* tout;
  TFile* fout;

  Double_t        cycleID;
  Double_t        triggerID;
  vector<int>*    cellIDs;
  vector<int>*    BCIDs;
  vector<int>*    hitTags;
  vector<int>*    gainTags;
  vector<double>* charges;
  vector<double>* times;
  vector<int>     vec_cellid;
  map<int, TH2D*> map_cellid_calib;
  map<int, TH2D*> map_layer_dacslope;
  map<int, TH2D*> map_layer_fit;
  map<int, TH2D*> map_layer_highgainplatform;
  map<int, int>   map_cellid_exist;
  TH2D*           hdacslope;
  TH2D*           hfit;
  TH2D*           hhighgain_platform;
  TH2D*           hped_high;
  TH2D*           hped_low;
  double          time_min = 1000., time_max = 0.;
  double          charge_min = 1000., charge_max = 0.;
  double          _slope = 0.;
  int             _cellid;
  string          input_list;
  TF1*            f1;
  TF1*            f2;

  DacManager(const TString& outname);
  virtual ~DacManager();
  virtual int  AnaDac(const std::string& list, const TString& mode);
  virtual void SetPedestal(const TString& pedname);
  virtual void ReadTree(TString fname);
  virtual void SaveCanvas(TH2D* h, TString name);
};

#endif
