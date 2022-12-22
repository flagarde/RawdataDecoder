#include "InfosFromFilename.h"

#include "Filesystem.h"

#include <iomanip>

std::uint32_t extractRunNumber(const std::string& file)
{
  std::string name      = filename(file);
  std::size_t beginning = name.find_first_of("_Run") + 4;
  name                  = name.substr(beginning);
  beginning             = name.find_first_of("_");
  return static_cast<std::uint32_t>(stoi(name.substr(0, beginning)));
}

std::time_t extractTimestamp(const std::string& file)
{
  std::string name      = filename(file);
  std::size_t beginning = name.find_first_of("_Run") + 4;
  name                  = name.substr(beginning);
  beginning             = name.find_first_of("_");
  std::istringstream ss(name.substr(beginning + 1));
  std::tm            t{0};
  if(ss >> std::get_time(&t, "%Y%m%d_%H%M%S")) { return std::mktime(&t); }
  else { return 0; }
}
