#pragma once

#include <cstdint>
#include <string>

/* Helper class to encode, decode CellID
 * Same for ROOT and LCIO
 * For ROOT CellID is encoded as : layer(8bits) + chip_id (8bits) + memory (8bits) + channel (8bits) for simplicity and to allow bad chip_id (>=10) to be store
 * Do CellID myCellID();
 * myCellID().set(layer,chip,memory,channel);
 * myCellID().getCellID();
 *
 * For LCIO you need to use the CellIDEncoder, This class provided the string to encode CellID0 CellID1:
 * DO :
 * UTIL::CellIDEncoder<IMPL::RawCalorimeterHitImpl> cd(CellID::getCellIDEncoderString(), m_CollectionVec);
 * then you can use LCIO facilities :
 * cd["BCID"]    = d.getChip(chip).getBCIDs(memory);
 * cd["gain"]    = d.getChip(chip).getCharge(memory, channel).gain();
 * cd["hit"]     = d.getChip(chip).getCharge(memory, channel).hit();
 * cd["layer"]   = d.getLayer();
 * cd["chip"]    = d.getChip(chip).getID();
 * cd["channel"] = channel;
 *
 *
 * */

class CellID
{
public:
  CellID() = default;
  explicit CellID(const std::uint32_t&);
  CellID(const std::uint8_t& layer, const std::uint8_t& chip, const std::uint8_t& memory, const std::uint8_t& channel);
  void               set(const std::uint8_t& layer, const std::uint8_t& chip, const std::uint8_t& memory, const std::uint8_t& channel);
  int                getLayerID();
  int                getChipID();
  int                getMemory();
  int                getChannel();
  std::uint32_t      getCellID();
  static std::string getCellIDEncoderString();

private:
  std::uint32_t      m_cellID{0};
  static std::string m_LCIOCellIDEncoderString;
};
