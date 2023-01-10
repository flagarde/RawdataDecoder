#include "Ploter.h"

void Ploter::save(const std::string& filename)
{
  if(m_TCanvas.size() == 1) m_TCanvas[0]->SaveAs(filename.c_str());
  else
  {
    for(std::size_t i = 0; i != m_TCanvas.size(); ++i)
    {
      if(i == 0) m_TCanvas[i]->SaveAs((filename + "(").c_str());
      else if(i == m_TCanvas.size() - 1)
        m_TCanvas[i]->SaveAs((filename + ")").c_str());
      else
        m_TCanvas[i]->SaveAs(filename.c_str());
    }
  }
}
void Ploter::addCanvas(TCanvas& canvas) { m_TCanvas.push_back(reinterpret_cast<TCanvas*>(canvas.Clone())); }
