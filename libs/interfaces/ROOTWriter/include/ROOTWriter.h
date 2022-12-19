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
  void processDIF(const Data&);
  void processFrame(const Data&, const std::uint32_t& frameIndex);
  void processPadInFrame(const Data&, const std::uint32_t& frameIndex, const std::uint32_t& channelIndex);
  void end();

  virtual void startEvent();
  virtual void endEvent();
  virtual void startDIF();
  virtual void endDIF();
  virtual void startFrame();
  virtual void endFrame();
  virtual void startPad();
  virtual void endPad();

private:
  TFile*              m_File{nullptr};
  TTree*              m_Tree{nullptr};
  std::string         m_Filename;
  /* branches variables */
  double              _cycleID;
  double              _triggerID;
  std::vector<int>    _buffer_v;
  std::vector<int>    _cellID;
  std::vector<int>    _bcid;
  std::vector<int>    _hitTag;
  std::vector<int>    _gainTag;
  std::vector<double> _charge;
  std::vector<double> _time;
};
