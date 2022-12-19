/**
*  \file ROOTWriter.cc
*  \copyright 2022 F.Lagarde
*/

#include "ROOTWriter.h"

void ROOTWriter::setFilename(const std::string& filename) { m_Filename = filename; }

ROOTWriter::ROOTWriter() : InterfaceWriter("ROOTWriter", "1.0.0") { addCompatibility("RawdataReader", ">=1.0.0"); }

void ROOTWriter::start(const VersionInfos& ver)
{
  m_File = TFile::Open(m_Filename.c_str(), "RECREATE", m_Filename.c_str(), ROOT::CompressionSettings(ROOT::kZLIB, 5));
  m_Tree = new TTree("RawData", "Raw SDHCAL data tree");
  m_Tree->Branch("cycleID", &_cycleID);
  m_Tree->Branch("triggerID", &_triggerID);
  m_Tree->Branch("cellIDs", &_cellID);
  m_Tree->Branch("BCIDs", &_bcid);
  m_Tree->Branch("hitTags", &_hitTag);
  m_Tree->Branch("gainTags", &_gainTag);
  m_Tree->Branch("charges", &_charge);
  m_Tree->Branch("times", &_time);
}

void ROOTWriter::end()
{
  if(m_Tree) m_Tree->Write();
  if(m_File)
  {
    m_File->Write();
    m_File->Close();
  }
  if(m_File) delete m_File;
}

void ROOTWriter::processDIF(const Data& d) {}

void ROOTWriter::processFrame(const Data& d, const std::uint32_t& frameIndex) {}

void ROOTWriter::processPadInFrame(const Data& d, const std::uint32_t& column, const std::uint32_t& channelIndex)
{
  _cycleID   = d.getCycleID();
  _triggerID = d.getTriggerID();
  _hitTag.push_back(d.getChip().getCharge(column, channelIndex).hit());
  _gainTag.push_back(d.getChip().getCharge(column, channelIndex).gain());
  _time.push_back(d.getChip().getTime(column, channelIndex).timestamp());
  _charge.push_back(d.getChip().getCharge(column, channelIndex).charge());
  _bcid.push_back(d.getChip().getBCIDs(column));
  _cellID.push_back(d.getLayer() * 1e5 + d.getChip().getID() * 1e4 + column * 1e2 + channelIndex);
}

void ROOTWriter::startEvent() {}

void ROOTWriter::endEvent()
{
  m_Tree->Fill();
  _hitTag.clear();
  _gainTag.clear();
  _time.clear();
  _charge.clear();
  _bcid.clear();
  _cellID.clear();
}

void ROOTWriter::startDIF() {}

void ROOTWriter::endDIF() {}

void ROOTWriter::startFrame() {}

void ROOTWriter::endFrame() {}

void ROOTWriter::startPad() {}

void ROOTWriter::endPad() {}
