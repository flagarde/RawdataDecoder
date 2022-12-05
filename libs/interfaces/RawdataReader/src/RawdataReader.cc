/** \file RawdataReader.cc
*  \copyright 2022 G.Grenier F.Lagarde
*/
#include "RawdataReader.h"

#include "Exception.h"

#include <cstring>
#include <stdexcept>

std::size_t RawdataReader::m_BufferSize = 0x100000;

void RawdataReader::setDefaultBufferSize(const std::size_t& size) { m_BufferSize = size; }

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
    m_FileStream.read(reinterpret_cast<char*>(&m_EventNumber), sizeof(std::uint32_t));
    m_FileStream.read(reinterpret_cast<char*>(&m_NumberOfDIF), sizeof(std::uint32_t));
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
    static int DIF_processed{0};
    if(DIF_processed >= m_NumberOfDIF)
    {
      DIF_processed = 0;
      return false;
    }
    else
    {
      DIF_processed++;
      std::uint32_t bsize{0};
      m_FileStream.read(reinterpret_cast<char*>(&bsize), sizeof(std::uint32_t));
      m_FileStream.read(reinterpret_cast<char*>(&m_buf[0]), bsize);
      m_Buffer = Buffer(m_buf);
    }
  }
  catch(const std::ios_base::failure& e)
  {
    log()->error("Caught an ios_base::failure in openFile : {}", e.what());
    return false;
  }
  return true;
}

const Buffer& RawdataReader::getBuffer() { return m_Buffer; }

void RawdataReader::setFileSize(const std::size_t& size) { m_FileSize = size; }

float RawdataReader::getFileSize() { return m_FileSize; }
