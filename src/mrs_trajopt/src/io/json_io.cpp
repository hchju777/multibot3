// SPDX-License-Identifier: Apache-2.0
#include "mrs_trajopt/io/json_io.hpp"

#include <cctype>
#include <cstdio>
#include <sstream>
#include <stdexcept>

namespace mrs_trajopt::io
{

namespace
{

/// @brief Format a double with 12 significant digits (round-trippable).
std::string num(double v)
{
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.12g", v);
    return std::string(buf);
}

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

std::string to_json(const adapter::BoundaryTrajectories& doc)
{
    std::ostringstream o;
    o << "{\n";
    o << "  \"schema\": \"mrs.trajectories\",\n";
    o << "  \"schema_version\": \"2.0.0\",\n";
    o << "  \"instance_id\": " << str(doc.instance_id) << ",\n";
    o << "  \"limits\": {\n";
    o << "    \"a_max\": " << num(doc.limits.a_max) << ",\n";
    o << "    \"j_max\": " << num(doc.limits.j_max) << ",\n";
    o << "    \"min_separation_m\": " << num(doc.limits.min_separation_m) << ",\n";
    o << "    \"omega_max_radps\": " << num(doc.limits.omega_max_radps) << ",\n";
    o << "    \"yaw_accel_max_radps2\": " << num(doc.limits.yaw_accel_max_radps2) << "\n";
    o << "  },\n";
    o << "  \"trajectories\": [\n";
    for (std::size_t r = 0; r < doc.trajectories.size(); ++r)
    {
        const auto& t = doc.trajectories[r];
        o << "    {\n";
        o << "      \"robot\": " << str(t.robot) << ",\n";
        o << "      \"points\": [\n";
        for (std::size_t i = 0; i < t.points.size(); ++i)
        {
            const auto& p = t.points[i];
            o << "        {\"t\": " << num(p.t) << ", \"x\": " << num(p.x)
              << ", \"y\": " << num(p.y) << ", \"yaw\": " << num(p.yaw);
            if (p.has_v)
            {
                o << ", \"v\": " << num(p.v);
            }
            if (p.has_a)
            {
                o << ", \"a\": " << num(p.a);
            }
            o << "}" << (i + 1 < t.points.size() ? "," : "") << "\n";
        }
        o << "      ]\n";
        o << "    }" << (r + 1 < doc.trajectories.size() ? "," : "") << "\n";
    }
    o << "  ]\n";
    o << "}\n";
    return o.str();
}

std::string to_json(const adapter::BoundaryStopDeclarations& doc)
{
    std::ostringstream o;
    o << "{\n";
    o << "  \"schema\": \"mrs.stop_declaration\",\n";
    o << "  \"schema_version\": \"5.0.1\",\n";
    o << "  \"instance_id\": " << str(doc.instance_id) << ",\n";
    o << "  \"declarations\": [\n";
    for (std::size_t i = 0; i < doc.declarations.size(); ++i)
    {
        const auto& d = doc.declarations[i];
        o << "    {\"seq\": " << d.seq << ", \"robot\": " << str(d.robot)
          << ", \"blocked_edge\": {\"from\": " << str(d.blocked_edge.from)
          << ", \"to\": " << str(d.blocked_edge.to)
          << "}, \"declared\": " << (d.declared ? "true" : "false")
          << ", \"reason\": " << str(d.reason) << "}"
          << (i + 1 < doc.declarations.size() ? "," : "") << "\n";
    }
    o << "  ]\n";
    o << "}\n";
    return o.str();
}

// --------------------------------------------------------------------------- parse

namespace
{

/// @brief A tiny scanning JSON reader — just enough for robot_specs fixtures.
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
        else if (c == 't' || c == 'f')
        {
            parse_bool();
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

private:
    const std::string& s_;
    std::size_t i_ = 0;
};

}  // namespace

adapter::BoundaryRobotSpecs parse_robot_specs(const std::string& text)
{
    Scanner sc(text);
    adapter::BoundaryRobotSpecs out;
    sc.expect('{');
    while (sc.peek() != '}')
    {
        std::string key = sc.parse_string();
        sc.expect(':');
        if (key == "robots")
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
        else if (key == "instance_id")
        {
            out.instance_id = sc.parse_string();
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

}  // namespace mrs_trajopt::io
