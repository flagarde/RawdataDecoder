#include "config.h"
#include "yaml-cpp/yaml.h"

#include <ctime>

using namespace std;

int main(int argc, char* argv[])
{
  clock_t startTime, endTime;
  startTime = clock();
  Config config;
  //Initialize a config parser
  //...
  for(int i = 1; i < argc; i++)
  {
    if(string(argv[i]) == "-c")
    {
      string config_file = "";
      config_file        = string(argv[i + 1]);
      config.Parse(config_file);
      config.Run();
    }
  }
  ////////////////////////
  endTime = clock();
  cout << "Running time: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << " s." << endl;
  return 1;
}
