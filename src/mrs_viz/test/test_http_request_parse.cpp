// SPDX-License-Identifier: Apache-2.0
// Self-contained assert main (no gtest dependency — module precedent,
// mrs_sim/test/test_roadmap_parse_roundtrip.cpp, mrs_core/test/test_status.cpp).
//
// 374_mrs_viz_dashboard.md — locks `ParseRequestLine`/`FirstLine`
// (http_server.hpp), the two pure functions the HTTP layer's correctness
// hinges on. No socket, no rclcpp::init — this runs in <1 ms.

#include <cassert>
#include <cstdio>
#include <string>

#include "../src/node/http_server.hpp"

using mrs_viz::node::FirstLine;
using mrs_viz::node::HttpRequest;
using mrs_viz::node::ParseRequestLine;

namespace
{

void TestValidGetRequestLine()
{
    HttpRequest req;
    const bool ok = ParseRequestLine("GET /api/state HTTP/1.1", req);
    assert(ok);
    assert(req.method == "GET");
    assert(req.path == "/api/state");
}

void TestRootPath()
{
    HttpRequest req;
    assert(ParseRequestLine("GET / HTTP/1.1", req));
    assert(req.path == "/");
}

void TestPostRequestLineStillParses()
{
    // http_server.hpp rejects non-GET at the HandleConnection layer (405),
    // not here — ParseRequestLine is method-agnostic syntax parsing only.
    HttpRequest req;
    assert(ParseRequestLine("POST /api/state HTTP/1.1", req));
    assert(req.method == "POST");
}

void TestMalformedLineRejected()
{
    HttpRequest req;
    req.method = "SENTINEL";
    req.path = "SENTINEL";
    const bool ok = ParseRequestLine("not a valid request line", req);
    assert(!ok);
    // 값을 지어내지 않는다 — 실패 시 out을 건드리지 않는다.
    assert(req.method == "SENTINEL");
    assert(req.path == "SENTINEL");
}

void TestPathMustStartWithSlash()
{
    HttpRequest req;
    const bool ok = ParseRequestLine("GET api/state HTTP/1.1", req);
    assert(!ok);
}

void TestEmptyLineRejected()
{
    HttpRequest req;
    assert(!ParseRequestLine("", req));
}

void TestFirstLineSplitsOnCrlf()
{
    const std::string raw = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(FirstLine(raw) == "GET / HTTP/1.1");
}

void TestFirstLineNoNewlineReturnsWhole()
{
    assert(FirstLine("GET / HTTP/1.1") == "GET / HTTP/1.1");
}

}  // namespace

int main()
{
    TestValidGetRequestLine();
    TestRootPath();
    TestPostRequestLineStillParses();
    TestMalformedLineRejected();
    TestPathMustStartWithSlash();
    TestEmptyLineRejected();
    TestFirstLineSplitsOnCrlf();
    TestFirstLineNoNewlineReturnsWhole();
    std::puts("test_http_request_parse: all assertions passed");
    return 0;
}
