/**
*  \file ROOTWriter.cc
*  \copyright 2022 F.Lagarde
*/

#include "ROOTWriter.h"

#include "CellID.h"
#include "Header.h"
#include "InfosFromFilename.h"

void ROOTWriter::setFilename(const std::string& filename) { m_Filename = filename; }

ROOTWriter::ROOTWriter() : InterfaceWriter("ROOTWriter", "1.0.0")
{
  addCompatibility("RawdataReader", ">=1.0.0");
  //myfile.open("BAD_CHID_ID_" + m_Filename + ".txt");
  //myfile << "Event,Layer,Chip,Channel\n";
}

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

  m_File = TFile::Open(m_Filename.c_str(), "RECREATE", m_Filename.c_str() /*, ROOT::CompressionSettings(ROOT::kZLIB, 5)*/);

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

  m_Tree = new TTree("Raw_Hit", "Rawdata");
  //m_Tree->SetAutoSave(320 * 1024 * 1024);
  m_Tree->Branch("Run_Num", &m_runNumber, 32 * 1024 * 1024);
  m_Tree->Branch("Event_Time", &m_eventTime, 32 * 1024 * 1024);
  m_Tree->Branch("Event_Number", &m_eventNumber, 32 * 1024 * 1024);
  m_Tree->Branch("DetectorID", &m_detectorID, 32 * 1024 * 1024);
  m_Tree->Branch("Cherenkov", &m_cherenkov, 32 * 1024 * 1024);
  m_Tree->Branch("CycleID", &m_cycleID, 32 * 1024 * 1024);
  m_Tree->Branch("TriggerID", &m_triggerID, 32 * 1024 * 1024);
  m_Tree->Branch("CellID", &m_cellID, 32 * 1024 * 1024);
  m_Tree->Branch("BCID", &m_bcid, 32 * 1024 * 1024);
  m_Tree->Branch("HitTag", &m_hitTag, 32 * 1024 * 1024);
  m_Tree->Branch("GainTag", &m_gainTag, 32 * 1024 * 1024);
  m_Tree->Branch("HG_Charge", &m_charge, 32 * 1024 * 1024);
  m_Tree->Branch("LG_Charge", &m_time, 32 * 1024 * 1024);
  m_Tree->Branch("Channel", &m_channel, 32 * 1024 * 1024);
  m_Tree->Branch("Chip", &m_chip, 32 * 1024 * 1024);
  m_Tree->Branch("Layer", &m_layer, 32 * 1024 * 1024);
  m_Tree->Branch("Memory", &m_memory, 32 * 1024 * 1024);
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

void ROOTWriter::processCell(const Data& d, const std::uint32_t& chip, const std::uint32_t& channel)
{
  m_runNumber   = getRunNumber();
  m_eventNumber = getEventNumber();
  m_detectorID  = d.getDetectorID();
  m_cycleID     = d.getCycleID();
  m_triggerID   = d.getTriggerID();
  //std::cout<<d.getChip(chip).getNumberColumns()<<std::endl;
  for(std::size_t i = 0; i != d.getChip(chip).getNumberColumns(); ++i)
  {
    if(d.getChip(chip).getID() >= 10) log()->error("Chip_id >=10 ({}) : Layer {} Chip {} memory {} channel {}", d.getChip(chip).getID(), d.getLayer(), d.getChip(chip).getID(), i, channel);
    m_hitTag.push_back(d.getChip(chip).getCharge(i, channel).hit());
    m_gainTag.push_back(d.getChip(chip).getCharge(i, channel).gain());
    m_time.push_back(d.getChip(chip).getTime(i, channel).timestamp());
    m_charge.push_back(d.getChip(chip).getCharge(i, channel).charge());
    m_bcid.push_back(d.getChip(chip).getBCIDs(i));
    CellID myCellID(d.getLayer(), d.getChip(chip).getID(), i, channel);
    if(d.getLayer() != myCellID.getLayerID() || d.getChip(chip).getID() != myCellID.getCellID() || i != myCellID.getMemory() || channel != myCellID.getChannel()) throw("Problem encoding CellID");
    m_cellID.push_back(myCellID.getCellID());
    m_channel.push_back(channel);
    m_chip.push_back(d.getChip(chip).getID());
    m_layer.push_back(d.getLayer());
    m_memory.push_back(i);
    if(static_cast<DetectorID>(d.getDetectorID()) == DetectorID::ECAL) m_cherenkov = -1;
    //else { myfile << m_eventNumber << "," << d.getLayer() << "," << d.getChip(chip).getID() << "," << channel << "\n"; }
  }
}

void ROOTWriter::startEvent()
{
  m_hitTag.clear();
  m_gainTag.clear();
  m_time.clear();
  m_charge.clear();
  m_bcid.clear();
  m_cellID.clear();
  m_channel.clear();
  m_chip.clear();
  m_layer.clear();
  m_memory.clear();
}

void ROOTWriter::endEvent()
{
  if(!m_hitTag.empty()) m_Tree->Fill();
  m_hitTag.clear();
  m_gainTag.clear();
  m_time.clear();
  m_charge.clear();
  m_bcid.clear();
  m_cellID.clear();
  m_channel.clear();
  m_chip.clear();
  m_layer.clear();
  m_memory.clear();
}

void ROOTWriter::startDIF() {}

void ROOTWriter::endDIF() {}

void ROOTWriter::startChip() {}

void ROOTWriter::endChip() {}

void ROOTWriter::startCell() {}

void ROOTWriter::endCell() {}
