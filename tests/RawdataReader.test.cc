/**
*  \file RawdataReader.test.cc
*  \copyright 2022 F.Lagarde
*/

#include "RawdataReader.h"

#include "Buffer.h"
#include "Data.h"
#include "Formatters.h"
#include "fmt/color.h"
//#include <doctest/doctest.h>
#include <iostream>

int main()
{
  VersionInfos  k;
  RawdataReader a("/home/travel/Bureau/AHCAL_Run143_20221024_072149.dat");
  a.start(k);
  while(a.nextEvent())
  {
    static std::uint32_t m_NbrEvents{1};
    a.startEvent();
    fmt::print(fmt::fg(fmt::color::red), "Event {}\n", m_NbrEvents);
    //Special case if we are in a new event the buffer has already been read
    if(a.getBuffer().size() != 0)
    {
      a.startDIF();
      Data data(a.getBuffer());
      fmt::print(fmt::fg(fmt::color::red), "TriggerID {}, layer {}, CycleID {} Empty {} \n", data.getTriggerID(), data.getLayer(), data.getCycleID(), data.empty());
      fmt::print(fmt::fg(fmt::color::red), "Buffer {}", to_hex(a.getBuffer()));
    }
    //std::cout<<"First Event "<<m_NbrEvents<<" "<<to_hex(a.getBuffer()[0])<<std::endl;
    while(a.nextDIFbuffer())
    {
      const Buffer& buffer = a.getBuffer();
      a.startDIF();
      Data data(a.getBuffer());
      fmt::print(fmt::fg(fmt::color::red), "TriggerID {}, layer {}, CycleID {} Empty {} \n", data.getTriggerID(), data.getLayer(), data.getCycleID(), data.empty());
      fmt::print(fmt::fg(fmt::color::red), "Buffer {}", to_hex(a.getBuffer()));
      fmt::print(fmt::fg(fmt::color::magenta), "data size : {}", data.dataSize());
    }
    m_NbrEvents++;
  }
}
