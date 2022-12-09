/** \file RawdataReader.cc
*  \copyright 2022 F.Lagarde
*/
#include "RawdataReader.h"

#include <cstring>

std::size_t RawdataReader::m_BufferSize = 0x100000;

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
    throw;
  }
}

void RawdataReader::openFile(const std::string& fileName)
{
  try
  {
    m_FileStream.rdbuf()->pubsetbuf(0, 0);
    m_FileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    m_FileStream.open(fileName.c_str(), std::ios::in | std::ios::binary | std::ios::ate);  // Start at the end to directly calculate the size of the file then come back to beginning
    m_FileStream.rdbuf()->pubsetbuf(0, 0);
    if(m_FileStream.is_open())
    {
      setFileSize(m_FileStream.tellg());
      m_FileStream.seekg(0, std::ios::beg);
    }
  }
  catch(const std::ios_base::failure& e)
  {
    log()->error("Caught an ios_base::failure in openFile : {}", e.what());
    throw;
  }
}

bool RawdataReader::nextEvent()
{
  try
  {
    return !m_FileStream.eof();
  }
  catch(const std::ios_base::failure& e)
  {
    return false;
  }
  return true;
}

bool RawdataReader::nextDIFbuffer()
{
  try
  {
    static std::int32_t trigger_ID{-1};
    bool                b_begin = 0;
    bool                b_end   = 0;
    bit8_t              buffer  = 0;
    while(!b_begin && m_FileStream.read(reinterpret_cast<char*>(&buffer), sizeof(char)))
    {
      m_buf.push_back(buffer);
      if(m_buf.size() > 4) m_buf.erase(m_buf.begin(), m_buf.begin() + m_buf.size() - 4);
      if(m_buf[0] == 0xfa && m_buf[1] == 0x5a && m_buf[2] == 0xfa && m_buf[3] == 0x5a && m_buf.size() == 4) b_begin = 1;
    }
    while(!b_end && m_FileStream.read((char*)(&buffer), 1))
    {
      m_buf.push_back(buffer);
      int int_tmp = m_buf.size();
      if(int_tmp >= 4 && m_buf[int_tmp - 2] == 0xfe && m_buf[int_tmp - 1] == 0xee && m_buf[int_tmp - 4] == 0xfe && m_buf[int_tmp - 3] == 0xee) b_end = 1;
    }
    m_FileStream.read(reinterpret_cast<char*>(&buffer), sizeof(char));

    if(buffer != 0xff)
    {
      m_buf.clear();
      throw 0;
    }
    m_buf.push_back(buffer);
    m_FileStream.read(reinterpret_cast<char*>(&buffer), sizeof(char));
    if(buffer < 0 || buffer > 39)
    {
      m_buf.clear();
      throw 0;
    }
    m_buf.push_back(buffer);
    if(trigger_ID == -1) trigger_ID = m_buf[8] * 0x100 + m_buf[9];
    if(m_buf[8] * 0x100 + m_buf[9] == trigger_ID) return true;
    else
    {
      trigger_ID = m_buf[8] * 0x100 + m_buf[9];
      return false;
    }
  }
  catch(const std::ios_base::failure& e)
  {
    log()->error("Caught an ios_base::failure in openFile : {}", e.what());
    return false;
  }
  return true;
}

Buffer RawdataReader::getBuffer() { return m_buf; }

void RawdataReader::setFileSize(const std::size_t& size) { m_FileSize = size; }

float RawdataReader::getFileSize() { return m_FileSize; }