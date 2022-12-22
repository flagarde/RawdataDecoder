#include "Header.h"

#include "Buffer.h"

Header::Header(const Buffer& buffer)
{
  m_header = static_cast<std::uint32_t>(buffer[buffer.size() - 5]) + static_cast<std::uint32_t>(buffer[buffer.size() - 6] << 8) + static_cast<std::uint32_t>(buffer[buffer.size() - 7] << 16) + static_cast<std::uint32_t>(buffer[buffer.size() - 8] << 24);
}

std::uint8_t  Header::getCherenkov() { return (m_header >> 30) & 0b11; }
std::uint32_t Header::getTimestamp() { return m_header & 0x3FFFFFFF; }
