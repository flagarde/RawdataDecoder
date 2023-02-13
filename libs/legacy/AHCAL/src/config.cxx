#include "config.h"

#include "DatManager.h"

#include <fstream>

using namespace std;

void Config::Parse(const string config_file)
{
  conf = YAML::LoadFile(config_file);
  //Printing version number
  cout << "HBUANA Version: " << conf["hbuana"]["version"].as<std::string>() << endl;
  cout << "HBUANA Github Repository: " << conf["hbuana"]["github"].as<std::string>() << endl;
}

int Config::Run()
{
  if(conf["DAT-ROOT"]["on-off"].as<bool>())
  {
    cout << "DAT mode: ON" << endl;
    if(conf["DAT-ROOT"]["auto-gain"].as<bool>()) cout << "auto gain mode: ON" << endl;      //<<(conf["DAT-ROOT"]["auto-gain"].as<bool>())<<endl;
    if(conf["DAT-ROOT"]["cherenkov"].as<bool>()) cout << "cherenkov detector: ON" << endl;  //<<(conf["DAT-ROOT"]["auto-gain"].as<bool>())<<endl;
    if(conf["DAT-ROOT"]["file-list"].as<std::string>() == "" || conf["DAT-ROOT"]["output-dir"].as<std::string>() == "") { cout << "ERROR: Please specify file list or output-dir for dat files" << endl; }
    else
    {
      ifstream   dat_list(conf["DAT-ROOT"]["file-list"].as<std::string>());
      DatManager dm;
      while(!dat_list.eof())
      {
        string dat_temp;
        dat_list >> dat_temp;
        if(dat_temp == "") continue;
        dm.Decode(dat_temp, conf["DAT-ROOT"]["output-dir"].as<std::string>(), conf["DAT-ROOT"]["auto-gain"].as<bool>(), conf["DAT-ROOT"]["cherenkov"].as<bool>());
      }
    }
  }
  if(conf["Pedestal"]["on-off"].as<bool>())
  {
    cout << "Pedestal mode: ON" << endl;
    if(conf["Pedestal"]["Cosmic"]["on-off"].as<bool>())
    {
      cout << "Pedestal mode for cosmic events: ON" << endl;
    }
    if(conf["Pedestal"]["DAC"]["on-off"].as<bool>())
    {
      cout << "Pedestal mode for DAC events: ON" << endl;
    }
  }
  if(conf["Calibration"]["on-off"].as<bool>())
  {
    if(conf["Calibration"]["Cosmic"]["on-off"].as<bool>())
    {
      cout << "Cosmic calibration mode:ON" << endl;
    }
    if(conf["Calibration"]["DAC"]["on-off"].as<bool>())
    {
      cout << "DAC Calibration mode:ON" << endl;
    }
  }
  return 1;
}


Config::Config() : conf(0) {}

Config::~Config() {}
