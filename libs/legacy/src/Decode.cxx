#include "RawtoRoot.h"
#include "TFile.h"
#include "TMath.h"

#include <fstream>

void raw2Root::SetTreeBranch(TTree* tree)
{
  tree->Branch("Run_Num", &_run_no);
  tree->Branch("Event_Time", &_event_time);
  tree->Branch("CycleID", &_cycleID);
  tree->Branch("TriggerID", &_triggerID);
  tree->Branch("CellID", &_cellID);
  tree->Branch("BCID", &_bcid);
  tree->Branch("HitTag", &_hitTag);
  tree->Branch("GainTag", &_gainTag);
  tree->Branch("HG_Charge", &_chargehi);  //for high gain charge
  tree->Branch("LG_Charge", &_chargelo);  //for low gain charge
  tree->Branch("Hit_Time", &_hit_time);   //for AutoGain mode time information
  tree->Branch("Temperature", &_temp);    //ECAL temperature data is with scientific data*/
}

int event_time = 0;

int raw2Root::CatchSPIROCBag(std::ifstream& f_in, std::vector<int>& buffer_v, int& layer_id, int& cycleID, int& triggerID)
{
  //if(Event_No>47)   cout<<"************************catch a bag*********************"<<endl;
  bool b_begin{false};
  bool b_end{false};
  char buffer{0};
  // Yuzhi: Read with unit of 8 numbers (eg. ff5a ff5a) into buffer.
  //        When the buffer is fa5a fa5a, a layer section begins.
  //        Then switch the tag, 'b_begin', to true.
  while(!b_begin && f_in.read((char*)(&buffer), 1))
  {
    // f_in.read((char*)(&buffer),1);
    buffer_v.push_back(buffer);
    if(buffer_v.size() > 4) buffer_v.erase(buffer_v.begin(), buffer_v.begin() + buffer_v.size() - 4);
    if(buffer_v[0] == 0xfa && buffer_v[1] == 0x5a && buffer_v[2] == 0xfa && buffer_v[3] == 0x5a && buffer_v.size() == 4) { b_begin = 1; }
  }  //************** fa5a fa5a ********************

  // Yuzhi: Tag 'b_begin' is true, it starts to collect the content of the current layer section into the 'buffer_v'
  //        When the last four elements of the 'buffer_v' is 'feee feee', the current layer section ends.
  //        The the tag 'b_end' is swicthed to true, the collection loop stops here.
  //        And the buffer_v should be: fa5a fa5a ... ... feee feee.
  while(!b_end && f_in.read((char*)(&buffer), 1))
  {
    // f_in.read((char*)(&buffer),1);
    buffer_v.push_back(buffer);
    int int_tmp = buffer_v.size();
    //cout <<"buffer_v.capacity()"<< dec << buffer_v.capacity() << endl;

    if(int_tmp >= 4 && buffer_v[int_tmp - 2] == 0xfe && buffer_v[int_tmp - 1] == 0xee && buffer_v[int_tmp - 4] == 0xfe && buffer_v[int_tmp - 3] == 0xee) b_end = 1;
  }
  //for(vector<int>::iterator it=buffer_v.begin(); it!=buffer_v.end();++it)
  //{cout<<hex << *it <<" " ;}cout << endl;
  //  cout <<  "buffer_v.size " << dec << buffer_v.size() << endl;

  //***********************************************************************************************
  //************* fa5a fa5a   cycleid(32bit)    sci_dat(36+36+bcid+chip)    triggerid    f001 temp_dat(16*16 bit) f002    feee feee **************
  //***********************************************************************************************

  // Read layer
  f_in.read((char*)(&buffer), 1);
  f_in.read((char*)(&buffer), 1);
  //	cout <<hex <<"layerid:"<< buffer << endl;
  if(buffer < 0 || buffer > 31)
  {
    std::cout << " Abnormal layer " << std::hex << buffer << std::endl;
    buffer_v.clear();
    return 0;
  }
  layer_id = buffer;
  if(layer_id == 0x1f)
  {
    event_time = 0;
    for(auto i_time: {0x1000000, 0x10000, 0x100, 0x1})
    {
      f_in.read((char*)(&buffer), 1);
      if(buffer != 0) event_time += buffer * i_time;
    }
    //	cout << "event_time = " << hex << event_time << endl;
  }
  // Yuzhi: Now each element of 'buffer_v' is like "ff" (two hexadecimal numbers).
  //        Merge each closer two elements into one <int>, i.e. four hexadecimal numbers.
  //        Then the buffer_v shrinks to half.

  for(auto i = 0; i < buffer_v.size() / 2; ++i) { buffer_v[i] = buffer_v[2 * i] * 0x100 + buffer_v[2 * i + 1]; }
  buffer_v.erase(buffer_v.begin() + buffer_v.size() / 2, buffer_v.end());  //from 8bit to 16bit

  //	for(vector<int>::iterator it=buffer_v.begin(); it!=buffer_v.end();++it)
  //	{cout<<hex << *it <<" " ;}cout << endl;

  if(buffer_v.size() < 6) return 0;  //< emptybag.size   drop
  // Yuzhi: The cycleID is the first two elements behind "fa5a fa5a".
  cycleID = buffer_v[3] + buffer_v[2] * 0x10000;  //not follow Yukun
  buffer_v.erase(buffer_v.begin() + 2, buffer_v.begin() + 4);

  //	for(vector<int>::iterator it=buffer_v.begin(); it!=buffer_v.end();++it)
  //	{cout<<hex << *it <<" " ;}cout << endl;
  if(buffer_v.size() == 4) return 0;  //empty bag size = 4 (fa5a fa5a feee feee)
  if(buffer_v.size() >= 90)           // data bag at least 74 +16+ 1(trigger)+ 2fa5a + 2feee + 2(f001 f002)
  {
    if(*(buffer_v.end() - (1 + 16 + 3)) != 0xf001 || *(buffer_v.end() - (1 + 2)) != 0xf002)
    {
      std::cout << "Abnormal data bag cycleID: " << std::hex << cycleID << std::endl;
      std::cout << "buffer_zize == " << std::dec << buffer_v.size() << std::endl;
      std::for_each(buffer_v.begin(), buffer_v.end(), [](int elem) { printf("%x, ", elem); });
      return 0;
    }
  }
  else
  {
    std::cout << "Not empty bag! Not right data! CycleID: " << std::endl;
    std::cout << "buffer_zize == " << std::dec << buffer_v.size() << std::endl;
    std::for_each(buffer_v.begin(), buffer_v.end(), [](int elem) { printf("%x, ", elem); });
    return 0;
  }

  triggerID = *(buffer_v.end() - (2 + 16 + 3));
  //   buffer_v.erase(buffer_v.end() - (2 + 16 + 3), buffer_v.end());
  //   buffer_v.push_back(0xfeee); buffer_v.push_back(0xfeee);// To ensure the consistance with Yukun, return back "feee feee" to buffer_v
  buffer_v.erase(buffer_v.end() - (2 + 16 + 3));

  // Debug: See the out put
  //    std::cout << "[Debug]: Science data is (";
  //    std::for_each(buffer_v.begin(), buffer_v.end(), [](int elem){printf("%x ", elem);});
  //    std::cout << ")" << std::endl;

  // cout << "		--------------Catch SPIROC Buffer END" << endl;
  if(!f_in.eof()) return 1;
  return 0;
}

int trigger_period = 0;  // base number of Branch, triggerID.
                         // When triggerID of DAQ turns back to one,
                         // trigger_period ++

int run_no           = 0;
int Event_No         = 0;
int TMPR_No          = 0;
int count_chipbuffer = 0;

int raw2Root::Chipbuffer_empty()
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

void raw2Root::BranchClear()
{
  // branch vector clear
  _cellID.clear();
  _bcid.clear();
  _hitTag.clear();
  _gainTag.clear();
  _chargehi.clear();
  _chargelo.clear();
  _hit_time.clear();
  for(int i_layer = 0; i_layer < Layer_No; i_layer++) _temp[i_layer].clear();
}

int raw2Root::FillChipBuffer(std::vector<int>& buffer_v, int cycleID, int triggerID, int layer_id)
{
  // Yuzhi: The function is iterable. Evergy times called, the first chip section
  //        is registered and removed from buffer_v.
  int size = buffer_v.size();
  if(size < 90)
  {
    buffer_v.clear();
    return 0;
  }  //drop empty bag and abnormal bag directly
  if(buffer_v[0] != 0xfa5a || buffer_v[1] != 0xfa5a || buffer_v[size - 2] != 0xfeee || buffer_v[size - 1] != 0xfeee)
  {
    std::cout << "FillChipBuffer: wrong data format! " << std::hex << buffer_v[0] << " " << buffer_v[1] << " " << buffer_v[size - 2] << " " << buffer_v[size - 1] << std::endl;
    buffer_v.clear();
    return 0;
  }
  else
  {
    buffer_v.erase(buffer_v.begin(), buffer_v.begin() + 2);  //erase fa5a fa5a
    buffer_v.erase(buffer_v.end() - 2, buffer_v.end());      //erase feee feee
    size = buffer_v.size();
  }

  ///************* temperature dat *****************
  if(*(buffer_v.end() - (1 + 16 + 1)) != 0xf001 && *(buffer_v.end() - 1) != 0xf002) { std::cout << "No temperature dat!!!!!" << std::endl; }
  buffer_v.erase(buffer_v.end() - (1 + 16 + 1));  //erase f001 f002
  buffer_v.erase(buffer_v.end() - 1);             //only sci_dat(36+36+1+1i)*n + temp_dat(16) left
  // for (vector<int>::iterator it = buffer_v.begin(); it != buffer_v.end(); ++it) {
  // cout << hex << *it << " ";
  // }
  // cout << endl;
  if(buffer_v.size() % 74 != 16)
  {
    buffer_v.clear();
    std::cout << "buffer size abnoraml" << std::endl;
    return 1;
  }
  for(auto i = channel_FEE; i < buffer_v.size(); i = i + channel_FEE)
  {  //channel_FEE = 73
    //cout<<dec<<i<<" "<<buffer_v.size()<<" "<<hex<<buffer_v[i]<<endl;
    // Yuzhi: here I modifed buffer_v[i]>9 to buffer_v[i]>chip_No
    //        so that it is suitable for ScECAL.
    // cout << "Now buffer_size = " << dec << buffer_v.size() << endl;
    //cout<< dec <<	"buffer_v.size()"<< buffer_v.size() << endl;
    if(buffer_v[i] < 1 || buffer_v[i] > chip_No) continue;
    int chip = buffer_v[i] - 1;
    _chip_v[layer_id][chip].assign(buffer_v.begin(), buffer_v.begin() + i + 1);
    buffer_v.erase(buffer_v.begin(), buffer_v.begin() + i + 1);
    //cout<< hex <<"triggerID:"<< triggerID<< endl;
    if(buffer_v.size() == 16)
    {
      _tmpr_v[layer_id].assign(buffer_v.begin(), buffer_v.end());  //only temperature dat
      buffer_v.erase(buffer_v.begin(), buffer_v.end());
    }

    //cout<<endl<<dec<<_chip_v[layer_id][chip].back()<<" FillChipBuffer "<<" "<<buffer_v.size()<<endl;
    i = 0;
  }
  //for(vector<int>::iterator it=_tmpr_v[layer_id].begin(); it!=_tmpr_v[layer_id].end();++it){cout<<hex << *it <<" " ;}cout << endl;

  // cout << "		--------------Fill Chip Buffer END" << endl;
  // Yuzhi: !!!!!!!! CHECK  clear();
  if(buffer_v.size())
  {
    count_chipbuffer++;
    std::cout << std::hex << cycleID << "  FillChipBuffer:abnormal chip buffer " << std::dec << " " << layer_id << " " << buffer_v.size() << " " << count_chipbuffer << std::endl;
    buffer_v.clear();
    return 0;
  }
  return 1;
}

int raw2Root::DecodeAEvent(std::vector<int>& chip_v, std::vector<int>& tmpr_v, int layer_id, int Memo_ID)
{
  int size = chip_v.size();
  if((size % 73) != 1)
  {
    std::cout << "wrong chip size " << chip_v.size() << std::endl;
    return 0;
  }
  // cout << "chip_v.size() " << chip_v.size() << endl;
  int Memo_No = size / channel_FEE;
  int offset  = 72 * (Memo_No - 1);
  // cout << "layer_id : " << dec << layer_id << " in the DecodeAEvent!!" << endl;
  for(int i_ch = 0; i_ch < channel_No; ++i_ch)
  {  //72+BCID+Chip
    int chanID = channel_No - 1 - i_ch;
    int gain   = chip_v.at(i_ch + offset) & 0x2000;  //bit operation
    int hit    = chip_v.at(i_ch + offset) & 0x1000;
    int tdc    = chip_v.at(i_ch + offset) & 0x0fff;
    int adc    = chip_v.at(i_ch + offset + channel_No) & 0x0fff;
    int chip   = chip_v[size - 1] - 1;
    int bcid   = chip_v[size - 2];

    _cellID.push_back(layer_id * 1E5 + chip * 1E4 + Memo_ID * 1E2 + chanID);  // same with yukun change layer_id * 1e6 -> layer_id * 1e5
    _bcid.push_back(bcid);
    _hitTag.push_back(hit > 1 ? 1 : 0);
    _gainTag.push_back(gain > 1 ? 1 : 0);
    _chargelo.push_back(adc);
    _chargehi.push_back(tdc);
    _hit_time.push_back(-1);  //No hit_time for HL-mode
                              //No Cherenkov for ECAL
  }

  if(_temp_checkfirst[layer_id])
  {
    for(int i_temp = 0; i_temp < temp_No; i_temp++)
    {
      double tmpr = tmpr_v.at(i_temp) / 128.;
      _temp[layer_id].push_back(tmpr);
    }
    _temp_checkfirst[layer_id] = 0;
    TMPR_No++;
  }

  chip_v.erase(chip_v.end() - 2, chip_v.end() - 1);
  chip_v.erase(chip_v.begin() + offset, chip_v.begin() + offset + 72);
  if(chip_v.size() == 1) chip_v.clear();
  return 0;
  // cout << "-------------DecodeAEvent finish!!!!" << endl;
}

int raw2Root::Decode(std::string input_file, std::string output_file)
{
  std::ifstream f_in;
  int           cycleID;
  int           triggerID;
  int           BCID[Layer_No][chip_No];
  int           Memo_ID[Layer_No][chip_No];
  int           layer_id;
  //_temp.resize(Layer_No);//vector<vector<double>> Initiation

  f_in.open(input_file, std::ios::in);
  if(!f_in)
  {
    std::cout << "cant open " << input_file << std::endl;
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

  std::string str_out = output_file;
  TFile*      fout    = TFile::Open(str_out.c_str(), "RECREATE");
  if(!fout)
  {
    std::cout << "cant create " << str_out << std::endl;
    return 0;
  }
  str_out = str_out.substr(str_out.rfind("Run") + 3, 3);
  run_no  = atoi(str_out.c_str());
  std::cout << "==========str_out is " << str_out << std::endl;

  TTree* tree = new TTree("Raw_Hit", "data from binary file");
  tree->SetAutoSave(32 * 1024 * 1024);
  SetTreeBranch(tree);

  long Bag_No       = 0;
  int  preID        = 0;
  int  tmp_ID       = 0;
  //	bool b_ReadOver = 1;
  bool b_chipbuffer = 0;  // if _chip_v[layer][chip] has dat, b_chipbuffer turns 1
  for(int i_layer = 0; i_layer < Layer_No; i_layer++)
  {
    _temp.push_back(std::vector<double>());
    _temp_checkfirst[i_layer] = 1;
  }  //Init
  //while (!(f_in.eof()) || b_chipbuffer) {}
  while(!(f_in.eof()))
  {
    _buffer_v.clear();
    CatchSPIROCBag(f_in, _buffer_v, layer_id, cycleID, triggerID);
    b_chipbuffer = Chipbuffer_empty();
    if(_buffer_v.size() < 90)
    {
      //cout<<"abnormal SPIROC bag size "<<_buffer_v.size()<<endl;
      _buffer_v.clear();
      if(!b_chipbuffer) continue;
    }
    tmp_ID = triggerID;
    if(!b_chipbuffer) preID = tmp_ID;
    if(tmp_ID < preID)
    {
      std::cout << std::hex << "abnormal ID" << tmp_ID << "; residual: " << preID - tmp_ID << std::endl;  // buffer_v.clear();
    }
    if(abs(tmp_ID - preID) > 30000) { trigger_period += 65536; }
    Bag_No++;
    if((Bag_No % 100000) == 0) { std::cout << std::dec << " Bag_No " << Bag_No << "	Event_No " << Event_No << std::endl; }

    // Yuzhi: iterate all chip sections in the "buffer_v"
    if(tmp_ID == preID) { FillChipBuffer(_buffer_v, cycleID, triggerID, layer_id); }
    if(tmp_ID > preID || abs(tmp_ID - preID) > 30000)
    {
      while(b_chipbuffer)
      {
        //	long int EventID = 0x1000000000000;
        //	int b_layer = 0;
        for(int i_layer = 0; i_layer < Layer_No; ++i_layer)
        {
          for(int i_chip = 0; i_chip < chip_No; ++i_chip)
          {
            int size = _chip_v[i_layer][i_chip].size();
            if(size == 0) continue;
            Memo_ID[i_layer][i_chip] = size / 73;
            BCID[i_layer][i_chip]    = _chip_v[i_layer][i_chip].at(size - 2);
            //tmp_ID = long(BCID[i_layer][i_chip]);
            //if (EventID > tmp_ID)
            //	EventID = tmp_ID;
          }
        }
        //cout<<"debug chip buffer"<<endl;
        //fill chip buffer over
        //coincidence judgement
        int b_coincidence = 0;
        BranchClear();
        for(int i_layer = 0; i_layer < Layer_No; ++i_layer)
        {
          //_temp.push_back(vector<double>());
          _temp_checkfirst[i_layer] = 1;
          // cout << "vector<vector>_size " << _temp.size() << "	layer:	" << i_layer << endl;
          int tmp                   = 0;
          for(int i_chip = 0; i_chip < chip_No; ++i_chip)
          {
            int size = _chip_v[i_layer][i_chip].size();
            if(size == 0) continue;
            //long int ID_tmp = long(BCID[i_layer][i_chip]);
            // cout << hex << ID_tmp << " coincidence " << EventID << dec << " " << i_chip << " " << i_layer << endl;
            //if (EventID == ID_tmp) {}
            if(true)
            {
              _run_no     = run_no;
              _event_time = event_time;
              _cycleID    = cycleID;
              _triggerID  = triggerID + trigger_period;
              DecodeAEvent(_chip_v[i_layer][i_chip], _tmpr_v[i_layer], i_layer, Memo_ID[i_layer][i_chip] - 1);
              Memo_ID[i_layer][i_chip]--;
            }
          }
          b_coincidence += tmp;
        }
        Event_No++;
        // std::cout << "*******************************Event_No is " << Event_No << " !*******************************" << std::endl;
        //XUAN : Debug	if(_cycleID>390889&&_cycleID<390893)
        tree->Fill();

        //if(Event_No>2500) {tree->Write();fout->Write();fout->Close();return 1;}
        BranchClear();
        preID        = tmp_ID;
        b_chipbuffer = Chipbuffer_empty();
      }  //the second while
      FillChipBuffer(_buffer_v, cycleID, triggerID, layer_id);
    }  //if
  }
  f_in.close();
  tree->Write();
  fout->Write();
  fout->Close();
  return 0;
}
