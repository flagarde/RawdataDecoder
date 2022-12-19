/** \file LCIOWriter.cc
*  \copyright 2022 G.Grenier F.Lagarde
*/

#include "LCIOWriter.h"

#include "Filesystem.h"
#include "Formatters.h"
#include "IMPL/LCFlagImpl.h"
#include "IMPL/LCParametersImpl.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IOIMPL/LCFactory.h"
#include "LCIOSTLTypes.h"
#include "UTIL/CellIDEncoder.h"

void LCIOWriter::setFilename(const std::string& filename) { m_Filename = filename; }

LCIOWriter::LCIOWriter() : InterfaceWriter("LCIOWriter", "1.0.0"), m_LCWriter(IOIMPL::LCFactory::getInstance()->createLCWriter()) { addCompatibility("RawdataReader", ">=1.0.0"); }

void LCIOWriter::start(const VersionInfos& ver)
{
  m_LCWriter->open(m_Filename, EVENT::LCIO::WRITE_NEW);
  std::unique_ptr<IMPL::LCRunHeaderImpl> runHdr(new IMPL::LCRunHeaderImpl);
  std::string                            filename_ = filename(m_Filename);
  std::size_t                            begin_    = filename_.find_first_of("_Run");
  filename_                                        = filename_.substr(begin_ + 4);
  begin_                                           = filename_.find_first_of("_");
  filename_                                        = filename_.substr(0, begin_);
  setRunNumber(stoi(filename_));
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

void LCIOWriter::processDIF(const Data& d)
{
  //log()->critical("{}",to_hex(d.getData()));
  /*std::string   parameter_name = "DIF" + std::to_string(d.getDIFid()) + "_Triggers";
  EVENT::IntVec parameters;
  parameters.push_back(d.getDTC());
  parameters.push_back(d.getGTC());
  parameters.push_back(d.getBCID());
  parameters.push_back(d.getAbsoluteBCID() & 0xFFFFFF);
  parameters.push_back((d.getAbsoluteBCID() >> 24) & 0xFFFFFF);
  parameters.push_back(0);
  parameters.push_back(0);
  parameters.push_back(0);
  m_CollectionVec->parameters().setValues("DIF" + std::to_string(d.getDIFid()) + "_Triggers", parameters);
  parameter_name = "DIF_DetectorID_" + std::to_string(d.getDIFid());
  m_CollectionVec->parameters().setValue(parameter_name, static_cast<int>(d.getDetectorID()));*/
}

void LCIOWriter::processFrame(const Data& d, const std::uint32_t& frameIndex) {}

void LCIOWriter::processPadInFrame(const Data& d, const std::uint32_t& column, const std::uint32_t& channelIndex)
{
  UTIL::CellIDEncoder<IMPL::RawCalorimeterHitImpl> cd("BCID:16,gain:1,hit:1,layer:8,chip:8,channel:8", m_CollectionVec);
  m_LCEvent->setTimeStamp(d.getTriggerID());
  m_LCEvent->setRunNumber(getRunNumber());

  IMPL::RawCalorimeterHitImpl* hit = new IMPL::RawCalorimeterHitImpl;
  cd["BCID"]                       = d.getChip().getBCIDs(column);
  cd["gain"]                       = d.getChip().getCharge(column, channelIndex).gain();
  cd["hit"]                        = d.getChip().getCharge(column, channelIndex).hit();
  cd["layer"]                      = d.getLayer();
  cd["chip"]                       = d.getChip().getID();
  cd["channel"]                    = channelIndex;
  cd.setCellID(hit);
  hit->setAmplitude(d.getChip().getCharge(column, channelIndex).charge());
  hit->setTimeStamp(d.getChip().getTime(column, channelIndex).timestamp());
  m_CollectionVec->addElement(hit);
}

void LCIOWriter::startEvent()
{
  m_LCEvent = std::make_unique<IMPL::LCEventImpl>();
  /// This event number is the vent number without empty payload
  m_LCEvent->setEventNumber(myEventNumber);
  m_LCEvent->setDetectorName(m_DetectorName);
  m_LCEvent->setWeight(1);
  m_CollectionVec = new IMPL::LCCollectionVec(EVENT::LCIO::RAWCALORIMETERHIT);
  IMPL::LCFlagImpl flag(0);
  flag.setBit(EVENT::LCIO::RCHBIT_ID1);
  flag.setBit(EVENT::LCIO::RCHBIT_TIME);
  m_CollectionVec->setFlag(flag.getFlag());

  //m_CollectionVec->parameters().setValue(EVENT::LCIO::CellIDEncoding, "gain:1,hit:1,layer:8,chip:8,channel:8");
}

void LCIOWriter::endEvent()
{
  if(!m_CollectionVec->empty())
  {
    myEventNumber++;
    m_LCEvent->addCollection(m_CollectionVec, "HCALRawHits");
    m_LCWriter->writeEvent(m_LCEvent.get());
  }
}

void LCIOWriter::startDIF() {}

void LCIOWriter::endDIF() {}

void LCIOWriter::startFrame() {}

void LCIOWriter::endFrame() {}

void LCIOWriter::startPad() {}

void LCIOWriter::endPad() {}
