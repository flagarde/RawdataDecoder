#include "DatManager.h"

using namespace std;
int int_tmp = 0;
int         DatManager::CatchEventBag(ifstream& f_in, vector<int>& buffer_v, long& cherenkov_counter)
{
  //cout<<"catch a bag"<<endl;
  bool b_begin = 0;
  bool b_end   = 0;
  int  buffer  = 0;
  buffer_v.clear();
  while(!b_begin && f_in.read((char*)(&buffer), 1))
  {
    //cout<<hex<<buffer<<" ";
    buffer_v.push_back(buffer);
    if(buffer_v.size() > 4) buffer_v.erase(buffer_v.begin(), buffer_v.begin() + buffer_v.size() - 4);
    if(buffer_v[0] == 0xfb && buffer_v[1] == 0xee && buffer_v[2] == 0xfb && buffer_v[3] == 0xee && buffer_v.size() == 4) b_begin = 1;
  }
  while(!b_end && f_in.read((char*)(&buffer), 1))
  {
    buffer_v.push_back(buffer);
    int_tmp = buffer_v.size();
    //if(int_tmp>4 && buffer_v[int_tmp-2] == 0xfe && buffer_v[int_tmp-1] == 0xee && buffer_v[int_tmp-4] == 0xfe && buffer_v[int_tmp-3] == 0xee) b_end=1;
    if(int_tmp >= 4 && buffer_v[int_tmp - 2] == 0xfe && buffer_v[int_tmp - 1] == 0xdd && buffer_v[int_tmp - 4] == 0xfe && buffer_v[int_tmp - 3] == 0xdd) b_end = 1;
  }
  int_tmp           = buffer_v.size();
  cherenkov_counter = buffer_v[int_tmp - 8] * 0x1000000 + buffer_v[int_tmp - 7] * 0x10000 + buffer_v[int_tmp - 6] * 0x100 + buffer_v[int_tmp - 5];
  if(f_in.eof()) return 0;
  else
    return 1;
}

int DatManager::CatchSPIROCBag(vector<int>& EventBuffer_v, vector<int>& buffer_v, int& layer_id, int& cycleID, int& triggerID)
{
  if(EventBuffer_v.size() < 74)
  {
    EventBuffer_v.clear();
    return 0;
  }
  buffer_v.clear();
  bool b_begin = 0;
  bool b_end   = 0;
  int  i_begin = 0;
  int  i_end   = 0;
  for(int i = 0; i < EventBuffer_v.size(); ++i)
  {
    if(b_begin == 0 && EventBuffer_v[i] == 0xfa && EventBuffer_v[i + 1] == 0x5a && EventBuffer_v[i + 2] == 0xfa && EventBuffer_v[i + 3] == 0x5a)
    {
      b_begin = 1;
      i_begin = i;
    }
    if(b_begin == 1 && EventBuffer_v[i - 3] == 0xfe && EventBuffer_v[i - 2] == 0xee && EventBuffer_v[i - 1] == 0xfe && EventBuffer_v[i] == 0xee)
    {
      b_end = 1;
      i_end = i;
      break;
    }
  }
  buffer_v.assign(EventBuffer_v.begin() + i_begin, EventBuffer_v.begin() + i_end + 1);
  EventBuffer_v.erase(EventBuffer_v.begin(), EventBuffer_v.begin() + i_end + 1);
  if((b_begin * b_end) == 0)
  {
    buffer_v.clear();
    return 0;
  }
  if(EventBuffer_v.size() < 2)
  {
    EventBuffer_v.clear();
    buffer_v.clear();
    cout << " abnormal Eventbuffer " << endl;
    return 0;
  }
  if(EventBuffer_v[0] != 0xff)
  {
    cout << " abnormal layer ff " << hex << EventBuffer_v[0] << endl;
    buffer_v.clear();
    return 0;
  }
  if(EventBuffer_v[1] < 0 || EventBuffer_v[1] > 39)
  {
    cout << " abnormal layer " << hex << EventBuffer_v[1] << endl;
    buffer_v.clear();
    return 0;
  }
  layer_id = EventBuffer_v[1];
  EventBuffer_v.erase(EventBuffer_v.begin(), EventBuffer_v.begin() + 2);
  if((buffer_v.size()) % 2)
  {
    cout << "wrong bag size " << dec << buffer_v.size() << endl;
    buffer_v.clear();
    return 0;
  }
  for(int i = 0; i < buffer_v.size() / 2; ++i) { buffer_v[i] = buffer_v[2 * i] * 0x100 + buffer_v[2 * i + 1]; }
  buffer_v.erase(buffer_v.begin() + buffer_v.size() / 2, buffer_v.end());
  cycleID   = buffer_v[2] * 0x10000 + buffer_v[3];
  triggerID = buffer_v[4];
  buffer_v.erase(buffer_v.begin() + 2, buffer_v.begin() + 5);
  return 1;
}
int DatManager::DecodeAEvent(vector<int>& chip_v, int layer_id, int Memo_ID, const bool b_auto_gain)
{
  int size = chip_v.size();
  if((size % 73) != 1)
  {
    cout << "wrong chip size " << chip_v.size() << endl;
    return 0;
  }
  int Memo_No = size / channel_FEE;
  int offset  = 72 * (Memo_No - 1);
  for(int i_ch = 0; i_ch < channel_No; ++i_ch)
  {  //72+BCID+Chip
    int chanID = channel_No - 1 - i_ch;
    int gain   = chip_v.at(i_ch + offset + channel_No) & 0x2000;
    int hit    = chip_v.at(i_ch + offset) & 0x1000;
    int tdc    = chip_v.at(i_ch + offset) & 0x0fff;
    int adc    = chip_v.at(i_ch + offset + channel_No) & 0x0fff;
    int chip   = chip_v[size - 1] - 1;
    int BCID   = chip_v[size - 2];
    if(hit <= 1) continue;
    _cellID.push_back(layer_id * 1E5 + chip * pow(10, 4) + Memo_ID * pow(10, 2) + chanID);
    _bcid.push_back(BCID);
    if(hit > 1) _hitTag.push_back(1);
    else
      _hitTag.push_back(0);

    if(b_auto_gain)
    {
      if(gain)
      {
        _gainTag.push_back(1);
        _HG_Charge.push_back(adc);
        _LG_Charge.push_back(-1);
      }
      else
      {
        _gainTag.push_back(0);
        _HG_Charge.push_back(-1);
        _LG_Charge.push_back(adc);
      }
      _Hit_Time.push_back(tdc);
    }
    else
    {
      _HG_Charge.push_back(tdc);
      _LG_Charge.push_back(adc);
      _Hit_Time.push_back(-1);
      _gainTag.push_back(-1);
    }
  }
  chip_v.erase(chip_v.end() - 2, chip_v.end() - 1);
  chip_v.erase(chip_v.begin() + offset, chip_v.begin() + offset + 72);
  if(chip_v.size() == 1) chip_v.clear();
}

int DatManager::FillChipBuffer(vector<int>& buffer_v, int cycleID, int triggerID, int layer_id)
{
  int size = buffer_v.size();
  if(size < 4)
  {
    buffer_v.clear();
    return 0;
  }
  if(buffer_v[0] != 0xfa5a || buffer_v[1] != 0xfa5a || buffer_v[size - 2] != 0xfeee || buffer_v[size - 1] != 0xfeee)
  {
    cout << "FillChipBuffer:wrong bag package " << hex << buffer_v[0] << " " << buffer_v[1] << " " << buffer_v[size - 2] << " " << buffer_v[size - 1] << endl;
    buffer_v.clear();
    return 0;
  }
  else
  {
    buffer_v.erase(buffer_v.begin(), buffer_v.begin() + 2);
    buffer_v.erase(buffer_v.end() - 2, buffer_v.end());
    size = buffer_v.size();
  }
  for(int i = channel_FEE; i < buffer_v.size(); i = i + channel_FEE)
  {
    if(buffer_v[i] < 1 || buffer_v[i] > 9) continue;
    int chip = buffer_v[i] - 1;
    _chip_v[layer_id][chip].assign(buffer_v.begin(), buffer_v.begin() + i + 1);
    buffer_v.erase(buffer_v.begin(), buffer_v.begin() + i + 1);
    i = 0;
  }
  if(buffer_v.size())
  {
    count_chipbuffer++;
    cout << hex << cycleID << " " << buffer_v.back() << " FillChipBuffer:abnormal chip buffer " << dec << " " << layer_id << " " << buffer_v.size() << " " << count_chipbuffer << endl;
    for(int i_chip = 0; i_chip < chip_No; ++i_chip)
    {
      //_chip_v[layer_id][i_chip].clear();
    }
    buffer_v.clear();
    return 0;
  }
  return 1;
}

int DatManager::Decode(const string& input_file, const string& output_file, const bool b_auto_gain, const bool b_cherenkov)
{
  ifstream f_in;
  int      layer_id;
  int      cycleID;
  int      triggerID;
  int      BCID[Layer_No][chip_No];
  int      Memo_ID[Layer_No][chip_No];
  f_in.open(input_file, ios::in);
  if(!f_in)
  {
    cout << "cant open " << input_file << endl;
    return 0;
  }
  for(int i_layer = 0; i_layer < Layer_No; ++i_layer)
  {
    _buffer_v.clear();
    for(int i_chip = 0; i_chip < chip_No; ++i_chip)
    {
      BCID[i_layer][i_chip]    = -1;
      Memo_ID[i_layer][i_chip] = 0;
      _chip_v[i_layer][i_chip].clear();
    }
  }
  string tmp_string = input_file;
  tmp_string        = tmp_string.substr(tmp_string.find_last_of('/') + 1);
  tmp_string        = tmp_string.substr(0, tmp_string.find_last_of('.'));
  string str_out    = output_file + "/" + tmp_string + ".root";
  tmp_string        = str_out.substr(str_out.find("Run") + 3);
  tmp_string        = tmp_string.substr(0, tmp_string.find_first_of("_"));
  stringstream geek(tmp_string);
  geek >> _Run_No;
  TFile* fout;
  fout = TFile::Open(str_out.c_str(), "RECREATE");
  if(!fout)
  {
    cout << "cant create " << str_out << endl;
    return 0;
  }
  TTree* tree = new TTree("Raw_Hit", "data from binary file");
  SetTreeBranch(tree);
  int          Bag_No              = 0;
  int          Event_No            = 0;
  int          Cherenkov_Event_No1 = 0;
  int          Cherenkov_Event_No2 = 0;
  int          Cherenkov_Event_No  = 0;
  int          Abnormal_Event_No   = 0;
  int          Loop_No             = 0;
  unsigned int last_Event_Time     = 0;
  long         cherenkov_counter   = 0;
  long         pre_trigID          = 0;
  long         pre_cycleID         = 0;
  long         last_trigID         = -1;
  long         last_cycleID        = -1;
  bool         b_chipbuffer        = 0;
  bool         b_Event             = 0;
  cout << " Start Read " << str_out << " auto gain: " << b_auto_gain << " cherenkov: " << b_cherenkov << " Run:" << _Run_No << endl;
  while(!(f_in.eof()) || b_chipbuffer)
  {
    _buffer_v.clear();
    _EventBuffer_v.clear();
    CatchEventBag(f_in, _EventBuffer_v, cherenkov_counter);
    Bag_No++;
    b_Event      = 0;
    b_chipbuffer = Chipbuffer_empty();  //just in case
    while(_EventBuffer_v.size() > 74)
    {
      CatchSPIROCBag(_EventBuffer_v, _buffer_v, layer_id, cycleID, triggerID);
      if(triggerID == last_trigID)
      {
        _EventBuffer_v.clear();
        _buffer_v.clear();
        continue;
      }
      if(b_chipbuffer == 0)
      {
        pre_trigID  = triggerID;
        pre_cycleID = cycleID;
      }
      if(_buffer_v.size() < 74)
      {
        if(_buffer_v.size() != 4) cout << "abnormal SPIROC bag size " << _buffer_v.size() << endl;
        _buffer_v.clear();
        if(!b_chipbuffer) continue;
      }
      if(triggerID != pre_trigID)
      {
        b_Event = 1;
        _buffer_v.clear();
        continue;
      }
      else { FillChipBuffer(_buffer_v, cycleID, triggerID, layer_id); }
      b_chipbuffer = Chipbuffer_empty();
    }
    if(b_Event) Abnormal_Event_No++;
    while(b_chipbuffer != 0)
    {
      if((pre_trigID - last_trigID) > 10 && last_trigID != 0) { cout << hex << pre_cycleID << " Abnormal triggerID " << pre_trigID << " " << last_trigID << endl; }
      if(last_trigID - pre_trigID > 40000)
      {
        cout << "Loop " << pre_trigID << " " << last_trigID << endl;
        Loop_No++;
      }
      BranchClear();
      for(int i_layer = 0; i_layer < Layer_No; ++i_layer)
      {
        for(int i_chip = 0; i_chip < chip_No; ++i_chip)
        {
          int size = _chip_v[i_layer][i_chip].size();
          if(size == 0) continue;
          _cycleID                 = pre_cycleID;
          _triggerID               = pre_trigID + Loop_No * pow(2, 16);
          Memo_ID[i_layer][i_chip] = int(size / 73);
          DecodeAEvent(_chip_v[i_layer][i_chip], i_layer, Memo_ID[i_layer][i_chip] - 1, b_auto_gain);
          if(Memo_ID[i_layer][i_chip] != 1)
          {
            _chip_v[i_layer][i_chip].clear();
            cout << "abnormal Memo_ID " << Memo_ID[i_layer][i_chip] << endl;
          }
        }
      }
      _Event_Time = (cherenkov_counter & 0x3fffffff);
      if(b_cherenkov)
      {
        _cherenkov.push_back((cherenkov_counter & 0x80000000) / 0x80000000);
        _cherenkov.push_back((cherenkov_counter & 0x40000000) / 0x40000000);
      }
      else
      {
        _cherenkov.push_back(-1);
        _cherenkov.push_back(-1);
      }
      if(_cherenkov[0] > 0) Cherenkov_Event_No1++;
      if(_cherenkov[1] > 0) Cherenkov_Event_No2++;
      if(_cherenkov[0] * _cherenkov[1] > 0) Cherenkov_Event_No++;
      Event_No++;
      tree->Fill();
      BranchClear();
      b_chipbuffer    = Chipbuffer_empty();
      last_trigID     = pre_trigID;
      last_cycleID    = pre_cycleID;
      last_Event_Time = _Event_Time;
    }
  }
  cout << Abnormal_Event_No << " cherenkov1 " << Cherenkov_Event_No1 << " cherenkov2 " << Cherenkov_Event_No2 << " cherenkov coincidence " << Cherenkov_Event_No << " Event No " << Event_No << " Bag No  " << Bag_No << endl;
  f_in.close();
  tree->Write();
  fout->Write();
  fout->Close();
  return 1;
}

void DatManager::SetTreeBranch(TTree* tree)
{
  tree->Branch("Run_Num", &_Run_No);
  tree->Branch("Event_Time", &_Event_Time);
  tree->Branch("CycleID", &_cycleID);
  tree->Branch("TriggerID", &_triggerID);
  tree->Branch("CellID", &_cellID);
  tree->Branch("BCID", &_bcid);
  tree->Branch("HitTag", &_hitTag);
  tree->Branch("GainTag", &_gainTag);
  tree->Branch("HG_Charge", &_HG_Charge);
  tree->Branch("LG_Charge", &_LG_Charge);
  tree->Branch("Hit_Time", &_Hit_Time);
  tree->Branch("Cherenkov", &_cherenkov);
}

void DatManager::BranchClear()
{
  _cellID.clear();
  _bcid.clear();
  _hitTag.clear();
  _gainTag.clear();
  _HG_Charge.clear();
  _LG_Charge.clear();
  _Hit_Time.clear();
  _cherenkov.clear();
}

DatManager::~DatManager() {}
