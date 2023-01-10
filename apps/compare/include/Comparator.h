#pragma once

#include "Ploter.h"
#include "ROOT/RDataFrame.hxx"

#include <memory>
#include <string>

class Comparator
{
public:
  Comparator(const std::string& old, const std::string& nouveau);
  void Run();
  void SaveAs(const std::string& file);

private:
  std::unique_ptr<ROOT::RDataFrame> m_oldData{nullptr};
  std::unique_ptr<ROOT::RDataFrame> m_newData{nullptr};
  std::string                       m_TreeName     = "Raw_Hit";
  std::string                       BR_CELL_ID_A   = "CellID";
  std::string                       BR_TAG_A       = "HitTag";
  std::string                       BR_LOW_GAIN_A  = "LG_Charge";
  std::string                       BR_HIGH_GAIN_A = "HG_Charge";

  void   en_spectrum(std::string hl_gain);
  void   num_hits();
  void   layer();
  Ploter m_Ploter;
};
