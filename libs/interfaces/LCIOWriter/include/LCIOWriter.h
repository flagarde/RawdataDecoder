/** \file LCIOWriter.h
*  \copyright 2022 F.Lagarde
*/

#pragma once

#include "Data.h"
#include "EVENT/LCIO.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/RawCalorimeterHitImpl.h"
#include "IO/LCWriter.h"
#include "Interface.h"

#include <memory>
#include <string>
#include <vector>

class LCIOWriter : public InterfaceWriter
{
public:
  LCIOWriter();

  void setFilename(const std::string&);

  void         start(const VersionInfos& ver) final;
  virtual void processHeader(const Buffer&) final;
  void         processDIF(const Data&);
  void         processChip(const Data&, const std::uint32_t& frameIndex);
  void         processCell(const Data&, const std::uint32_t& frameIndex, const std::uint32_t& channelIndex);
  void         end();

  virtual void startEvent();
  virtual void endEvent();
  virtual void startLayer();
  virtual void endLayer();
  virtual void startChip();
  virtual void endChip();
  virtual void startCell();
  virtual void endCell();

private:
  std::unique_ptr<IO::LCWriter>      m_LCWriter{nullptr};
  std::unique_ptr<IMPL::LCEventImpl> m_LCEvent{nullptr};
  IMPL::LCCollectionVec*             m_CollectionVec{nullptr};
  std::string                        m_DetectorName{"Prototype"};
  std::string                        m_Filename;
};
