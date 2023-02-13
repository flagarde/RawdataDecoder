#ifndef DATMANAGER_h
#define DATMANAGER_h

#include <TFile.h>
#include <TMath.h>
#include <TTree.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

class DatManager
{
public:
  static const int channel_FEE = 73;  //(36charges+36times + BCIDs )*16column+ ChipID
  static const int Layer_No    = 40;
  static const int chip_No     = 9;
  static const int channel_No  = 36;
  string           outname     = "";
  int              _Run_No;
  int              _cycleID;
  int              _triggerID;
  unsigned int     _Event_Time;
  vector<int>      _buffer_v;
  vector<int>      _EventBuffer_v;
  vector<int>      _chip_v[Layer_No][chip_No];
  vector<int>      _cellID;
  vector<int>      _bcid;
  vector<int>      _hitTag;
  vector<int>      _gainTag;
  vector<int>      _cherenkov;
  vector<double>   _HG_Charge;
  vector<double>   _LG_Charge;
  vector<double>   _Hit_Time;
  int              count_chipbuffer = 0;

  DatManager(){};
  virtual ~DatManager();
  int  Decode(const string& binary_name, const string& raw_name, const bool b_auto_gain = 0, const bool b_cherenkov = 0);
  int  CatchEventBag(ifstream& f_in, vector<int>& buffer_v, long& cherenkov_counter);
  int  CatchSPIROCBag(vector<int>& EventBuffer_v, vector<int>& buffer_v, int& layer_id, int& cycleID, int& triggerID);
  int  CatchSPIROCBag(ifstream& f_in, vector<int>& buffer_v, int& layer_id, int& cycleID, int& triggerID);
  void SetTreeBranch(TTree* tree);
  void BranchClear();
  int  DecodeAEvent(vector<int>& chip_v, int layer_ID, int Memo_ID, const bool b_auto_gain);
  int  Chipbuffer_empty()
  {
    int b_chipbuffer = 0;
    for(int i_layer = 0; i_layer < Layer_No; ++i_layer)
    {
      for(int i_chip = 0; i_chip < chip_No; ++i_chip)
      {
        if(_chip_v[i_layer][i_chip].size()) b_chipbuffer = 1;
      }
    }
    return b_chipbuffer;
  }
  int FillChipBuffer(vector<int>& buffer_v, int cycleID, int triggerID, int layer_id);
};

#endif
