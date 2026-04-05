#pragma once

#include "lifelong_types.hpp"

#include <string>

class ScenarioLoader {
public:
    explicit ScenarioLoader(const std::string& path);

    const LifelongScenario& scenario() const { return scenario_; }

private:
    LifelongScenario scenario_;
};
