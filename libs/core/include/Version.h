/** \file Version.h
*  \copyright 2022 G.Grenier F.Lagarde
*/

#pragma once

#include <cstdint>
#include <semver.hpp>
#include <string>

class Version : public semver::version
{
public:
  Version(const std::uint8_t& mj, const std::uint8_t& mn, const std::uint8_t& pt, const semver::prerelease& prt = semver::prerelease::none, const std::uint8_t& prn = 0) noexcept : semver::version(mj, mn, pt, prt, prn) {}
  explicit Version(const std::string_view& str) : semver::version(str) {}
  Version() = default;
  std::uint8_t getMajor();
  std::uint8_t getMinor();
  std::uint8_t getPatch();
  std::string  getPreRelease();
  std::uint8_t getPreReleaseNumber();
};
