#pragma once
#include <cstdint>

class Buffer;

class Header
{
public:
  Header(const Buffer&);
  std::uint8_t  getCherenkov();
  std::uint32_t getTimestamp();

private:
  std::uint32_t m_header{0};
};
