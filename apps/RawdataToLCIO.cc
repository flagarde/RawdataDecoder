#include "BufferLooper.h"
#include "CLI/CLI.hpp"
#include "Filesystem.h"
#include "LCIOWriter.h"
#include "RawdataReader.h"

#include <limits>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

int main(int argc, char** argv)
{
  CLI::App app{"Rawdata to LCIO writer"};
  app.set_version_flag("--version", rawdatadecoder_version.to_string());
  std::string file{""};
  app.add_option("-f,--filename", file, "Path of the file")->check(CLI::ExistingFile)->required();
  std::uint32_t eventNbr{std::numeric_limits<std::uint32_t>::max()};
  app.add_option("-e,--events", eventNbr, "Event number to process")->expected(1)->check(CLI::PositiveNumber);
  spdlog::level::level_enum verbosity{spdlog::level::trace};
  app.add_option("-v,--verbosity", verbosity, "Verbosity level.")
    ->transform(CLI::CheckedTransformer(
      std::map<std::string, spdlog::level::level_enum>(
        {{"off", spdlog::level::off}, {"trace", spdlog::level::trace}, {"debug", spdlog::level::debug}, {"info", spdlog::level::info}, {"warn", spdlog::level::warn}, {"error", spdlog::level::err}, {"critical", spdlog::level::critical}}),
      CLI::ignore_case));
  std::string output_path{path(argv[0])};
  app.add_option("--output_path", output_path, "Output path")->envname("STREAMOUT_OUTPUT_PATH");
  try
  {
    app.parse(argc, argv);
  }
  catch(const CLI::ParseError& e)
  {
    return app.exit(e);
  }
  spdlog::set_level(verbosity);

  RawdataReader source(file.c_str());
  LCIOWriter    destination;
  destination.setFilename((output_path + "/" + filename(file) + ".slcio").c_str());
  BufferLooper looper(source, destination);
  looper.addSink(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
  looper.loop(eventNbr);
}
