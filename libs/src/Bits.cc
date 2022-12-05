/**
*  \file Bits.cc
*  \copyright 2022 G.Grenier F.Lagarde
*/

#include "Bits.h"

std::ostream& operator<<(std::ostream& os, const bit8_t& c) { return os << c + 0; }
