#pragma once

#include <spdlog/common.h>
#include <string>

// Initialize the process-wide MAPF logger. Reuses the same logger if it already exists.
void init_logging(spdlog::level::level_enum level);
void init_logging(const std::string& level_name);

// Parse common textual log levels. Returns false if the string is not recognized.
bool try_parse_log_level(const std::string& level_name,
                         spdlog::level::level_enum& level);
