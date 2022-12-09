/** \file Filesystem.cc
*  \copyright 2022 F.Lagarde
*/

#include "Filesystem.h"

std::string path(const std::string& file)
{
  std::size_t pos = file.find_last_of("\\/");
  return (std::string::npos == pos) ? "" : file.substr(0, pos);
}

std::string extension(const std::string& file)
{
  std::size_t position = file.find_last_of(".");
  return (std::string::npos == position || position == 0) ? "" : file.substr(position + 1);
}

std::string filename(const std::string& file)
{
  std::size_t position = file.find_last_of(".");
  std::size_t pos      = file.find_last_of("\\/");
  return (std::string::npos == pos) ? file.substr(0, position) : file.substr(pos + 1, position - pos - 1);
}
