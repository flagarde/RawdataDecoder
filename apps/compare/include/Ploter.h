#pragma once

#include "TCanvas.h"

#include <string>
#include <vector>

class Ploter
{
public:
  void save(const std::string& filename);
  void addCanvas(TCanvas& canvas);

private:
  std::vector<TCanvas*> m_TCanvas;
};
