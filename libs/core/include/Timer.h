/** \file Timer.h
*  \copyright 2022 F.Lagarde
*/

#pragma once

#include <chrono>

class Timer
{
public:
  void  start() { m_StartTime = std::chrono::high_resolution_clock::now(); }
  void  stop() { m_StopTime = std::chrono::high_resolution_clock::now(); }
  float getElapsedTime() { return std::chrono::duration_cast<std::chrono::microseconds>(m_StopTime - m_StartTime).count(); }

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
  std::chrono::time_point<std::chrono::high_resolution_clock> m_StopTime;
};
