#include "CellID.h"

std::string CellID::m_LCIOCellIDEncoderString = {"BCID:16,gain:1,hit:1,layer:8,chip:8,memory:8,channel:8"};

std::string CellID::getCellIDEncoderString() { return m_LCIOCellIDEncoderString; }

CellID::CellID(const std::uint32_t& cellID) { m_cellID = cellID; }

std::uint32_t CellID::getCellID() { return m_cellID; }

CellID::CellID(const std::uint8_t& layer, const std::uint8_t& chip, const std::uint8_t& memory, const std::uint8_t& channel) { set(layer, chip, memory, channel); }

void CellID::set(const std::uint8_t& layer, const std::uint8_t& chip, const std::uint8_t& memory, const std::uint8_t& channel) { m_cellID = (layer << 24) + (chip << 16) + (memory << 8) + channel; }

int CellID::getLayerID() { return (m_cellID >> 24) & 0xFF; }

int CellID::getChipID() { return (m_cellID >> 16) & 0xFF; }

int CellID::getMemory() { return (m_cellID >> 8) & 0xFF; }

int CellID::getChannel() { return (m_cellID)&0xFF; }
