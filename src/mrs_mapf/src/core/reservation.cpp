// SPDX-License-Identifier: Apache-2.0
#include "mrs_mapf/core/reservation.hpp"

#include <cmath>

namespace mrs_mapf::core
{

namespace
{

const RoadmapArc* find_arc(const Roadmap& roadmap, const std::string& from, const std::string& to)
{
    auto it = roadmap.out_arcs.find(from);
    if (it == roadmap.out_arcs.end())
    {
        return nullptr;
    }
    for (const auto& arc : it->second)
    {
        if (arc.to == to)
        {
            return &arc;
        }
    }
    return nullptr;
}

}  // namespace

std::vector<double> exact_arrival_schedule(const std::vector<StepEntry>& steps,
                                           const Roadmap& roadmap,
                                           double avg_speed_mps)
{
    std::vector<double> arrival;
    if (steps.empty() || avg_speed_mps <= 0.0)
    {
        return arrival;
    }
    arrival.reserve(steps.size());
    arrival.push_back(0.0);  // origin = "now" (§255-6-1).
    for (std::size_t i = 0; i + 1 < steps.size(); ++i)
    {
        const RoadmapArc* arc = find_arc(roadmap, steps[i].location, steps[i + 1].location);
        if (arc == nullptr)
        {
            return {};  // missing arc — the caller cannot form a schedule.
        }
        arrival.push_back(arrival.back() + arc->length_m / avg_speed_mps);
    }
    return arrival;
}

ReservedInterval reserve_interval(double nominal_entry_s,
                                  double nominal_exit_s,
                                  std::optional<double> tick_s)
{
    ReservedInterval r;
    r.nominal_entry_s = nominal_entry_s;
    r.nominal_exit_s = nominal_exit_s;
    if (!tick_s.has_value() || tick_s.value() <= 0.0)
    {
        r.quantized = false;
        r.reserved_entry_s = nominal_entry_s;
        r.reserved_exit_s = nominal_exit_s;
        return r;
    }
    const double tick = tick_s.value();
    r.quantized = true;
    // Endpoint-only outward rounding — entry floor, exit ceil (§255-5-1(ii)).
    r.reserved_entry_s = std::floor(nominal_entry_s / tick) * tick;
    r.reserved_exit_s = std::ceil(nominal_exit_s / tick) * tick;
    return r;
}

std::vector<ReservedInterval> reconstruct_reservations(const std::vector<StepEntry>& steps,
                                                       const Roadmap& roadmap,
                                                       double avg_speed_mps,
                                                       std::optional<double> tick_s)
{
    std::vector<ReservedInterval> out;
    if (steps.size() < 2)
    {
        return out;
    }
    const std::vector<double> arrival = exact_arrival_schedule(steps, roadmap, avg_speed_mps);
    if (arrival.size() != steps.size())
    {
        return out;  // schedule could not be formed (missing arc / invalid speed).
    }
    out.reserve(steps.size() - 1);
    for (std::size_t i = 0; i + 1 < steps.size(); ++i)
    {
        out.push_back(reserve_interval(arrival[i], arrival[i + 1], tick_s));
    }
    return out;
}

}  // namespace mrs_mapf::core
