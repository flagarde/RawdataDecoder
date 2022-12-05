/**
*  \file Bits.h
*  \copyright 2022 G.Grenier F.Lagarde
*/
#pragma once

#include <cstdint>
#include <iosfwd>

using bit8_t  = std::uint8_t;  /*<! type to represent 8bits words (1 byte) */
using bit16_t = std::uint16_t; /*<! type to represent 16bits words (2 bytes) */
using bit32_t = std::uint32_t; /*<! type to represent 32bits words (4 bytes) */
using bit64_t = std::uint64_t; /*<! type to represent 64bits words (8 bytes) */

/** Stream operator to print bit8_t aka std::uint8_t and not char or unsigned char */
std::ostream& operator<<(std::ostream& os, const bit8_t& c);
