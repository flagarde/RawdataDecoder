#include "CEPCCDRstyle.h"
#include "CLI/CLI.hpp"
#include "Comparator.h"
#include "Filesystem.h"

#include <string>

int main(int argc, char** argv)
{
  CLI::App app{"Compare between rawdata programs"};
  app.set_version_flag("--version", "1.0");
  std::string old;
  app.add_option("--original", old, "original file")->check(CLI::ExistingFile)->required();
  std::string nouveau;
  app.add_option("--new", nouveau, "new file")->check(CLI::ExistingFile)->required();
  std::string output_path;
  app.add_option("--output_path", output_path, "Output path")->default_val(".")->envname("STREAMOUT_OUTPUT_PATH");
  std::string output_file;
  app.add_option("--pdf_name", output_file, "Name of the pdf generated without .pdf")->default_val("result");
  try
  {
    app.parse(argc, argv);
  }
  catch(const CLI::ParseError& e)
  {
    return app.exit(e);
  }
  SetCEPCCDRStyle();
  Comparator comparator(old, nouveau);
  comparator.Run();
  comparator.SaveAs(output_path + "/" + output_file + ".pdf");
}
