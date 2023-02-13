#ifndef Treew_h_
#define Treew_h_

#include "TTree.h"

#include <iostream>
#include <string>
#include <vector>

class raw2Root
{
private:
  static const int channel_FEE = 73;  //(36charges+36times + BCIDs )*16column+ ChipID
  static const int chip_No     = 6;   // Yuzhi: Changed to ScECAL
  static const int channel_No  = 36;
  static const int Layer_No    = 32;  // Yuzhi: Changed to ScECAL
  static const int temp_No     = 16;
  std::vector<int> chipNo;

public:
  int  Decode(std::string binary_name, std::string raw_name);
  void SetTreeBranch(TTree* tree);
  int  CatchSPIROCBag(std::ifstream& f_in, std::vector<int>& buffer_v, int& layer_id, int& cycleID, int& triggerID);

  int FillChipBuffer(std::vector<int>& buffer_v, int cycleID, int triggerID, int layer_id);

  int    DecodeAEvent(std::vector<int>& chip_v, std::vector<int>& tmpr_v, int layer_ID, int Memo_ID);
  void   BranchClear();
  double cycleIDbuffer;
  int    prevCycleID;
  int    tluOverflow;
  int    Chipbuffer_empty();

private:
  std::vector<int>    _buffer_v;
  std::vector<int>    _chip_v[Layer_No][chip_No];
  std::vector<int>    _tmpr_v[Layer_No];
  //vector< int > _hitTagOri; ///< Debug: original hit tag
  std::vector<double> _time;
  int                 _temp_checkfirst[Layer_No];

private:
  Int_t                            _run_no{0};
  Int_t                            _event_time{0};
  Int_t                            _cycleID{0};
  Int_t                            _triggerID{0};
  std::vector<int>                 _cellID;
  std::vector<int>                 _bcid;
  std::vector<int>                 _hitTag;
  std::vector<int>                 _gainTag;
  std::vector<double>              _chargehi;
  std::vector<double>              _chargelo;
  std::vector<double>              _hit_time;
  std::vector<std::vector<double>> _temp;
};

#endif
