// SPDX-License-Identifier: Apache-2.0
#include "mrs_mapf/io/json_io.hpp"

#include <cctype>
#include <cstdio>
#include <sstream>
#include <stdexcept>

namespace mrs_mapf::io
{

namespace
{

/// @brief Emit a JSON string literal (escapes quotes/backslashes).
std::string str(const std::string& s)
{
    std::string out = "\"";
    for (char c : s)
    {
        if (c == '"' || c == '\\')
        {
            out.push_back('\\');
        }
        out.push_back(c);
    }
    out.push_back('"');
    return out;
}

}  // namespace

// --------------------------------------------------------------------------- serialize

std::string to_json(const adapter::BoundaryDiscretePlan& doc)
{
    std::ostringstream o;
    o << "{\n";
    o << "  \"schema\": " << str(adapter::BoundaryDiscretePlan::kSchema) << ",\n";
    o << "  \"schema_version\": " << str(adapter::BoundaryDiscretePlan::kSchemaVersion) << ",\n";
    o << "  \"instance_id\": " << str(doc.instance_id) << ",\n";
    o << "  \"plan_revision\": " << doc.plan_revision << ",\n";
    o << "  \"plans\": [\n";
    for (std::size_t i = 0; i < doc.plans.size(); ++i)
    {
        const auto& p = doc.plans[i];
        o << "    {\"robot\": " << str(p.robot) << ", \"terminal\": " << str(p.terminal)
          << ", \"steps\": [";
        for (std::size_t j = 0; j < p.steps.size(); ++j)
        {
            o << "{\"index\": " << p.steps[j].index
              << ", \"location\": " << str(p.steps[j].location) << "}"
              << (j + 1 < p.steps.size() ? ", " : "");
        }
        o << "]}" << (i + 1 < doc.plans.size() ? "," : "") << "\n";
    }
    o << "  ],\n";
    o << "  \"visit_order\": [\n";
    for (std::size_t i = 0; i < doc.visit_order.size(); ++i)
    {
        const auto& v = doc.visit_order[i];
        o << "    {\"location\": " << str(v.location) << ", \"sequence\": [";
        for (std::size_t j = 0; j < v.sequence.size(); ++j)
        {
            o << "{\"robot\": " << str(v.sequence[j].robot)
              << ", \"index\": " << v.sequence[j].index << "}"
              << (j + 1 < v.sequence.size() ? ", " : "");
        }
        o << "]}" << (i + 1 < doc.visit_order.size() ? "," : "") << "\n";
    }
    o << "  ]\n";
    o << "}\n";
    return o.str();
}

// --------------------------------------------------------------------------- parse

namespace
{

/// @brief A tiny scanning JSON reader — enough for this package's fixtures and
/// its own emitted documents (shared shape with `mrs_trajopt::io::Scanner`).
class Scanner
{
public:
    explicit Scanner(const std::string& s) : s_(s)
    {
    }

    void skip_ws()
    {
        while (i_ < s_.size() && std::isspace(static_cast<unsigned char>(s_[i_])))
        {
            ++i_;
        }
    }

    bool consume(char c)
    {
        skip_ws();
        if (i_ < s_.size() && s_[i_] == c)
        {
            ++i_;
            return true;
        }
        return false;
    }

    void expect(char c)
    {
        if (!consume(c))
        {
            throw std::runtime_error(std::string("expected '") + c + "' at " + std::to_string(i_));
        }
    }

    std::string parse_string()
    {
        skip_ws();
        expect('"');
        std::string out;
        while (i_ < s_.size() && s_[i_] != '"')
        {
            if (s_[i_] == '\\' && i_ + 1 < s_.size())
            {
                ++i_;
            }
            out.push_back(s_[i_++]);
        }
        expect('"');
        return out;
    }

    double parse_number()
    {
        skip_ws();
        std::size_t start = i_;
        while (i_ < s_.size() &&
               (std::isdigit(static_cast<unsigned char>(s_[i_])) || s_[i_] == '-' ||
                s_[i_] == '+' || s_[i_] == '.' || s_[i_] == 'e' || s_[i_] == 'E'))
        {
            ++i_;
        }
        return std::stod(s_.substr(start, i_ - start));
    }

    std::uint64_t parse_uint()
    {
        return static_cast<std::uint64_t>(parse_number());
    }

    char peek()
    {
        skip_ws();
        return i_ < s_.size() ? s_[i_] : '\0';
    }

    void skip_value()
    {
        skip_ws();
        char c = peek();
        if (c == '"')
        {
            parse_string();
        }
        else if (c == '{')
        {
            skip_object();
        }
        else if (c == '[')
        {
            expect('[');
            while (peek() != ']')
            {
                skip_value();
                if (!consume(','))
                {
                    break;
                }
            }
            expect(']');
        }
        else if (c == 't')
        {
            i_ += 4;
        }
        else if (c == 'f')
        {
            i_ += 5;
        }
        else
        {
            parse_number();
        }
    }

    void skip_object()
    {
        expect('{');
        while (peek() != '}')
        {
            parse_string();
            expect(':');
            skip_value();
            if (!consume(','))
            {
                break;
            }
        }
        expect('}');
    }

    bool parse_bool()
    {
        skip_ws();
        if (s_.compare(i_, 4, "true") == 0)
        {
            i_ += 4;
            return true;
        }
        if (s_.compare(i_, 5, "false") == 0)
        {
            i_ += 5;
            return false;
        }
        throw std::runtime_error("expected bool at " + std::to_string(i_));
    }

private:
    const std::string& s_;
    std::size_t i_ = 0;
};

}  // namespace

adapter::BoundaryDiscretePlan parse_discrete_plan(const std::string& text)
{
    Scanner sc(text);
    adapter::BoundaryDiscretePlan out;
    sc.expect('{');
    while (sc.peek() != '}')
    {
        std::string key = sc.parse_string();
        sc.expect(':');
        if (key == "schema")
        {
            out.schema = sc.parse_string();
        }
        else if (key == "schema_version")
        {
            out.schema_version = sc.parse_string();
        }
        else if (key == "instance_id")
        {
            out.instance_id = sc.parse_string();
        }
        else if (key == "plan_revision")
        {
            out.plan_revision = sc.parse_uint();
        }
        else if (key == "plans")
        {
            sc.expect('[');
            while (sc.peek() != ']')
            {
                adapter::BoundaryRobotPlan p;
                sc.expect('{');
                while (sc.peek() != '}')
                {
                    std::string f = sc.parse_string();
                    sc.expect(':');
                    if (f == "robot")
                    {
                        p.robot = sc.parse_string();
                    }
                    else if (f == "terminal")
                    {
                        p.terminal = sc.parse_string();
                    }
                    else if (f == "steps")
                    {
                        sc.expect('[');
                        while (sc.peek() != ']')
                        {
                            adapter::BoundaryStep s;
                            sc.expect('{');
                            while (sc.peek() != '}')
                            {
                                std::string sf = sc.parse_string();
                                sc.expect(':');
                                if (sf == "index")
                                {
                                    s.index = static_cast<std::uint32_t>(sc.parse_uint());
                                }
                                else if (sf == "location")
                                {
                                    s.location = sc.parse_string();
                                }
                                else
                                {
                                    sc.skip_value();
                                }
                                if (!sc.consume(','))
                                {
                                    break;
                                }
                            }
                            sc.expect('}');
                            p.steps.push_back(s);
                            if (!sc.consume(','))
                            {
                                break;
                            }
                        }
                        sc.expect(']');
                    }
                    else
                    {
                        sc.skip_value();
                    }
                    if (!sc.consume(','))
                    {
                        break;
                    }
                }
                sc.expect('}');
                out.plans.push_back(p);
                if (!sc.consume(','))
                {
                    break;
                }
            }
            sc.expect(']');
        }
        else if (key == "visit_order")
        {
            sc.expect('[');
            while (sc.peek() != ']')
            {
                adapter::BoundaryVisitOrderEntry v;
                sc.expect('{');
                while (sc.peek() != '}')
                {
                    std::string f = sc.parse_string();
                    sc.expect(':');
                    if (f == "location")
                    {
                        v.location = sc.parse_string();
                    }
                    else if (f == "sequence")
                    {
                        sc.expect('[');
                        while (sc.peek() != ']')
                        {
                            adapter::BoundaryVisitItem it;
                            sc.expect('{');
                            while (sc.peek() != '}')
                            {
                                std::string sf = sc.parse_string();
                                sc.expect(':');
                                if (sf == "robot")
                                {
                                    it.robot = sc.parse_string();
                                }
                                else if (sf == "index")
                                {
                                    it.index = static_cast<std::uint32_t>(sc.parse_uint());
                                }
                                else
                                {
                                    sc.skip_value();
                                }
                                if (!sc.consume(','))
                                {
                                    break;
                                }
                            }
                            sc.expect('}');
                            v.sequence.push_back(it);
                            if (!sc.consume(','))
                            {
                                break;
                            }
                        }
                        sc.expect(']');
                    }
                    else
                    {
                        sc.skip_value();
                    }
                    if (!sc.consume(','))
                    {
                        break;
                    }
                }
                sc.expect('}');
                out.visit_order.push_back(v);
                if (!sc.consume(','))
                {
                    break;
                }
            }
            sc.expect(']');
        }
        else
        {
            sc.skip_value();
        }
        if (!sc.consume(','))
        {
            break;
        }
    }
    sc.expect('}');
    return out;
}

adapter::BoundaryRoadmap parse_roadmap(const std::string& text)
{
    Scanner sc(text);
    adapter::BoundaryRoadmap out;
    sc.expect('{');
    while (sc.peek() != '}')
    {
        std::string key = sc.parse_string();
        sc.expect(':');
        if (key == "instance_id")
        {
            out.instance_id = sc.parse_string();
        }
        else if (key == "nodes")
        {
            sc.expect('[');
            while (sc.peek() != ']')
            {
                adapter::BoundaryNode n;
                sc.expect('{');
                while (sc.peek() != '}')
                {
                    std::string f = sc.parse_string();
                    sc.expect(':');
                    if (f == "id")
                    {
                        n.id = sc.parse_string();
                    }
                    else if (f == "x")
                    {
                        n.x = sc.parse_number();
                    }
                    else if (f == "y")
                    {
                        n.y = sc.parse_number();
                    }
                    else if (f == "clearance_m")
                    {
                        n.clearance_m = sc.parse_number();
                    }
                    else
                    {
                        sc.skip_value();
                    }
                    if (!sc.consume(','))
                    {
                        break;
                    }
                }
                sc.expect('}');
                out.nodes.push_back(n);
                if (!sc.consume(','))
                {
                    break;
                }
            }
            sc.expect(']');
        }
        else if (key == "edges")
        {
            sc.expect('[');
            while (sc.peek() != ']')
            {
                adapter::BoundaryEdge e;
                sc.expect('{');
                while (sc.peek() != '}')
                {
                    std::string f = sc.parse_string();
                    sc.expect(':');
                    if (f == "from")
                    {
                        e.from = sc.parse_string();
                    }
                    else if (f == "to")
                    {
                        e.to = sc.parse_string();
                    }
                    else if (f == "traversal")
                    {
                        e.traversal = sc.parse_string();
                    }
                    else if (f == "length_m")
                    {
                        e.length_m = sc.parse_number();
                    }
                    else if (f == "width_m")
                    {
                        e.width_m = sc.parse_number();
                    }
                    else if (f == "capacity_robots")
                    {
                        e.capacity_robots = static_cast<int>(sc.parse_number());
                    }
                    else if (f == "corridor")
                    {
                        e.corridor = sc.parse_string();
                    }
                    else
                    {
                        sc.skip_value();
                    }
                    if (!sc.consume(','))
                    {
                        break;
                    }
                }
                sc.expect('}');
                out.edges.push_back(e);
                if (!sc.consume(','))
                {
                    break;
                }
            }
            sc.expect(']');
        }
        else if (key == "endpoints")
        {
            sc.expect('[');
            while (sc.peek() != ']')
            {
                out.endpoints.push_back(sc.parse_string());
                if (!sc.consume(','))
                {
                    break;
                }
            }
            sc.expect(']');
        }
        else
        {
            sc.skip_value();
        }
        if (!sc.consume(','))
        {
            break;
        }
    }
    sc.expect('}');
    return out;
}

adapter::BoundaryAssignment parse_assignment(const std::string& text)
{
    Scanner sc(text);
    adapter::BoundaryAssignment out;
    sc.expect('{');
    while (sc.peek() != '}')
    {
        std::string key = sc.parse_string();
        sc.expect(':');
        if (key == "instance_id")
        {
            out.instance_id = sc.parse_string();
        }
        else if (key == "revision")
        {
            out.revision = sc.parse_uint();
        }
        else if (key == "assignments")
        {
            sc.expect('[');
            while (sc.peek() != ']')
            {
                adapter::BoundaryAssignmentEntry a;
                sc.expect('{');
                while (sc.peek() != '}')
                {
                    std::string f = sc.parse_string();
                    sc.expect(':');
                    if (f == "robot")
                    {
                        a.robot = sc.parse_string();
                    }
                    else if (f == "start")
                    {
                        a.start = sc.parse_string();
                    }
                    else if (f == "goals")
                    {
                        sc.expect('[');
                        while (sc.peek() != ']')
                        {
                            adapter::BoundaryGoal g;
                            sc.expect('{');
                            while (sc.peek() != '}')
                            {
                                std::string gf = sc.parse_string();
                                sc.expect(':');
                                if (gf == "goal_id")
                                {
                                    g.goal_id = sc.parse_string();
                                }
                                else if (gf == "task")
                                {
                                    g.task = sc.parse_string();
                                }
                                else if (gf == "location")
                                {
                                    g.location = sc.parse_string();
                                }
                                else
                                {
                                    sc.skip_value();
                                }
                                if (!sc.consume(','))
                                {
                                    break;
                                }
                            }
                            sc.expect('}');
                            a.goals.push_back(g);
                            if (!sc.consume(','))
                            {
                                break;
                            }
                        }
                        sc.expect(']');
                    }
                    else
                    {
                        sc.skip_value();
                    }
                    if (!sc.consume(','))
                    {
                        break;
                    }
                }
                sc.expect('}');
                out.assignments.push_back(a);
                if (!sc.consume(','))
                {
                    break;
                }
            }
            sc.expect(']');
        }
        else if (key == "unassigned_tasks")
        {
            sc.expect('[');
            while (sc.peek() != ']')
            {
                out.unassigned_tasks.push_back(sc.parse_string());
                if (!sc.consume(','))
                {
                    break;
                }
            }
            sc.expect(']');
        }
        else
        {
            sc.skip_value();
        }
        if (!sc.consume(','))
        {
            break;
        }
    }
    sc.expect('}');
    return out;
}

adapter::BoundaryReplanRequest parse_replan_request(const std::string& text)
{
    Scanner sc(text);
    adapter::BoundaryReplanRequest out;
    sc.expect('{');
    while (sc.peek() != '}')
    {
        std::string key = sc.parse_string();
        sc.expect(':');
        if (key == "instance_id")
        {
            out.instance_id = sc.parse_string();
        }
        else if (key == "reason")
        {
            out.reason = sc.parse_string();
        }
        else if (key == "blocked_hash")
        {
            out.blocked_hash = sc.parse_string();
        }
        else if (key == "plan_revision")
        {
            out.plan_revision = sc.parse_uint();
        }
        else if (key == "affected_robots")
        {
            sc.expect('[');
            while (sc.peek() != ']')
            {
                out.affected_robots.push_back(sc.parse_string());
                if (!sc.consume(','))
                {
                    break;
                }
            }
            sc.expect(']');
        }
        else if (key == "blocked")
        {
            sc.expect('[');
            while (sc.peek() != ']')
            {
                adapter::BoundaryBlockedEdge e;
                sc.expect('{');
                while (sc.peek() != '}')
                {
                    std::string f = sc.parse_string();
                    sc.expect(':');
                    if (f == "from")
                    {
                        e.from = sc.parse_string();
                    }
                    else if (f == "to")
                    {
                        e.to = sc.parse_string();
                    }
                    else
                    {
                        sc.skip_value();
                    }
                    if (!sc.consume(','))
                    {
                        break;
                    }
                }
                sc.expect('}');
                out.blocked.push_back(e);
                if (!sc.consume(','))
                {
                    break;
                }
            }
            sc.expect(']');
        }
        else if (key == "ordering_constraints")
        {
            sc.expect('[');
            while (sc.peek() != ']')
            {
                adapter::BoundaryOrderingConstraint oc;
                sc.expect('{');
                while (sc.peek() != '}')
                {
                    std::string f = sc.parse_string();
                    sc.expect(':');
                    if (f == "location")
                    {
                        oc.location = sc.parse_string();
                    }
                    else if (f == "sequence")
                    {
                        sc.expect('[');
                        while (sc.peek() != ']')
                        {
                            adapter::BoundaryOrderingItem it;
                            sc.expect('{');
                            while (sc.peek() != '}')
                            {
                                std::string sf = sc.parse_string();
                                sc.expect(':');
                                if (sf == "robot")
                                {
                                    it.robot = sc.parse_string();
                                }
                                else if (sf == "index")
                                {
                                    it.index = static_cast<std::uint32_t>(sc.parse_uint());
                                }
                                else
                                {
                                    sc.skip_value();
                                }
                                if (!sc.consume(','))
                                {
                                    break;
                                }
                            }
                            sc.expect('}');
                            oc.sequence.push_back(it);
                            if (!sc.consume(','))
                            {
                                break;
                            }
                        }
                        sc.expect(']');
                    }
                    else
                    {
                        sc.skip_value();
                    }
                    if (!sc.consume(','))
                    {
                        break;
                    }
                }
                sc.expect('}');
                out.ordering_constraints.push_back(oc);
                if (!sc.consume(','))
                {
                    break;
                }
            }
            sc.expect(']');
        }
        else if (key == "baseline")
        {
            sc.expect('[');
            while (sc.peek() != ']')
            {
                adapter::BoundaryBaselineEntry b;
                sc.expect('{');
                while (sc.peek() != '}')
                {
                    std::string f = sc.parse_string();
                    sc.expect(':');
                    if (f == "robot")
                    {
                        b.robot = sc.parse_string();
                    }
                    else if (f == "committed_segment_count")
                    {
                        b.committed_segment_count = static_cast<std::uint32_t>(sc.parse_uint());
                    }
                    else
                    {
                        sc.skip_value();
                    }
                    if (!sc.consume(','))
                    {
                        break;
                    }
                }
                sc.expect('}');
                out.baseline.push_back(b);
                if (!sc.consume(','))
                {
                    break;
                }
            }
            sc.expect(']');
        }
        else
        {
            sc.skip_value();
        }
        if (!sc.consume(','))
        {
            break;
        }
    }
    sc.expect('}');
    return out;
}

adapter::BoundaryRobotSpecs parse_robot_specs(const std::string& text)
{
    Scanner sc(text);
    adapter::BoundaryRobotSpecs out;
    sc.expect('{');
    while (sc.peek() != '}')
    {
        std::string key = sc.parse_string();
        sc.expect(':');
        if (key == "instance_id")
        {
            out.instance_id = sc.parse_string();
        }
        else if (key == "robots")
        {
            sc.expect('[');
            while (sc.peek() != ']')
            {
                adapter::BoundaryRobotSpec r;
                sc.expect('{');
                while (sc.peek() != '}')
                {
                    std::string f = sc.parse_string();
                    sc.expect(':');
                    if (f == "robot")
                    {
                        r.robot = sc.parse_string();
                    }
                    else if (f == "avg_traversal_speed_mps")
                    {
                        r.avg_traversal_speed_mps = sc.parse_number();
                    }
                    else if (f == "v_max")
                    {
                        r.v_max = sc.parse_number();
                    }
                    else if (f == "brake_decel_min_mps2")
                    {
                        r.brake_decel_min_mps2 = sc.parse_number();
                    }
                    else if (f == "curvature_max_invm")
                    {
                        r.curvature_max_invm = sc.parse_number();
                    }
                    else if (f == "reverse_motion_allowed")
                    {
                        r.reverse_motion_allowed = sc.parse_bool();
                    }
                    else if (f == "circumradius_m")
                    {
                        r.circumradius_m = sc.parse_number();
                    }
                    else
                    {
                        sc.skip_value();
                    }
                    if (!sc.consume(','))
                    {
                        break;
                    }
                }
                sc.expect('}');
                out.robots.push_back(r);
                if (!sc.consume(','))
                {
                    break;
                }
            }
            sc.expect(']');
        }
        else
        {
            sc.skip_value();
        }
        if (!sc.consume(','))
        {
            break;
        }
    }
    sc.expect('}');
    return out;
}

}  // namespace mrs_mapf::io
