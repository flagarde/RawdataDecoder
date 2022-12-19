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
  /******************/
  /*** START DIF ***/
  m_Source.startDIF();
  m_Destination.startDIF();

  /******************/
  m_Destination.processDIF(data);

  for(std::size_t column = 0; column != data.getChip().getNumberColumns(); ++column)
  {
    //
    m_Source.startFrame();
    m_Destination.startFrame();
    //
    m_Destination.processFrame(data, 0);
    for(std::size_t j = 0; j != data.getChip().getNumberChannels(); ++j)
    {
      m_Source.startPad();
      m_Destination.startPad();
      m_Destination.processPadInFrame(data, column, j);
      m_Source.endPad();
      m_Destination.endPad();
    }
    //
    m_Source.endFrame();
    m_Destination.endFrame();
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
  BufferLooper(SOURCE& source, DESTINATION& dest, bool debug = false) : m_Source(source), m_Destination(dest), m_Debug(debug)
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

  void loop(const std::uint32_t& m_NbrEventsToProcess = 0)
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
    while(m_Source.nextEvent() && m_NbrEventsToProcess >= m_NbrEvents)
    {
      m_Destination.setEventNumber(m_Source.getEventNumber());
      /*******************/
      /*** START EVENT ***/
      m_Source.startEvent();
      m_Destination.startEvent();
      /*******************/

      m_Logger->warn("===*** Event {} ***===", m_Destination.getEventNumber());

      //Special case if we are in a new event the buffer has already been read
      if(m_Source.getBuffer().size() != 0) { diff(m_Source, m_Destination); }
      //std::cout<<"First Event "<<m_NbrEvents<<" "<<to_hex(a.getBuffer()[0])<<std::endl;
      while(m_Source.nextDIFbuffer()) { diff(m_Source, m_Destination); }  // end of DIF while loop
      m_NbrEvents++;
      /*****************/
      /*** END EVENT ***/
      m_Source.endEvent();
      m_Destination.endEvent();
      /*****************/
    }  // end of event while loop
    m_Destination.end();
    m_Source.end();
    timer.stop();
    fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "=== elapsed time {}ms ({}ms/event) ===\n", timer.getElapsedTime() / 1000, timer.getElapsedTime() / (1000 * m_NbrEvents));
  }
  std::shared_ptr<spdlog::logger> log() { return m_Logger; }

private:
  std::shared_ptr<spdlog::logger> m_Logger{nullptr};
  std::vector<spdlog::sink_ptr>   m_Sinks;
  SOURCE&                         m_Source{nullptr};
  DESTINATION&                    m_Destination{nullptr};
  bool                            m_Debug{false};
  std::uint32_t                   m_NbrEvents{1};
};
