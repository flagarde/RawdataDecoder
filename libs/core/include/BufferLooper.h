/** \file BufferLooper.h
*  \copyright 2022 F.Lagarde
*/

#pragma once

#include "AppVersion.h"
#include "Data.h"
#include "Formatters.h"
#include "Timer.h"
#include "VersionInfos.h"

#include <algorithm>
#include <cassert>
#include <fmt/color.h>
#include <map>
#include <memory>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>
// function to loop on buffers

template<typename SOURCE, typename DESTINATION> void diff(SOURCE& m_Source, DESTINATION& m_Destination)
{
  Data data(m_Source.getBuffer());

  if(data.empty()) return;

  //std::cout<<to_hex(data.getBuffer())<<std::endl;

  // DIF
  m_Source.startDIF();
  m_Destination.startDIF();
  //
  m_Destination.processDIF(data);

  for(std::size_t column = 0; column != data.getChip().getNumberColumns(); ++column)
  {
    //
    m_Source.startChip();
    m_Destination.startChip();
    //
    m_Destination.processChip(data, 0);
    for(std::size_t j = 0; j != data.getChip().getNumberChannels(); ++j)
    {
      m_Source.startCell();
      m_Destination.startCell();
      m_Destination.processCell(data, column, j);
      m_Source.endCell();
      m_Destination.endCell();
    }
    //
    m_Source.endChip();
    m_Destination.endChip();
    //
  }
  //
  m_Source.endDIF();
  m_Destination.endDIF();
  //
}

template<typename SOURCE, typename DESTINATION> class BufferLooper
{
public:
  BufferLooper(SOURCE& source, DESTINATION& dest) : m_Source(source), m_Destination(dest)
  {
    m_Logger = spdlog::create<spdlog::sinks::null_sink_mt>("streamout");
    if(!spdlog::get("streamout")) { spdlog::register_logger(m_Logger); }
    m_Source.setLogger(m_Logger);
    m_Destination.setLogger(m_Logger);
  }

  void addSink(const spdlog::sink_ptr& sink, const spdlog::level::level_enum& level = spdlog::get_level())
  {
    sink->set_level(level);
    m_Sinks.push_back(sink);
    m_Logger = std::make_shared<spdlog::logger>("streamout", begin(m_Sinks), end(m_Sinks));
    m_Source.setLogger(m_Logger);
    m_Destination.setLogger(m_Logger);
  }

  void loop(const std::uint32_t& m_NbrEventsToProcess = std::numeric_limits<std::uint32_t>::max())
  {
    // clang-format off
    fmt::print(fg(fmt::color::medium_orchid) | fmt::emphasis::bold,
            "\n"
" SSSSSSSSSSSSSSS      tttt                                                                                                                                   tttt\n"
"SS:::::::::::::::S  ttt:::t                                                                                                                                ttt:::t\n"
"S:::::SSSSSS::::::S  t:::::t                                                                                                                                t:::::t\n"
"S:::::S     SSSSSSS  t:::::t                                                                                                                                t:::::t\n"
"S:::::S        ttttttt:::::ttttttt   rrrrr   rrrrrrrrr       eeeeeeeeeeee    aaaaaaaaaaaaa      mmmmmmm    mmmmmmm      ooooooooooo   uuuuuu    uuuuuuttttttt:::::ttttttt\n"
"S:::::S        t:::::::::::::::::t   r::::rrr:::::::::r    ee::::::::::::ee  a::::::::::::a   mm:::::::m  m:::::::mm  oo:::::::::::oo u::::u    u::::ut:::::::::::::::::t\n"
" S::::SSSS     t:::::::::::::::::t   r:::::::::::::::::r  e::::::eeeee:::::eeaaaaaaaaa:::::a m::::::::::mm::::::::::mo:::::::::::::::ou::::u    u::::ut:::::::::::::::::t\n"
"  SS::::::SSSSStttttt:::::::tttttt   rr::::::rrrrr::::::re::::::e     e:::::e         a::::a m::::::::::::::::::::::mo:::::ooooo:::::ou::::u    u::::utttttt:::::::tttttt\n"
"    SSS::::::::SS    t:::::t          r:::::r     r:::::re:::::::eeeee::::::e  aaaaaaa:::::a m:::::mmm::::::mmm:::::mo::::o     o::::ou::::u    u::::u      t:::::t\n"
"       SSSSSS::::S   t:::::t          r:::::r     rrrrrrre:::::::::::::::::e aa::::::::::::a m::::m   m::::m   m::::mo::::o     o::::ou::::u    u::::u      t:::::t\n"
"            S:::::S  t:::::t          r:::::r            e::::::eeeeeeeeeee a::::aaaa::::::a m::::m   m::::m   m::::mo::::o     o::::ou::::u    u::::u      t:::::t\n"
"            S:::::S  t:::::t    ttttttr:::::r            e:::::::e         a::::a    a:::::a m::::m   m::::m   m::::mo::::o     o::::ou:::::uuuu:::::u      t:::::t    tttttt\n"
"SSSSSSS     S:::::S  t::::::tttt:::::tr:::::r            e::::::::e        a::::a    a:::::a m::::m   m::::m   m::::mo:::::ooooo:::::ou:::::::::::::::uu    t::::::tttt:::::t\n"
"S::::::SSSSSS:::::S  tt::::::::::::::tr:::::r             e::::::::eeeeeeeea:::::aaaa::::::a m::::m   m::::m   m::::mo:::::::::::::::o u:::::::::::::::u    tt::::::::::::::t\n"
"S:::::::::::::::SS     tt:::::::::::ttr:::::r              ee:::::::::::::e a::::::::::aa:::am::::m   m::::m   m::::m oo:::::::::::oo   uu::::::::uu:::u      tt:::::::::::tt\n"
" SSSSSSSSSSSSSSS         ttttttttttt  rrrrrrr                eeeeeeeeeeeeee  aaaaaaaaaa  aaaammmmmm   mmmmmm   mmmmmm   ooooooooooo       uuuuuuuu  uuuu        ttttttttttt {}\n"
"\n",
fmt::format(fg(fmt::color::red) | fmt::emphasis::bold, "v{}", rawdatadecoder_version.to_string()));
    // clang-format on
    log()->info("*******************************************************************");
    log()->info("Streamout Version : {}", rawdatadecoder_version.to_string());
    log()->info("Using InterfaceReader {} version {}", m_Source.getName(), m_Source.getVersion().to_string());
    log()->info("Using InterfaceWriter {} version {}", m_Destination.getName(), m_Destination.getVersion().to_string());

    VersionInfos version;
    version.setLibraryInfos("streamout", rawdatadecoder_version);
    version.setReaderInfos(m_Source.getName(), m_Source.getVersion());
    version.setWriterInfos(m_Destination.getName(), m_Destination.getVersion());

    if(!m_Destination.checkCompatibility(m_Source.getName(), m_Source.getVersion().to_string()))
    {
      log()->critical("{} version {} is not compatible with {} version {} ! ", m_Source.getName(), m_Source.getVersion().to_string(), m_Destination.getName(), m_Destination.getVersion().to_string());
      log()->info("Compatible Interfaces for {} are", m_Destination.getName());
      for(std::map<std::string, std::string>::iterator it = m_Destination.getCompatibility().begin(); it != m_Destination.getCompatibility().end(); ++it) { log()->info("{} version {}", it->first, it->second); }
      std::exit(-1);
    }
    log()->info("*******************************************************************");
    Timer timer;
    timer.start();
    m_Source.start(version);
    m_Destination.start(version);
    while(m_Source.nextEvent() && m_NbrEventsToProcess >= m_processedEvents)
    {
      Timer timerEvent;
      timerEvent.start();
      Buffer buffer = m_Source.getEventBuffer();
      if(m_NbrEventsToProcess == std::numeric_limits<std::uint32_t>::max()) m_Logger->info("Event {} size {:04.2f}kb ({:3.2f}%)", m_Source.getEventNumber(), (buffer.size() * sizeof(bit8_t)) * 1.0 / 1024, m_Source.relativePosition() * 100);
      else
        m_Logger->info("Event {} size {:04.2f}kb ({:3.2f}%)", m_Source.getEventNumber(), (buffer.size() * sizeof(bit8_t)) * 1.0 / 1024, m_processedEvents * 100.0 / m_NbrEventsToProcess);
      m_Destination.setEventNumber(m_Source.getEventNumber());
      // Start event
      m_Source.startEvent();
      m_Destination.startEvent();

      m_Destination.processHeader(buffer);

      bool eventEmpty{true};
      while(m_Source.nextDIFbuffer())
      {
        m_Destination.processDIF(m_Source.getBuffer());
        Data data(m_Source.getBuffer());
        if(data.empty())
        {
          log()->warn("Layer {} empty", data.getLayer());
          continue;
        }
        eventEmpty = false;
        // DIF
        m_Source.startDIF();
        m_Destination.startDIF();
        //

        //fmt::print("{} : number colum {} toto {}\n",data.dataSize(), data.dataSize()%73 , (data.dataSize()%73)*73+(data.dataSize()%73));
        for(std::size_t column = 0; column != data.getChip().getNumberColumns(); ++column)
        {
          //
          m_Source.startChip();
          m_Destination.startChip();
          //
          m_Destination.processChip(data, 0);
          for(std::size_t j = 0; j != data.getChip().getNumberChannels(); ++j)
          {
            m_Source.startCell();
            m_Destination.startCell();
            m_Destination.processCell(data, column, j);
            m_Source.endCell();
            m_Destination.endCell();
          }
          //
          m_Source.endChip();
          m_Destination.endChip();
          //
        }
        //
        m_Source.endDIF();
        m_Destination.endDIF();
      }
      ++m_processedEvents;
      if(eventEmpty == false)
      {
        m_Destination.processTrailer(buffer);
        m_Source.endEvent();
        m_Destination.endEvent();
      }
      else
      {
        log()->warn(fmt::format(fmt::fg(fmt::color::red), "Empty event !"));
        continue;
      }
      timerEvent.stop();
      m_Logger->info("{0:─^{2}} processed in {1}us", "", timerEvent.getElapsedTime(), fmt::format("Event {}", m_Source.getEventNumber()).size());
    }  // end of event while loop
    m_Destination.end();
    m_Source.end();
    timer.stop();
    fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "=== elapsed time {}ms ({}ms/event) ===\n", timer.getElapsedTime() / 1000, timer.getElapsedTime() / (1000 * m_processedEvents));
  }
  std::shared_ptr<spdlog::logger> log() { return m_Logger; }

private:
  std::shared_ptr<spdlog::logger> m_Logger{nullptr};
  std::vector<spdlog::sink_ptr>   m_Sinks;
  SOURCE&                         m_Source{nullptr};
  DESTINATION&                    m_Destination{nullptr};
  std::uint32_t                   m_processedEvents{1};
};
