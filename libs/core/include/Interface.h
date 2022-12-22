/** \file Interface.h
*  \copyright 2022 G.Grenier F.Lagarde
*/
#pragma once

#include "AppVersion.h"
#include "Buffer.h"
#include "Version.h"
#include "VersionInfos.h"

#include <ctime>
#include <map>
#include <memory>
#include <semver.hpp>
#include <spdlog/logger.h>
#include <string>

/**
* template class should implement
* void SOURCE::start();
* bool SOURCE::next();
* void SOURCE::end();
* const Buffer& SOURCE::getBuffer();
*
* void DESTINATION::begin();
* void DESTINATION::processDIF(const DIFPtr&);
* void DESTINATION::processFrame(const DIFPtr&,const std::uint32_t& frameIndex);
* void DESTINATION::processPadInFrame(const DIFPtr&,const std::uint32_t& frameIndex,const std::uint32_t& channelIndex);
* void DESTINATION::processSlowControl(const Buffer&);
* void DESTINATION::end();
**/

enum class InterfaceType
{
  Unknown = 0,
  Reader  = 1,
  Writer  = 2
};

class Interface
{
public:
  Interface() = default;
  Interface(const std::string& name, const std::string& version, const InterfaceType& type) : m_Name(name), m_Version(version), m_Type(type) {}
  virtual ~Interface() = default;
  virtual void                     start(const VersionInfos& ver) {}
  virtual void                     end() {}
  // Event
  virtual void                     startEvent() {}
  virtual void                     endEvent() {}
  //
  // Layer
  virtual void                     startDIF() {}
  virtual void                     endDIF() {}
  //
  // Chip
  virtual void                     startChip() {}
  virtual void                     endChip() {}
  //
  // Cell
  virtual void                     startCell() {}
  virtual void                     endCell() {}
  //
  std::shared_ptr<spdlog::logger>& log() { return m_Logger; }
  void                             setLogger(const std::shared_ptr<spdlog::logger>& logger) { m_Logger = logger; }
  std::string                      getName() { return m_Name; }
  Version                          getVersion() { return m_Version; }
  std::uint32_t                    getEventNumber() { return m_EventNumber; }
  void                             setEventNumber(const std::uint32_t& nbr) { m_EventNumber = nbr; }
  void                             addToEventNumber(const std::uint32_t& nbr) { m_EventNumber += nbr; }
  std::uint32_t                    getRunNumber() { return m_RunNumber; }
  void                             setRunNumber(const std::uint32_t& nbr) { m_RunNumber = nbr; }
  virtual void                     setTime(const std::time_t& time) { m_Time = time; }
  std::time_t                      getTime() { return m_Time; }
  InterfaceType                    getType() { return m_Type; }

private:
  std::uint32_t                   m_EventNumber{0};
  std::uint32_t                   m_RunNumber{0};
  std::time_t                     m_Time{0};
  std::shared_ptr<spdlog::logger> m_Logger{nullptr};
  std::string                     m_Name;
  Version                         m_Version;
  InterfaceType                   m_Type{InterfaceType::Unknown};
};

class InterfaceReader : public Interface
{
public:
  InterfaceReader() = default;
  InterfaceReader(const std::string& name, const std::string& version) : Interface(name, version, InterfaceType::Reader) {}
  virtual ~InterfaceReader() = default;

protected:
  Buffer m_Buffer;
};

class InterfaceWriter : public Interface
{
public:
  InterfaceWriter() = default;
  InterfaceWriter(const std::string& name, const std::string& version) : Interface(name, version, InterfaceType::Writer) {}
  void addCompatibility(const std::string& name, const std::string& version) { m_Compatible[name] = version; }
  std::map<std::string, std::string> getCompatibility() { return m_Compatible; }
  bool checkCompatibility(const std::string& name, const std::string& version)
  {
    if(m_Compatible.find(name) != m_Compatible.end())
    {
      auto            ran = semver::range::detail::range(m_Compatible[name]);
      semver::version ver = semver::version(version);
      if(ran.satisfies(ver, false)) { return true; }
      else
        return false;
    }
    else
      return false;
  }
  virtual ~InterfaceWriter() = default;
  virtual void processHeader(const Buffer&){};
  virtual void processDIF(const Buffer&){};
  virtual void processChip(const Buffer&, const std::uint32_t& frameIndex){};
  virtual void processCell(const Buffer&, const std::uint32_t& frameIndex, const std::uint32_t& channelIndex){};
  virtual void processTrailer(const Buffer&){};

private:
  std::map<std::string, std::string> m_Compatible;
};
