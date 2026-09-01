// SPDX-License-Identifier: Apache-2.0
// Self-contained assert main (no gtest dependency — module precedent).
#include <cassert>
#include <cstdio>

#include "mrs_core/i_instr_sink.hpp"
#include "mrs_core/i_steady_clock.hpp"
#include "mrs_core/status.hpp"

namespace
{
enum class Err
{
    kBad
};

/// @brief Minimal ISteadyClock/IInstrSink stand-ins — confirm the seams are
/// implementable outside mrs_core (i.e. actually usable interfaces, not just
/// headers that compile in isolation).
class FakeClock : public mrs_core::ISteadyClock
{
public:
    double NowSeconds() const override
    {
        return 42.0;
    }
};

class FakeSink : public mrs_core::IInstrSink
{
public:
    mrs_core::Status Observe(const std::string&, double) override
    {
        return mrs_core::Status::kOk;
    }

    mrs_core::Status Increment(const std::string&, std::uint64_t) override
    {
        return mrs_core::Status::kOk;
    }
};
}  // namespace

int main()
{
    using mrs_core::ok;
    using mrs_core::Result;
    using mrs_core::Status;

    assert(ok(Status::kOk));
    assert(!ok(Status::kError));

    auto r = Result<int, Err>::ok_value(7);
    assert(r.ok());
    assert(r.value() == 7);

    auto e = Result<int, Err>::err(Err::kBad);
    assert(!e.ok());
    assert(e.error() == Err::kBad);

    FakeClock clock;
    assert(clock.NowSeconds() == 42.0);

    FakeSink sink;
    assert(ok(sink.Observe("x", 1.0)));
    assert(ok(sink.Increment("y", 1)));

    try
    {
        throw mrs_core::ContractViolation("test");
        assert(false && "unreachable");
    }
    catch (const mrs_core::ContractViolation& cv)
    {
        assert(std::string(cv.what()) == "test");
    }

    std::puts("test_status: OK");
    return 0;
}
