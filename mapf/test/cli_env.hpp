#pragma once

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

namespace mapf_cli {

inline std::string trim_copy(const std::string& s)
{
    const auto begin = s.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) return "";
    const auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(begin, end - begin + 1);
}

inline std::string unquote_copy(const std::string& s)
{
    if (s.size() >= 2 &&
        ((s.front() == '"' && s.back() == '"') ||
         (s.front() == '\'' && s.back() == '\''))) {
        return s.substr(1, s.size() - 2);
    }
    return s;
}

inline std::optional<std::filesystem::path> find_dotenv(std::filesystem::path start_dir)
{
    start_dir = std::filesystem::absolute(start_dir);
    while (true) {
        const auto candidate = start_dir / ".env";
        if (std::filesystem::exists(candidate)) return candidate;
        if (start_dir == start_dir.root_path()) break;
        start_dir = start_dir.parent_path();
    }
    return std::nullopt;
}

inline std::optional<std::string> load_dotenv_value(const std::filesystem::path& dotenv_path,
                                                    const std::string& key)
{
    std::ifstream f(dotenv_path);
    if (!f) return std::nullopt;

    std::string line;
    while (std::getline(f, line)) {
        const std::string trimmed = trim_copy(line);
        if (trimmed.empty() || trimmed[0] == '#') continue;

        const auto eq = trimmed.find('=');
        if (eq == std::string::npos) continue;

        const std::string lhs = trim_copy(trimmed.substr(0, eq));
        if (lhs != key) continue;

        return unquote_copy(trim_copy(trimmed.substr(eq + 1)));
    }

    return std::nullopt;
}

inline std::string resolve_path_from(const std::filesystem::path& base_dir,
                                     const std::string& raw_path)
{
    const std::filesystem::path path(raw_path);
    if (path.is_absolute()) return path.lexically_normal().string();
    return (base_dir / path).lexically_normal().string();
}

}  // namespace mapf_cli
