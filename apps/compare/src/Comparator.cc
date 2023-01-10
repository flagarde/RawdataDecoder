#include "Comparator.h"

#include "TStyle.h"

Comparator::Comparator(const std::string& old, const std::string& nouveau)
{
  ROOT::EnableImplicitMT();
  m_oldData = std::make_unique<ROOT::RDataFrame>(m_TreeName, old);
  m_newData = std::make_unique<ROOT::RDataFrame>(m_TreeName, nouveau);
}

void Comparator::Run()
{
  ADC_Low();
  ADC_High();
  num_hits();
  layer();
  chip();
  memory();
  channel();
  cellID();
}

void Comparator::ADC_Low()
{
  auto a = m_oldData->Define("ADC", [](ROOT::RVecD& adc, ROOT::RVecI& hit_tag) { return adc[hit_tag == 1]; }, {"LG_Charge", "HitTag"}).Histo1D<ROOT::RVecD>({"Legacy", ";ADC;Count", 4096, 0, 4096}, "ADC");
  auto b = m_newData->Define("ADC", [](ROOT::RVecD& adc, ROOT::RVecI& hit_tag) { return adc[hit_tag == 1]; }, {"LG_Charge", "HitTag"}).Histo1D<ROOT::RVecD>({"New", ";ADC;Count", 4096, 0, 4096}, "ADC");
  generateCanvas(a, b, ";Low ADC; #Delta");
}

void Comparator::ADC_High()
{
  auto a = m_oldData->Define("ADC", [](ROOT::RVecD& adc, ROOT::RVecI& hit_tag) { return adc[hit_tag == 1]; }, {"HG_Charge", "HitTag"}).Histo1D<ROOT::RVecD>({"Legacy", ";ADC;Count", 4096, 0, 4096}, "ADC");
  auto b = m_newData->Define("ADC", [](ROOT::RVecD& adc, ROOT::RVecI& hit_tag) { return adc[hit_tag == 1]; }, {"HG_Charge", "HitTag"}).Histo1D<ROOT::RVecD>({"New", ";ADC;Count", 4096, 0, 4096}, "ADC");
  generateCanvas(a, b, ";High ADC; #Delta");
}

void Comparator::SaveAsPDF(const std::string& file) { m_Ploter.save(file.c_str()); }

void Comparator::num_hits()
{
  auto a = m_oldData->Define("Sum", "Sum(HitTag == 1)").Histo1D({"Legacy", ";Num. of Hits;Count", 200, 0, 200}, "Sum");
  auto b = m_newData->Define("Sum", "Sum(HitTag == 1)").Histo1D({"New", ";Num. of Hits;Count", 200, 0, 200}, "Sum");
  generateCanvas(a, b, ";Num. of Hits; #Delta", false);
}

void Comparator::layer()
{
  auto a = m_oldData->Define("tmp", "ROOT::RVec<int>(CellID[HitTag == 1]/1e5)").Histo1D({"Legacy", ";Layer Index;Count", 42, 0, 42}, "tmp");
  auto b = m_newData->Define("tmp", "ROOT::RVec<int>(CellID[HitTag == 1]/1e5)").Histo1D({"New", ";Layer Index;Count", 42, 0, 42}, "tmp");
  generateCanvas(a, b, ";Layer Index; #Delta");
}

void Comparator::chip()
{
  auto a = m_oldData->Define("tmp", [](ROOT::RVecI& adc, ROOT::RVecI& hit_tag) { return (adc / 1e4) - (adc / 1e5) * 10; }, {"CellID", "HitTag"}).Histo1D({"Legacy", ";Chip Index;Count", 9, 1, 10}, "tmp");
  auto b = m_newData->Define("tmp", [](ROOT::RVecI& adc, ROOT::RVecI& hit_tag) { return (adc / 1e4) - (adc / 1e5) * 10; }, {"CellID", "HitTag"}).Histo1D({"New", ";Chip Index;Count", 9, 1, 10}, "tmp");
  generateCanvas(a, b, ";Chip Index; #Delta");
}

void Comparator::memory()
{
  auto a = m_oldData->Define("tmp", [](ROOT::RVecI& adc, ROOT::RVecI& hit_tag) { return (adc / 1e2) - (adc / 1e4) * 100; }, {"CellID", "HitTag"}).Histo1D({"Legacy", ";Layer Index;Count", 16, 0, 16}, "tmp");
  auto b = m_newData->Define("tmp", [](ROOT::RVecI& adc, ROOT::RVecI& hit_tag) { return (adc / 1e2) - (adc / 1e4) * 100; }, {"CellID", "HitTag"}).Histo1D({"New", ";Layer Index;Count", 16, 0, 16}, "tmp");
  generateCanvas(a, b, ";Memory Index; #Delta");
}

void Comparator::channel()
{
  auto a = m_oldData->Define("tmp", "ROOT::RVec<int>(CellID[HitTag == 1]%100)").Histo1D({"Legacy", ";Layer Index;Count", 36, 0, 36}, "tmp");
  auto b = m_newData->Define("tmp", "ROOT::RVec<int>(CellID[HitTag == 1]%100)").Histo1D({"New", ";Layer Index;Count", 36, 0, 36}, "tmp");
  generateCanvas(a, b, ";Channel Index; #Delta");
}

void Comparator::cellID()
{
  auto a = m_oldData->Define("tmp", "ROOT::RVec<int>(CellID[HitTag == 1])").Histo1D({"Legacy", ";Layer Index;Count", 4300000, 0, 4300000}, "tmp");
  auto b = m_newData->Define("tmp", "ROOT::RVec<int>(CellID[HitTag == 1])").Histo1D({"New", ";Layer Index;Count", 4300000, 0, 4300000}, "tmp");
  generateCanvas(a, b, ";CellID; #Delta");
}
