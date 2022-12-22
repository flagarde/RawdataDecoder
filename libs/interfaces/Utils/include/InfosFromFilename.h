#pragma once

#include <cstdint>
#include <ctime>
#include <string>

/**
 * Exctract run number from filename
 * @param file
 * @return runNumber
 */
std::uint32_t extractRunNumber(const std::string& file);

/**
 * Exctract timestamp from filename
 * @param file
 * @return timestamp
 */
std::time_t extractTimestamp(const std::string& file);
