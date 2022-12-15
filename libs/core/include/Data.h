#pragma once

#include "Buffer.h"
#include "Formatters.h"

#include <array>
#include <iostream>
class Chip
{
private:
  std::uint16_t                              m_chipID{0};
  std::uint16_t                              m_BCID{0};
  static constexpr std::size_t               m_numberChannel{36};
  std::array<std::uint16_t, m_numberChannel> m_charges;
  std::array<std::uint16_t, m_numberChannel> m_times;

public:
  Chip(const std::uint16_t& id, const std::uint16_t& bcid, const std::array<std::uint16_t, 36>& charges, const std::array<std::uint16_t, 36>& times) : m_chipID(id), m_BCID{bcid}, m_charges(charges), m_times(times) {}
  static constexpr std::size_t NumberChannels() { return m_numberChannel; }
  std::uint16_t                chipID() { return m_chipID; }
  std::uint16_t                BCID() { return m_BCID; }
  bool                         gain(const std::size_t& i) { return m_charges[i] & 0x2000; }
  bool                         hit(const std::size_t& i) { return m_charges[i] & 0x1000; }
  std::uint16_t                adc(const std::size_t& i) { return m_charges[i] & 0x0fff; }
  std::uint16_t                tdc(const std::size_t& i) { return m_times[i] & 0x0fff; }
};

class Data
{
public:
  Data(const Buffer& buffer) : m_Buffer(buffer)
  {
    if(m_Buffer.size() >= 42 && m_Buffer[m_Buffer.size() - 42] == 0xf0 && m_Buffer[m_Buffer.size() - 41] == 0x01 && m_Buffer[m_Buffer.size() - 8] == 0xf0 && m_Buffer[m_Buffer.size() - 7] == 0x02) m_isECAL = true;
    else
      m_isECAL = false;

    //if(m_Buffer.size() ==14) std::cout<<to_hex(m_Buffer)<<std::endl;

    //if(m_isECAL)
    //std::cout<<"ECAL :"<<m_isECAL<<"   "<< m_Buffer.size() <<"     "<<m_Buffer.size() - m_begin_chip_data_ECAL - m_end_chip_data_ECAL<<std::endl;
    //else std::cout<<"HCAL :"<<m_isECAL<<"   "<< m_Buffer.size() <<"     "<<m_Buffer.size() - m_begin_chip_data - m_end_chip_data<<std::endl;

    if(m_isECAL && m_Buffer.size() >= m_begin_chip_data_ECAL - m_end_chip_data_ECAL) m_Data = Buffer(&m_Buffer[m_begin_chip_data_ECAL], m_Buffer.size() - m_begin_chip_data_ECAL - m_end_chip_data_ECAL);
    else if(m_Buffer.size() >= m_end_chip_data - m_begin_chip_data)
      m_Data = Buffer(&m_Buffer[m_begin_chip_data], m_Buffer.size() - m_end_chip_data - m_begin_chip_data);
    m_Layer     = buffer[buffer.size() - 1];
    m_triggerid = m_isECAL ? m_Buffer[m_Buffer.size() - 43] * 0x100 + m_Buffer[m_Buffer.size() - 44] : (m_Buffer[8] & 0xff) * 256 + (m_Buffer[9] & 0xff);
    m_cycleID   = ((m_Buffer[4] & 0xff) * 256 + (m_Buffer[5] & 0xff)) * 0x10000 + ((m_Buffer[6] & 0xff) * 256 + (m_Buffer[7] & 0xff));
    createChips();
  }
  std::uint32_t getCycleID() const { return m_cycleID; }
  std::uint16_t getDetectorID() const { return m_detectorID; }
  std::uint32_t getTriggerID() const { return m_triggerid; }
  std::uint16_t getLayer() const { return m_Layer; }
  bool          empty() const
  {
    if(m_isECAL) return (m_Buffer.size() - m_begin_chip_data_ECAL - m_end_chip_data_ECAL + 2 == 0);
    else
      return (m_Buffer.size() - m_begin_chip_data - m_end_chip_data == 0);
  }
  std::vector<Chip> getChips() const { return m_chips; }

  Chip getChip(const std::size_t& i) const { return m_chips[i]; }

  std::size_t getNumberChips() const { return m_chips.size(); }
  std::size_t dataSize() const { return m_Data.size(); }
  std::size_t getNumberOfFrames() const { return 1; }
  Buffer      getData() const { return m_Data; }
  Buffer      getBuffer() const { return m_Buffer; }

private:
  void createChips()
  {
    std::size_t i{0};
    while(i != m_Data.size())
    {
      static int                                        k = 0;
      static std::uint16_t                              m_BCID{0};
      static std::uint16_t                              m_ID{0};
      std::array<std::uint16_t, Chip::NumberChannels()> m_charges;
      std::array<std::uint16_t, Chip::NumberChannels()> m_times;
      if(k < 36) { m_charges[35 - k] = m_Data[i] * 0x100 + m_Data[i + 1]; }
      else if(k < 71) { m_times[35 - (k - 36)] = m_Data[i] * 0x100 + m_Data[i + 1]; }
      else if(k == 72) { m_ID = m_Data[i] * 0x100 + m_Data[i + 1]; }
      else if(k == 71) { m_BCID = m_Data[i] * 0x100 + m_Data[i + 1]; }

      if(i % (74 * 2) == 0)
      {
        m_chips.push_back(Chip(m_ID, m_BCID, m_charges, m_times));
        k = 0;
      }
      else
        ++k;
      i = i + 2;
    }
  }
  bool                               m_isECAL{false};
  const static constexpr std::size_t m_begin_chip_data{10};     /* header (4) + Cycle (4) + triggerID (2) */
  const static constexpr std::size_t m_begin_chip_data_ECAL{8}; /* header (4) + Cycle (4) */
  const static constexpr std::size_t m_end_chip_data{6};        /* layer(1) + 0xff (1) + trailer (4) */
  const static constexpr std::size_t m_end_chip_data_ECAL{44};  /* layer(1) + 0xff (1) + trailer (4) + 0x02 (1) + 0xf0 (1) + 2*16 (32) + 0x01 (1) + 0xf1 (1) + triggerID (2)*/
  std::uint16_t                      m_Layer{0};
  std::uint32_t                      m_cycleID{0};
  std::uint16_t                      m_detectorID{0};
  std::uint32_t                      m_triggerid{0};
  Buffer                             m_Buffer;
  Buffer                             m_Data;
  std::vector<Chip>                  m_chips;
};
