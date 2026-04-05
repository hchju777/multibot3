#include "logger.hpp"

#include <algorithm>
#include <cctype>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace {

std::string normalize_level_name(const std::string& level_name)
{
    std::string normalized = level_name;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return normalized;
}

}  // namespace

bool try_parse_log_level(const std::string& level_name,
                         spdlog::level::level_enum& level)
{
    const std::string normalized = normalize_level_name(level_name);

    if (normalized == "trace") {
        level = spdlog::level::trace;
        return true;
    }
    if (normalized == "debug") {
        level = spdlog::level::debug;
        return true;
    }
    if (normalized == "info") {
        level = spdlog::level::info;
        return true;
    }
    if (normalized == "warn" || normalized == "warning") {
        level = spdlog::level::warn;
        return true;
    }
    if (normalized == "error" || normalized == "err") {
        level = spdlog::level::err;
        return true;
    }
    if (normalized == "critical") {
        level = spdlog::level::critical;
        return true;
    }
    if (normalized == "off") {
        level = spdlog::level::off;
        return true;
    }

    return false;
}

void init_logging(spdlog::level::level_enum level)
{
    auto logger = spdlog::get("mapf");
    if (!logger) {
        logger = spdlog::stdout_color_mt("mapf");
        logger->set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");
        spdlog::set_default_logger(logger);
        spdlog::flush_on(spdlog::level::warn);
    } else {
        spdlog::set_default_logger(logger);
    }

    logger->set_level(level);
    spdlog::set_level(level);
}

void init_logging(const std::string& level_name)
{
    spdlog::level::level_enum level = spdlog::level::info;
    if (!try_parse_log_level(level_name, level)) {
        level = spdlog::level::info;
    }
    init_logging(level);
}
