/** \file LCIOWriter.cc
*  \copyright 2022 G.Grenier F.Lagarde
*/

#include "LCIOWriter.h"

#include "Header.h"
#include "IMPL/LCFlagImpl.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IOIMPL/LCFactory.h"
#include "InfosFromFilename.h"
#include "LCIOSTLTypes.h"
#include "UTIL/CellIDEncoder.h"

void LCIOWriter::setFilename(const std::string& filename) { m_Filename = filename; }

LCIOWriter::LCIOWriter() : InterfaceWriter("LCIOWriter", "1.0.0"), m_LCWriter(IOIMPL::LCFactory::getInstance()->createLCWriter()) { addCompatibility("RawdataReader", ">=1.0.0"); }

void LCIOWriter::start(const VersionInfos& ver)
{
  setTime(extractTimestamp(m_Filename));
  setRunNumber(extractRunNumber(m_Filename));

  m_LCWriter->open(m_Filename, EVENT::LCIO::WRITE_NEW);

  std::unique_ptr<IMPL::LCRunHeaderImpl> runHdr(new IMPL::LCRunHeaderImpl);
  runHdr->setRunNumber(getRunNumber());
  runHdr->setDetectorName(m_DetectorName);
  std::string description("data collected with prototype");
  runHdr->setDescription(description);
  runHdr->parameters().setValue("Library_Name", ver.getLibraryInfos().first);
  runHdr->parameters().setValue("Library_Version", ver.getLibraryInfos().second.to_string());
  runHdr->parameters().setValue("Reader_Name", ver.getReaderInfos().first);
  runHdr->parameters().setValue("Reader_Version", ver.getReaderInfos().second.to_string());
  runHdr->parameters().setValue("Writer_Name", ver.getWriterInfos().first);
  runHdr->parameters().setValue("Writer_Version", ver.getWriterInfos().second.to_string());
  m_LCWriter->writeRunHeader(runHdr.get());
}

void LCIOWriter::end() { m_LCWriter->close(); }

void LCIOWriter::processHeader(const Buffer& d)
{
  Header header(d);
  m_LCEvent->setTimeStamp((getTime() + header.getTimestamp()) * 1e9);
  m_LCEvent->parameters().setValue("Cherenkov", header.getCherenkov());
}

void LCIOWriter::processDIF(const Data& d) {}

void LCIOWriter::processChip(const Data& d, const std::uint32_t& frameIndex) {}

void LCIOWriter::processCell(const Data& d, const std::uint32_t& chip, const std::uint32_t& channel)
{
  UTIL::CellIDEncoder<IMPL::RawCalorimeterHitImpl> cd("BCID:16,gain:1,hit:1,layer:8,chip:8,channel:8", m_CollectionVec);
  m_LCEvent->setTimeStamp(m_LCEvent->getTimeStamp() + d.getTriggerID());

  IMPL::RawCalorimeterHitImpl* hit = new IMPL::RawCalorimeterHitImpl;
  for(std::size_t memory = 0; memory != d.getChip(chip).getNumberColumns(); ++memory)
  {
    if(d.getChip(chip).getID() <= 10)  //FIXME
    {
      cd["BCID"]    = d.getChip(chip).getBCIDs(memory);
      cd["gain"]    = d.getChip(chip).getCharge(memory, channel).gain();
      cd["hit"]     = d.getChip(chip).getCharge(memory, channel).hit();
      cd["layer"]   = d.getLayer();
      cd["chip"]    = d.getChip(chip).getID();
      cd["channel"] = channel;
      cd.setCellID(hit);
      hit->setAmplitude(d.getChip(chip).getCharge(memory, channel).charge());
      hit->setTimeStamp(d.getChip(chip).getTime(memory, channel).timestamp());
      if(static_cast<DetectorID>(d.getDetectorID()) == DetectorID::ECAL) m_LCEvent->parameters().setValue("Cherenkov", -1);
    }
  }
  m_CollectionVec->addElement(hit);
}

void LCIOWriter::startEvent()
{
  m_LCEvent = std::make_unique<IMPL::LCEventImpl>();
  m_LCEvent->setRunNumber(getRunNumber());
  m_LCEvent->setEventNumber(getEventNumber());
  m_LCEvent->setDetectorName(m_DetectorName);
  m_LCEvent->setWeight(1);
  m_CollectionVec = new IMPL::LCCollectionVec(EVENT::LCIO::RAWCALORIMETERHIT);
  IMPL::LCFlagImpl flag(0);
  flag.setBit(EVENT::LCIO::RCHBIT_ID1);
  flag.setBit(EVENT::LCIO::RCHBIT_TIME);
  m_CollectionVec->setFlag(flag.getFlag());
}

void LCIOWriter::endEvent()
{
    m_LCEvent->addCollection(m_CollectionVec, "HCALRawHits");
    m_LCWriter->writeEvent(m_LCEvent.get());
}

void LCIOWriter::startLayer() {}

void LCIOWriter::endLayer() {}

void LCIOWriter::startChip() {}

void LCIOWriter::endChip() {}

void LCIOWriter::startCell() {}

void LCIOWriter::endCell() {}
