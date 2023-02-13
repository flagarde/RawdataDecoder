#include "CLI/CLI.hpp"
#include "RawtoRoot.h"

#include <ctime>

int main(int argc, char** argv)
{
  CLI::App app{"Legacy Rawdata to ROOT program"};
  app.set_version_flag("--version", "1.0");
  std::string file;
  app.add_option("-f,--filename", file, "Path of the file")->check(CLI::ExistingFile)->required();
  std::string output;
  app.add_option("-o,--output", output, "Path of the file")->required();
  try
  {
    app.parse(argc, argv);
  }
  catch(const CLI::ParseError& e)
  {
    return app.exit(e);
  }
  double   start = clock();
  raw2Root tw;
  tw.Decode(file, output);
  double end = clock();
  std::cout << "end of RawToRoot Time : " << (end - start) / CLOCKS_PER_SEC << std::endl;
}
