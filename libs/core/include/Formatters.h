/** \file Formatters.h
*  \copyright 2022 G.Grenier F.Lagarde
*/

#pragma once

#include "Bits.h"

#include <iosfwd>
#include <string>

class Buffer;

std::string to_dec(const Buffer& b, const std::size_t& begin = 0, const std::size_t& end = -1);
std::string to_dec(const bit8_t&);
std::string to_dec(const bit16_t&);
std::string to_dec(const bit32_t&);
std::string to_dec(const bit64_t&);

std::string to_hex(const Buffer& b, const std::size_t& begin = 0, const std::size_t& end = -1);
std::string to_hex(const bit8_t&);
std::string to_hex(bit16_t&);
std::string to_hex(const bit32_t&);
std::string to_hex(const bit64_t&);

std::string to_bin(const Buffer& b, const std::size_t& begin = 0, const std::size_t& end = -1);
std::string to_bin(const bit8_t&);
std::string to_bin(const bit16_t&);
std::string to_bin(const bit32_t&);
std::string to_bin(const bit64_t&);

std::string to_oct(const Buffer& b, const std::size_t& begin = 0, const std::size_t& end = -1);
std::string to_oct(const bit8_t&);
std::string to_oct(const bit16_t&);
std::string to_oct(const bit32_t&);
std::string to_oct(const bit64_t&);
