/** \file RawdataReader.cc
*  \copyright 2022 F.Lagarde
*/
#include "RawdataReader.h"

#include "Exception.h"
#include "Formatters.h"
#include "fmt/color.h"

#include <cstring>
#include <iostream>

std::size_t RawdataReader::m_BufferSize = 20000000;

Buffer RawdataReader::getEventBuffer() { return m_buf; }

RawdataReader::RawdataReader(const char* fileName) : InterfaceReader("RawdataReader", "1.0.0")
{
  m_buf.reserve(m_BufferSize);
  m_Filename = fileName;
}

void RawdataReader::start(const VersionInfos& ver) { openFile(m_Filename); }

void RawdataReader::end() { closeFile(); }

void RawdataReader::closeFile()
{
  try
  {
    if(m_FileStream.is_open()) m_FileStream.close();
  }
  catch(const std::ios_base::failure& e)
  {
    log()->error("Caught an ios_base::failure in closeFile : {} {}", e.what(), e.code().value());
    throw Exception(fmt::format("Caught an ios_base::failure in closeFile : {} {}", e.what(), e.code().value()));
  }
}

void RawdataReader::openFile(const std::string& fileName)
{
  try
  {
    //m_FileStream.rdbuf()->pubsetbuf(0, 0);
    m_FileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    m_FileStream.open(fileName.c_str(), std::ios::in | std::ios::binary | std::ios::ate);  // Start at the end to directly calculate the size of the file then come back to beginning
    //m_FileStream.rdbuf()->pubsetbuf(0, 0);
    if(m_FileStream.is_open())
    {
      setFileSize(m_FileStream.tellg());
      m_FileStream.seekg(0, std::ios::beg);
    }
  }
  catch(const std::ios_base::failure& e)
  {
    log()->error("Caught an ios_base::failure in openFile : {}", e.what());
    throw Exception(fmt::format("Caught an ios_base::failure in openFile : {}", e.what()));
  }
}

bool RawdataReader::nextEvent()
{
  try
  {
    m_buf.clear();
    bit8_t bits;
    do {
      m_FileStream.read(reinterpret_cast<char*>(&bits), sizeof(bit8_t));
      m_buf.push_back(bits);
    } while(m_buf.size() < 4 || m_buf[m_buf.size() - 4] != 0xfe || m_buf[m_buf.size() - 3] != 0xdd || m_buf[m_buf.size() - 2] != 0xfe || m_buf[m_buf.size() - 1] != 0xdd);
    addToEventNumber(1);
    m_ActualSize += m_buf.size();
    return true;
  }
  catch(const std::ios_base::failure& e)
  {
    return false;
  }
}

bool RawdataReader::nextDIFbuffer()
{
  try
  {
    static std::size_t pos{0};
    std::size_t        position{0};
    std::size_t        size{0};
    for(std::size_t i = pos; i != m_buf.size() - 4; ++i)
    {
      if(m_buf[i] == 0xfa && m_buf[i + 1] == 0x5a && m_buf[i + 2] == 0xfa && m_buf[i + 3] == 0x5a) position = i;
      if(m_buf[i] == 0xfe && m_buf[i + 1] == 0xee && m_buf[i + 2] == 0xfe && m_buf[i + 3] == 0xee)
      {
        size = i + 4;
        if(m_buf[i + 4] != 0xff)
        {
          log()->error("Layer ID is not of the form 0xff**, I got {} \n Skipping it !!!", m_buf[i + 4]);
          continue;
        }
        else
        {
          ++pos;
          ++pos;
          m_DIFbuffer = Buffer(&m_buf[position], size - position + 2);
          break;
        }
      }
      ++pos;
    }

    if(pos == m_buf.size() - 4)
    {
      pos = 0;
      return false;
    }
    else
      return true;
  }
  catch(const std::ios_base::failure& e)
  {
    log()->error("Caught an ios_base::failure in openFile : {}", e.what());
    return false;
  }
}

Buffer RawdataReader::getBuffer() { return m_DIFbuffer; }

void RawdataReader::setFileSize(const std::size_t& size) { m_FileSize = size; }

float RawdataReader::getFileSize() { return m_FileSize; }
