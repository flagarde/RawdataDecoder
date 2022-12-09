/**
*  \file Formatters.cc
*  \copyright 2022 G.Grenier F.Lagarde
*/

#include "Formatters.h"

#include "Bits.h"
#include "Buffer.h"

#include <fmt/format.h>

std::string to_dec(const Buffer& b, const std::size_t& begin, const std::size_t& end)
{
  std::size_t iend = end;
  if(iend == -1) iend = b.size();
  std::string ret;
  for(std::size_t k = begin; k < iend; k++)
  {
    ret += to_dec(b[k]);
    ret += " - ";
  }
  return ret;
}

std::string to_dec(const bit8_t& b) { return fmt::format("{:#d}", b); }

std::string to_dec(const bit16_t& b) { return fmt::format("{:#d}", b); }

std::string to_dec(const bit32_t& b) { return fmt::format("{:#d}", b); }

std::string to_dec(const bit64_t& b) { return fmt::format("{:#d}", b); }

std::string to_hex(const Buffer& b, const std::size_t& begin, const std::size_t& end)
{
  std::size_t iend = end;
  if(iend == -1) iend = b.size();
  std::string ret;
  for(std::size_t k = begin; k < iend; k++)
  {
    ret += to_hex(b[k]);
    ret += " - ";
  }
  return ret;
}

std::string to_hex(const bit8_t& b) { return fmt::format("{:#02x}", b); }

std::string to_hex(bit16_t& b) { return fmt::format("{:#04x}", b); }

std::string to_hex(const bit32_t& b) { return fmt::format("{:#08x}", b); }

std::string to_hex(const bit64_t& b) { return fmt::format("{:#016x}", b); }

std::string to_bin(const Buffer& b, const std::size_t& begin, const std::size_t& end)
{
  std::size_t iend = end;
  if(iend == -1) iend = b.size();
  std::string ret;
  for(std::size_t k = begin; k < iend; k++)
  {
    ret += to_bin(b[k]);
    ret += " - ";
  }
  return ret;
}

std::string to_bin(const bit8_t& b) { return fmt::format("{:#08b}", b); }

std::string to_bin(const bit16_t& b) { return fmt::format("{:#016b}", b); }

std::string to_bin(const bit32_t& b) { return fmt::format("{:#032b}", b); }

std::string to_bin(const bit64_t& b) { return fmt::format("{:#064b}", b); }

std::string to_oct(const Buffer& b, const std::size_t& begin, const std::size_t& end)
{
  std::size_t iend = end;
  if(iend == -1) iend = b.size();
  std::string ret;
  for(std::size_t k = begin; k < iend; k++)
  {
    ret += to_oct(b[k]);
    ret += " - ";
  }
  return ret;
}

std::string to_oct(const bit8_t& b) { return fmt::format("{:#04o}", b); }

std::string to_oct(const bit16_t& b) { return fmt::format("{:#08o}", b); }

std::string to_oct(const bit32_t& b) { return fmt::format("{:#016o}", b); }

std::string to_oct(const bit64_t& b) { return fmt::format("{:#032o}", b); }
