#pragma once

#include "Buffer.h"
#include "Exception.h"
#include "Formatters.h"
#include "fmt/format.h"

#include <array>
#include <iostream>

enum class DetectorID
{
  Unkown = -1,
  ECAL   = 0,
  HCAL   = 1
};

class Time
{
public:
  Time() {}
  Time(const std::uint16_t& data) : m_Data(data) {}
  bool          hit() { return m_Data & 0x1000; }
  bool          gain() { return m_Data & 0x2000; }
  std::uint16_t timestamp() { return m_Data & 0x0fff; }

private:
  std::uint16_t m_Data{0};
};

class Charge
{
public:
  Charge() {}
  Charge(const std::uint16_t& data) : m_Data(data) {}
  bool          hit() { return m_Data & 0x1000; }
  bool          gain() { return m_Data & 0x2000; }
  std::uint16_t charge() { return m_Data & 0x0fff; }

private:
  std::uint16_t m_Data{0};
};

class Chip
{
public:
  static constexpr std::size_t m_numberChannel{36};
  Chip() {}
  void          setID(const std::uint8_t& id) { m_chipID = id; }
  std::uint16_t getID() { return m_chipID; }
  void          reserveColumns(const std::size_t& columns)
  {
    m_BCIDs.reserve(columns);
    m_charges.reserve(columns);
    m_times.reserve(columns);
  }
  void                                addCharges(const std::array<Charge, Chip::m_numberChannel>& charges) { m_charges.push_back(charges); }
  void                                addTimes(const std::array<Time, Chip::m_numberChannel>& times) { m_times.push_back(times); }
  void                                addBCID(const std::uint16_t& bcid) { m_BCIDs.push_back(bcid); }
  std::size_t                         getNumberColumns() { return m_BCIDs.size(); }
  std::uint16_t                       getBCIDs(const std::size_t& column) { return m_BCIDs[column]; }
  std::array<Charge, m_numberChannel> getCharges(const std::size_t& column) { return m_charges[column]; }

  Charge getCharge(const std::size_t& column, const std::size_t& channel) { return m_charges[column][channel]; }

  std::array<Time, m_numberChannel> getTimes(const std::size_t& column) { return m_times[column]; }

  Time getTime(const std::size_t& column, const std::size_t& channel) { return m_times[column][channel]; }

  static constexpr std::size_t getNumberChannels() { return m_numberChannel; }
private:
  std::uint16_t                                    m_chipID{0};
  std::vector<std::uint16_t>                       m_BCIDs;
  std::vector<std::array<Charge, m_numberChannel>> m_charges;
  std::vector<std::array<Time, m_numberChannel>>   m_times;
};

class Data
{
public:
  Data(const Buffer& buffer) : m_Buffer(buffer)
  {
    m_Layer = buffer[buffer.size() - 1];
    if(m_Buffer.size() == 14 || m_Buffer.size() == 16) m_isEmpty = true;
    if(!empty())
    {
      if(m_Buffer.size() >= 42 && m_Buffer[m_Buffer.size() - 42] == 0xf0 && m_Buffer[m_Buffer.size() - 41] == 0x01 && m_Buffer[m_Buffer.size() - 8] == 0xf0 && m_Buffer[m_Buffer.size() - 7] == 0x02) m_detectoID = DetectorID::ECAL;
      else
        m_detectoID = DetectorID::HCAL;

      m_triggerid = m_detectoID == DetectorID::ECAL ? m_Buffer[m_Buffer.size() - 43] * 0x100 + m_Buffer[m_Buffer.size() - 44] : (m_Buffer[8] & 0xff) * 256 + (m_Buffer[9] & 0xff);

      m_cycleID = ((m_Buffer[4] & 0xff) * 256 + (m_Buffer[5] & 0xff)) * 0x10000 + ((m_Buffer[6] & 0xff) * 256 + (m_Buffer[7] & 0xff));

      if(m_detectoID == DetectorID::ECAL) m_Data = Buffer(&m_Buffer[m_begin_chip_data_ECAL], m_Buffer.size() - m_begin_chip_data_ECAL - m_end_chip_data_ECAL);
      else if(m_detectoID == DetectorID::HCAL)
        m_Data = Buffer(&m_Buffer[m_begin_chip_data], m_Buffer.size() - m_end_chip_data - m_begin_chip_data);
      else
        throw;

      createChips();
    }
  }
  std::uint32_t getCycleID() const { return m_cycleID; }
  std::uint32_t getTriggerID() const { return m_triggerid; }
  std::uint16_t getLayer() const { return m_Layer; }
  bool          empty() const { return m_isEmpty; }
  Chip          getChip() const { return m_chip; }

  std::size_t dataSize() const { return m_Data.size(); }
  Buffer      getData() const { return m_Data; }
  Buffer      getBuffer() const { return m_Buffer; }
  int         getDetectorID() const { return static_cast<int>(m_detectoID); }

private:
  void createChips()
  {
    m_chip.setID(m_Data[m_Data.size() - 1]);
    // Number of SCA columns = (size() - chipID(2)) % (( 36 charges + 36 times + BCID )*2)
    std::size_t nbrColumns = (m_Data.size() - 2) % (73 * 2);
    m_chip.reserveColumns(nbrColumns);

    static std::size_t caret{0};
    for(std::size_t column = 0; column != nbrColumns; ++column)
    {
      std::array<Charge, Chip::m_numberChannel> charges;
      for(std::size_t charge = Chip::m_numberChannel - 1; charge != 0; --charge)
      {
        charges[charge] = m_Data[caret] * 0x100 + m_Data[caret + 1];
        caret += 2;
      }
      m_chip.addCharges(charges);
      std::array<Time, Chip::m_numberChannel> times;
      for(std::size_t time = Chip::m_numberChannel - 1; time != 0; --time)
      {
        times[time] = m_Data[caret] * 0x100 + m_Data[caret + 1];
        caret += 2;
      }
      m_chip.addTimes(times);
    }
    for(std::size_t column = 0; column != nbrColumns; ++column)
    {
      m_chip.addBCID(m_Data[caret] * 0x100 + m_Data[caret + 1]);
      caret += 2;
    }
    //m_chips.push_back(chip);
    caret = 0;
  }
  DetectorID                         m_detectoID{DetectorID::Unkown};
  const static constexpr std::size_t m_begin_chip_data{10};     /* header (4) + Cycle (4) + triggerID (2) */
  const static constexpr std::size_t m_begin_chip_data_ECAL{8}; /* header (4) + Cycle (4) */
  const static constexpr std::size_t m_end_chip_data{6};        /* layer(1) + 0xff (1) + trailer (4) */
  const static constexpr std::size_t m_end_chip_data_ECAL{44};  /* layer(1) + 0xff (1) + trailer (4) + 0x02 (1) + 0xf0 (1) + 2*16 (32) + 0x01 (1) + 0xf1 (1) + triggerID (2)*/
  std::uint16_t                      m_Layer{0};
  std::uint32_t                      m_cycleID{0};
  std::uint32_t                      m_triggerid{0};
  Buffer                             m_Buffer;
  Buffer                             m_Data;
  Chip                               m_chip;
  bool                               m_isEmpty{false};
};
