/**
*  \file ROOTWriter.cc
*  \copyright 2022 F.Lagarde
*/

#include "ROOTWriter.h"

#include "Header.h"
#include "InfosFromFilename.h"

void ROOTWriter::setFilename(const std::string& filename) { m_Filename = filename; }

ROOTWriter::ROOTWriter() : InterfaceWriter("ROOTWriter", "1.0.0") { addCompatibility("RawdataReader", ">=1.0.0"); }

void ROOTWriter::processHeader(const Buffer& d)
{
  Header header(d);
  m_eventTime = getTime() + header.getTimestamp();
  m_cherenkov = header.getCherenkov();
}

void ROOTWriter::start(const VersionInfos& ver)
{
  setTime(extractTimestamp(m_Filename));
  setRunNumber(extractRunNumber(m_Filename));

  m_File = TFile::Open(m_Filename.c_str(), "RECREATE", m_Filename.c_str(), ROOT::CompressionSettings(ROOT::kZLIB, 5));

  // Informations from streamout
  TTree*      infos           = new TTree("Infos", "Library and interfaces informations");
  std::string Library_Name    = ver.getLibraryInfos().first;
  std::string Library_Version = ver.getLibraryInfos().second.to_string();
  std::string Reader_Name     = ver.getReaderInfos().first;
  std::string Reader_Version  = ver.getReaderInfos().second.to_string();
  std::string Writer_Name     = ver.getWriterInfos().first;
  std::string Writer_Version  = ver.getWriterInfos().second.to_string();
  infos->Branch("Library_Name", &Library_Name);
  infos->Branch("Library_Version", &Library_Version);
  infos->Branch("Reader_Name", &Reader_Name);
  infos->Branch("Reader_Version", &Reader_Version);
  infos->Branch("Writer_Name", &Writer_Name);
  infos->Branch("Writer_Version", &Writer_Version);
  infos->Write();
  delete infos;
  //

  m_Tree = new TTree("RawData", "Rawdata");
  m_Tree->Branch("runNumber", &m_runNumber);
  m_Tree->Branch("eventTime", &m_eventTime);
  m_Tree->Branch("eventNumber", &m_eventNumber);
  m_Tree->Branch("detectorID", &m_detectorID);
  m_Tree->Branch("cherenkov", &m_cherenkov);
  m_Tree->Branch("cycleID", &m_cycleID);
  m_Tree->Branch("triggerID", &m_triggerID);
  m_Tree->Branch("cellIDs", &m_cellID);
  m_Tree->Branch("BCIDs", &m_bcid);
  m_Tree->Branch("hitTags", &m_hitTag);
  m_Tree->Branch("gainTags", &m_gainTag);
  m_Tree->Branch("HG_Charge", &m_charge);
  m_Tree->Branch("LG_Charge", &m_time);
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

void ROOTWriter::processChip(const Data& d, const std::uint32_t& frameIndex) {}

void ROOTWriter::processCell(const Data& d, const std::uint32_t& column, const std::uint32_t& channelIndex)
{
  m_runNumber   = getRunNumber();
  m_eventNumber = getEventNumber();
  m_detectorID  = d.getDetectorID();
  m_cycleID     = d.getCycleID();
  m_triggerID   = d.getTriggerID();
  m_hitTag.push_back(d.getChip().getCharge(column, channelIndex).hit());
  m_gainTag.push_back(d.getChip().getCharge(column, channelIndex).gain());
  m_time.push_back(d.getChip().getTime(column, channelIndex).timestamp());
  m_charge.push_back(d.getChip().getCharge(column, channelIndex).charge());
  m_bcid.push_back(d.getChip().getBCIDs(column));
  m_cellID.push_back(d.getLayer() * 1e5 + d.getChip().getID() * 1e4 + column * 1e2 + channelIndex);
}

void ROOTWriter::startEvent() {}

void ROOTWriter::endEvent()
{
  m_Tree->Fill();
  m_hitTag.clear();
  m_gainTag.clear();
  m_time.clear();
  m_charge.clear();
  m_bcid.clear();
  m_cellID.clear();
}

void ROOTWriter::startDIF() {}

void ROOTWriter::endDIF() {}

void ROOTWriter::startChip() {}

void ROOTWriter::endChip() {}

void ROOTWriter::startCell() {}

void ROOTWriter::endCell() {}
