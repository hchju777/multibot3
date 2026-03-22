#include "scenario_loader.hpp"

#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <unordered_set>

#include <yaml-cpp/yaml.h>

namespace {

std::string resolve_path_from(const std::filesystem::path& base_dir,
                              const std::string& raw_path)
{
    const std::filesystem::path path(raw_path);
    if (path.is_absolute()) return path.lexically_normal().string();
    return (base_dir / path).lexically_normal().string();
}

std::string require_scalar(const YAML::Node& node, const char* key)
{
    if (!node[key] || !node[key].IsScalar()) {
        throw std::runtime_error(std::string("YAML: '") + key + "' must be a scalar");
    }
    return node[key].as<std::string>();
}

ScenarioEventType parse_event_type(const std::string& value)
{
    if (value == "assign_goal") return ScenarioEventType::ASSIGN_GOAL;
    throw std::runtime_error("YAML: unsupported event type '" + value + "'");
}

ScenarioTriggerType parse_trigger_type(const std::string& value)
{
    if (value == "at_start") return ScenarioTriggerType::AT_START;
    if (value == "on_robot_idle") return ScenarioTriggerType::ON_ROBOT_IDLE;
    throw std::runtime_error("YAML: unsupported trigger type '" + value + "'");
}

ReplanPolicy parse_replan_policy(const std::string& value)
{
    if (value == "replan_on_assignment") {
        return ReplanPolicy::REPLAN_ON_ASSIGNMENT;
    }
    throw std::runtime_error("YAML: unsupported replan policy '" + value + "'");
}

}  // namespace

ScenarioLoader::ScenarioLoader(const std::string& path)
{
    YAML::Node root;
    try {
        root = YAML::LoadFile(path);
    } catch (const YAML::Exception& e) {
        throw std::runtime_error("Failed to load scenario YAML: " + std::string(e.what()));
    }

    const std::filesystem::path scenario_path = std::filesystem::absolute(path);
    const std::filesystem::path base_dir = scenario_path.parent_path();

    if (root["version"]) {
        scenario_.version = root["version"].as<int>();
    }
    if (scenario_.version != 1) {
        throw std::runtime_error("YAML: only scenario version 1 is supported");
    }

    scenario_.path = scenario_path.lexically_normal().string();
    scenario_.map_path = resolve_path_from(base_dir, require_scalar(root, "map"));
    scenario_.params_path = resolve_path_from(base_dir, require_scalar(root, "params"));

    const YAML::Node& robots = root["robots"];
    if (!robots || !robots.IsSequence() || robots.size() == 0) {
        throw std::runtime_error("YAML: 'robots' must be a non-empty sequence");
    }

    std::unordered_set<std::string> robot_ids;
    scenario_.robots.reserve(robots.size());
    for (const auto& robot : robots) {
        RobotSpec spec;
        spec.id = require_scalar(robot, "id");
        if (!robot_ids.insert(spec.id).second) {
            throw std::runtime_error("YAML: duplicate robot id '" + spec.id + "'");
        }
        if (!robot["start"]) {
            throw std::runtime_error("YAML: robot.start is required");
        }
        spec.start = robot["start"].as<int>();
        if (robot["home"]) {
            spec.home = robot["home"].as<int>();
        }
        scenario_.robots.push_back(std::move(spec));
    }

    const YAML::Node& scenario_node = root["scenario"];
    if (!scenario_node) {
        throw std::runtime_error("YAML: 'scenario' key missing");
    }
    scenario_.replan_policy =
        parse_replan_policy(require_scalar(scenario_node, "replan_policy"));

    const YAML::Node& events = scenario_node["events"];
    if (!events || !events.IsSequence()) {
        throw std::runtime_error("YAML: 'scenario.events' must be a sequence");
    }

    std::unordered_set<std::string> event_ids;
    scenario_.events.reserve(events.size());
    for (const auto& event_node : events) {
        ScenarioEvent event;
        event.id = require_scalar(event_node, "id");
        if (!event_ids.insert(event.id).second) {
            throw std::runtime_error("YAML: duplicate event id '" + event.id + "'");
        }

        event.type = parse_event_type(require_scalar(event_node, "type"));
        event.robot_id = require_scalar(event_node, "robot");
        if (robot_ids.count(event.robot_id) == 0) {
            throw std::runtime_error("YAML: event references unknown robot '" + event.robot_id + "'");
        }
        if (!event_node["goal"]) {
            throw std::runtime_error("YAML: event.goal is required");
        }
        event.goal = event_node["goal"].as<int>();

        const YAML::Node& when = event_node["when"];
        if (!when) throw std::runtime_error("YAML: event.when is required");
        event.when.type = parse_trigger_type(require_scalar(when, "type"));
        if (event.when.type == ScenarioTriggerType::ON_ROBOT_IDLE) {
            if (when["robot"]) {
                event.when.robot_id = when["robot"].as<std::string>();
                if (*event.when.robot_id != event.robot_id) {
                    throw std::runtime_error(
                        "YAML: on_robot_idle trigger robot must match event.robot for event '" +
                        event.id + "'");
                }
            } else {
                event.when.robot_id = event.robot_id;
            }
        }

        scenario_.events.push_back(std::move(event));
    }
}
