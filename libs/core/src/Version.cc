/** \file Version.cc
*  \copyright 2022 G.Grenier F.Lagarde
*/

#include "Version.h"

const static Version streamout_version;

std::uint8_t Version::getMajor() { return major; }

std::uint8_t Version::getMinor() { return minor; }

std::uint8_t Version::getPatch() { return patch; }

std::string Version::getPreRelease()
{
  switch(prerelease_type)
  {
    case semver::prerelease::alpha: return "alpha";
    case semver::prerelease::beta: return "beta";
    case semver::prerelease::rc: return "rc";
    case semver::prerelease::none: return "";
    default: return "";
  }
}

std::uint8_t Version::getPreReleaseNumber() { return prerelease_number; }
