/**
*  \file ROOTWriter.h
*  \copyright 2022 F.Lagarde
*/

#pragma once

#include "Data.h"
#include "Interface.h"

#include <TFile.h>
#include <TTree.h>
#include <string>
#include <vector>

class ROOTWriter : public InterfaceWriter
{
public:
  ROOTWriter();
  void setFilename(const std::string&);
  void start(const VersionInfos& ver);

  virtual void processHeader(const Buffer&) final;
  void         processDIF(const Data&);
  void         processChip(const Data&, const std::uint32_t& frameIndex);
  void         processCell(const Data&, const std::uint32_t& frameIndex, const std::uint32_t& channelIndex);
  void         end();

  virtual void startEvent();
  virtual void endEvent();
  virtual void startDIF();
  virtual void endDIF();
  virtual void startChip();
  virtual void endChip();
  virtual void startCell();
  virtual void endCell();

private:
  TFile*             m_File{nullptr};
  TTree*             m_Tree{nullptr};
  std::string        m_Filename;
  /* branches variables */
  int                m_runNumber{0};
  int                m_eventTime{0};
  int                m_eventNumber{0};
  int                m_detectorID{0};
  int                m_cherenkov{0};
  double             m_cycleID{0};
  double             m_triggerID{0};
  std::vector<int>   m_cellID;
  std::vector<int>   m_bcid;
  std::vector<int>   m_hitTag;
  std::vector<int>   m_gainTag;
  std::vector<float> m_charge;
  std::vector<float> m_time;
};
