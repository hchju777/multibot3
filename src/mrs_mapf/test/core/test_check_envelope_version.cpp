// SPDX-License-Identifier: Apache-2.0
#include <fstream>
#include <sstream>
#include <string>

#include "../test_util.hpp"
#include "mrs_mapf/adapter/boundary_types.hpp"

/// @file test_check_envelope_version.cpp
/// @brief 🔴 함정 ③(발주서) — diffs `BoundaryDiscretePlan::kSchemaVersion` (the
/// SINGLE named constant this package carries the envelope version in,
/// `adapter/boundary_types.hpp`) against the real schema file's
/// `x-contract-version`. Old pseudocode documents hardcode "2.0.0"; the
/// contract realized is 4.0.0 (`255`§255-9-1 — this is a latent trap for the
/// IMPLEMENTATION step specifically, not a today-live defect in the docs
/// themselves). Modeled on `1.4.2`'s `test_check_envelope_versions.py` lesson.
///
/// The path is baked in by CMake as `MRS_MAPF_SCHEMA_PATH` (an absolute path
/// computed at configure time — a ctest run's working directory is the build
/// tree, not the source tree, so a relative guess here would be fragile).

#ifndef MRS_MAPF_SCHEMA_PATH
#error "MRS_MAPF_SCHEMA_PATH must be defined by CMakeLists.txt"
#endif

int main()
{
    std::ifstream f(MRS_MAPF_SCHEMA_PATH);
    CHECK(f.is_open());
    if (!f.is_open())
    {
        std::printf("  cannot open schema file: %s\n", MRS_MAPF_SCHEMA_PATH);
        return mapf_test::summary();
    }
    std::ostringstream ss;
    ss << f.rdbuf();
    const std::string text = ss.str();

    const std::string key = "\"x-contract-version\": \"";
    auto pos = text.find(key);
    CHECK(pos != std::string::npos);
    if (pos == std::string::npos)
    {
        return mapf_test::summary();
    }
    pos += key.size();
    auto end = text.find('"', pos);
    CHECK(end != std::string::npos);
    const std::string real_version = text.substr(pos, end - pos);

    CHECK(real_version == mrs_mapf::adapter::BoundaryDiscretePlan::kSchemaVersion);
    if (real_version != mrs_mapf::adapter::BoundaryDiscretePlan::kSchemaVersion)
    {
        std::printf("  schema file says %s, this package's constant says %s\n",
                    real_version.c_str(),
                    mrs_mapf::adapter::BoundaryDiscretePlan::kSchemaVersion);
    }

    return mapf_test::summary();
}
