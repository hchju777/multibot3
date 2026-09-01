// SPDX-License-Identifier: Apache-2.0
// CN-22: test/core/ — emits sample boundary artifacts (JSON) for the
// external python checkers (`check_mrta_coverage.py`, `check_boundary_schema.py`)
// to validate. This is deliberately test-local, hand-rolled JSON, NOT a
// production `io/` codec — `341_arch_mrta_addendum.md` §341-5 determined this
// module does not install an `io/` layer (its boundary is entirely ROS wire;
// the production JSON-dump role belongs to `mrs_eval/scripts/dump_artifacts.py`,
// a sibling package this developer does not write). ctest only confirms the
// emitter itself runs clean; the checkers are run separately (see the
// implementation notes' "체커 실행 결과" table for the actual commands/output).
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "mrs_mrta/adapter/assignment_adapter.hpp"
#include "mrs_mrta/adapter/boundary_types.hpp"
#include "mrs_mrta/core/status.hpp"
#include "mrs_mrta/core/types.hpp"
#include "mrs_mrta/plugins/assigner_impls.hpp"
#include "mrs_mrta/service/assignment_service.hpp"
#include "test_util.hpp"

using mrs_mrta::core::AssignerContext;
using mrs_mrta::core::AssignmentView;
using mrs_mrta::core::ReleaseEvent;
using mrs_mrta::plugins::GreedyTailAppend;
using mrs_mrta::service::AssignmentService;
using mrs_mrta::testutil::MapDistanceOracle;

namespace
{

std::string esc(const std::string& s)
{
    std::string out;
    out.reserve(s.size());
    for (char c : s)
    {
        if (c == '"' || c == '\\')
        {
            out.push_back('\\');
        }
        out.push_back(c);
    }
    return out;
}

std::string jstr(const std::string& s)
{
    return "\"" + esc(s) + "\"";
}

ReleaseEvent make_release(std::uint64_t idx,
                          const std::string& task,
                          const std::vector<std::pair<std::string, std::string>>& legs)
{
    ReleaseEvent ev;
    ev.release_index = idx;
    ev.task = task;
    for (const auto& [goal_id, loc] : legs)
    {
        mrs_mrta::core::GoalRecord g;
        g.id = goal_id;
        g.task = task;
        g.location = loc;
        ev.goals.push_back(g);
    }
    return ev;
}

void write_file(const std::filesystem::path& dir,
                const std::string& name,
                const std::string& content)
{
    std::ofstream ofs(dir / name);
    MRTA_CHECK(ofs.good());
    ofs << content;
}

std::string roadmap_json()
{
    std::ostringstream os;
    os << "{\n"
       << "  \"schema\": \"mrs.roadmap\",\n"
       << "  \"schema_version\": \"3.0.0\",\n"
       << "  \"instance_id\": \"mrta-46-sample\",\n"
       << "  \"nodes\": [\n"
       << "    {\"id\": \"V1\", \"x\": 0.0, \"y\": 0.0, \"clearance_m\": 0.5},\n"
       << "    {\"id\": \"V2\", \"x\": 1.0, \"y\": 0.0, \"clearance_m\": 0.5},\n"
       << "    {\"id\": \"L10\", \"x\": 2.0, \"y\": 0.0, \"clearance_m\": 0.5},\n"
       << "    {\"id\": \"L11\", \"x\": 3.0, \"y\": 0.0, \"clearance_m\": 0.5},\n"
       << "    {\"id\": \"L12\", \"x\": 4.0, \"y\": 0.0, \"clearance_m\": 0.5}\n"
       << "  ],\n"
       << "  \"edges\": [\n"
       << "    {\"from\": \"V1\", \"to\": \"L10\", \"traversal\": \"bidirectional\", "
          "\"length_m\": 2.0, \"width_m\": 1.0, \"capacity_robots\": 1, \"corridor\": \"\"},\n"
       << "    {\"from\": \"V2\", \"to\": \"L10\", \"traversal\": \"bidirectional\", "
          "\"length_m\": 1.0, \"width_m\": 1.0, \"capacity_robots\": 1, \"corridor\": \"\"},\n"
       << "    {\"from\": \"V2\", \"to\": \"L11\", \"traversal\": \"bidirectional\", "
          "\"length_m\": 2.0, \"width_m\": 1.0, \"capacity_robots\": 1, \"corridor\": \"\"},\n"
       << "    {\"from\": \"L11\", \"to\": \"L12\", \"traversal\": \"bidirectional\", "
          "\"length_m\": 1.0, \"width_m\": 1.0, \"capacity_robots\": 1, \"corridor\": \"\"}\n"
       << "  ],\n"
       << "  \"endpoints\": [\"L10\", \"L11\", \"L12\"],\n"
       << "  \"corridors\": [],\n"
       << "  \"provenance\": {\n"
       << "    \"min_separation_m\": 0.5,\n"
       << "    \"wall_inflation_m\": 0.1,\n"
       << "    \"capacity_rule_id\": \"rule_a\",\n"
       << "    \"generator_version\": \"mrta-46-test\"\n"
       << "  }\n"
       << "}\n";
    return os.str();
}

std::string assignment_json(const mrs_mrta::adapter::BoundaryAssignment& a)
{
    std::ostringstream os;
    os << "{\n"
       << "  \"schema\": " << jstr(a.schema) << ",\n"
       << "  \"schema_version\": " << jstr(a.schema_version) << ",\n"
       << "  \"instance_id\": " << jstr(a.instance_id) << ",\n"
       << "  \"revision\": " << a.revision << ",\n"
       << "  \"assignments\": [\n";
    for (std::size_t i = 0; i < a.assignments.size(); ++i)
    {
        const auto& ra = a.assignments[i];
        os << "    {\"robot\": " << jstr(ra.robot) << ", \"start\": " << jstr(ra.start)
           << ", \"goals\": [";
        for (std::size_t j = 0; j < ra.goals.size(); ++j)
        {
            const auto& g = ra.goals[j];
            os << "{\"goal_id\": " << jstr(g.goal_id) << ", \"task\": " << jstr(g.task)
               << ", \"location\": " << jstr(g.location) << "}";
            if (j + 1 < ra.goals.size())
            {
                os << ", ";
            }
        }
        os << "]}";
        if (i + 1 < a.assignments.size())
        {
            os << ",";
        }
        os << "\n";
    }
    os << "  ],\n  \"unassigned_tasks\": [";
    for (std::size_t i = 0; i < a.unassigned_tasks.size(); ++i)
    {
        os << jstr(a.unassigned_tasks[i]);
        if (i + 1 < a.unassigned_tasks.size())
        {
            os << ", ";
        }
    }
    os << "]\n}\n";
    return os.str();
}

std::string task_release_json(const ReleaseEvent& ev, const std::string& instance_id)
{
    std::ostringstream os;
    os << "{\n"
       << "  \"schema\": \"mrs.task_release\",\n"
       << "  \"schema_version\": \"2.0.0\",\n"
       << "  \"instance_id\": " << jstr(instance_id) << ",\n"
       << "  \"release_index\": " << ev.release_index << ",\n"
       << "  \"task\": " << jstr(ev.task) << ",\n"
       << "  \"goals\": [";
    for (std::size_t i = 0; i < ev.goals.size(); ++i)
    {
        os << "{\"goal_id\": " << jstr(ev.goals[i].id)
           << ", \"location\": " << jstr(ev.goals[i].location) << "}";
        if (i + 1 < ev.goals.size())
        {
            os << ", ";
        }
    }
    os << "]\n}\n";
    return os.str();
}

std::string tasks_json(const std::vector<std::string>& tasks)
{
    std::ostringstream os;
    os << "{\"tasks\": [";
    for (std::size_t i = 0; i < tasks.size(); ++i)
    {
        os << jstr(tasks[i]);
        if (i + 1 < tasks.size())
        {
            os << ", ";
        }
    }
    os << "]}\n";
    return os.str();
}

}  // namespace

int main()
{
    MapDistanceOracle distances;
    // robotA is closer to L10; robotB is closer to L11 (2-leg task) — deterministic picks.
    distances.set("V1", "L10", 50);
    distances.set("V2", "L10", 500);
    distances.set("V1", "L11", 500);
    distances.set("V2", "L11", 50);

    GreedyTailAppend assigner;
    AssignerContext ctx;
    ctx.robot_count = 2;
    ctx.goal_queue_capacity_ta = 2;
    ctx.distances = &distances;
    MRTA_CHECK(assigner.configure(ctx) == mrs_mrta::core::Status::kOk);

    AssignmentService svc({"robotA", "robotB"},
                          /*goal_queue_capacity_ta=*/2,
                          /*event_ring_slots=*/8,
                          assigner,
                          nullptr);
    svc.set_robot_vertex("robotA", "V1");
    svc.set_robot_vertex("robotB", "V2");

    const ReleaseEvent rel_a = make_release(0, "taskA", {{"gA1", "L10"}});
    const ReleaseEvent rel_b = make_release(1, "taskB", {{"gB1", "L11"}, {"gB2", "L12"}});
    // taskC targets L10 again while gA1 (taskA) is still uncompleted there — A8 blocks it
    // regardless of capacity, so it lands in unassigned_tasks deterministically.
    const ReleaseEvent rel_c = make_release(2, "taskC", {{"gC1", "L10"}});

    MRTA_CHECK(svc.ingest_release(rel_a) == mrs_mrta::core::Status::kOk);
    MRTA_CHECK(svc.ingest_release(rel_b) == mrs_mrta::core::Status::kOk);
    MRTA_CHECK(svc.ingest_release(rel_c) == mrs_mrta::core::Status::kOk);

    const auto view = svc.drain_once();
    MRTA_CHECK(view.has_value());
    MRTA_CHECK(view->unassigned_tasks.size() == 1 && view->unassigned_tasks[0] == "taskC");

    mrs_mrta::adapter::BoundaryAssignment boundary;
    boundary.instance_id = "mrta-46-sample";
    MRTA_CHECK(mrs_mrta::adapter::AssignmentAdapter::to_boundary(*view, boundary) ==
               mrs_mrta::core::Status::kOk);
    MRTA_CHECK(boundary.assignments.size() == 2);  // Every roster robot appears (minItems:1, both).

    const std::filesystem::path dir = "mrta_boundary_artifacts";
    std::filesystem::create_directories(dir);

    write_file(dir, "roadmap.json", roadmap_json());
    write_file(dir, "assignment.json", assignment_json(boundary));
    write_file(dir, "task_release_taskA.json", task_release_json(rel_a, "mrta-46-sample"));
    write_file(dir, "task_release_taskB.json", task_release_json(rel_b, "mrta-46-sample"));
    write_file(dir, "task_release_taskC.json", task_release_json(rel_c, "mrta-46-sample"));
    write_file(dir, "tasks.json", tasks_json({"taskA", "taskB", "taskC"}));

    std::fprintf(stdout,
                 "wrote boundary artifacts under %s\n",
                 std::filesystem::absolute(dir).string().c_str());
    return 0;
}
