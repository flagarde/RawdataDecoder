/** \file Exception.h
*  \copyright 2022 F.Lagarde
*/

#pragma once

#include <cstdint>
#include <exception>
#include <string>

class Exception : public std::exception
{
public:
  virtual const char* what() const noexcept { return m_What.c_str(); }
  explicit Exception(const std::string& message) : m_Message(message) { constructWhat(); }
  Exception(const std::int32_t& error, const std::string& message) : m_Error(error), m_Message(message) { constructWhat(); }
  std::int32_t error() { return m_Error; }
  std::string  message() { return m_Message; }

private:
  void constructWhat()
  {
    if(m_Error == 0) m_What = m_Message;
    else
      m_What = std::string("Error ") + std::to_string(m_Error) + std::string(" : ") + m_Message;
  }
  std::string  m_What;
  std::string  m_Message;
  std::int32_t m_Error{0};
};
