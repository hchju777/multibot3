// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_VIZ_NODE_HTTP_SERVER_HPP
#define MRS_VIZ_NODE_HTTP_SERVER_HPP

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <cstring>
#include <functional>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

/// @file http_server.hpp
/// @brief Minimal, dependency-0 HTTP/1.1 server (374_mrs_viz_dashboard.md §「브라우저가
/// ROS 데이터를 어떻게 받는가」 판정).
///
/// 🔴 **판정**: 후보 셋(`rosbridge_suite` / 작은 HTTP·WebSocket 서버 직접구현 / 그 밖)
/// 중 **"그 밖" — POSIX 소켓으로 직접 짠 plain-HTTP 폴링 서버**를 골랐다. WebSocket이
/// 아니다: 브라우저가 `GET /api/state`를 주기 폴링(`config/viz_dashboard.yaml`의
/// `poll_period_ms` — `GET /api/config`로 서버가 값을 알려준다)한다. 이 라운드는
/// **관찰 전용**이라 브라우저→서버 방향의 실시간 채널이 필요 없고(양방향 핸드셰이크·
/// 프레이밍이 필요한 WebSocket은 그 이득을 못 쓴다), 완전한 요청/응답 왕복 하나가
/// 실패해도 다음 폴에서 스스로 복구되는 것이 소켓 하나를 오래 들고 있는 것보다
/// 단순·견고하다. 대가: 진짜 푸시가 아니라 폴 주기(기본 300 ms)만큼의 지연 —
/// 사람이 보는 진단 대시보드에는 그 지연이 무해하다(제어 루프가 아니다).
/// `rosbridge_suite`를 안 고른 이유: 이 샌드박스에 설치돼 있지 않고(실측 확인,
/// `apt`/`ros2 pkg list` 0건), 추가하면 이 레포가 지금 `rclcpp`·`rosidl`·표준 메시지
/// 밖 의존이 거의 없다는 상태를 깬다.
///
/// 접속마다 스레드 하나(짧게 살고 죽는다) — 느린 클라이언트 하나가 accept 루프를
/// 막지 않는다. 요청은 GET만 다룬다(이 대시보드는 조작 UI가 없다 — POST/PUT 라우팅을
/// 만들지 않았다). `Connection: close`로 응답해 매 요청을 독립된 TCP 왕복으로 둔다.

namespace mrs_viz::node
{

/// @brief 파싱된 HTTP 요청 첫 줄.
struct HttpRequest
{
    std::string method;
    std::string path;
};

/// @brief 핸들러가 채우는 응답.
struct HttpResponse
{
    int status = 200;
    std::string status_text = "OK";
    std::string content_type = "text/plain; charset=utf-8";
    std::string body;
};

/// @brief `"GET /api/state HTTP/1.1"` 형태의 요청 첫 줄을 파싱한다(소켓 없이 시험
/// 가능한 순수 함수 — `test/test_http_request_parse.cpp`).
/// @param line 요청의 첫 줄(개행 없이).
/// @param out 파싱 결과.
/// @return true면 `out.method`/`out.path`가 채워졌다. 형식이 안 맞으면 false —
///         이 경우 `out`을 건드리지 않는다(값을 지어내지 않는다).
inline bool ParseRequestLine(const std::string& line, HttpRequest& out)
{
    std::istringstream iss(line);
    std::string method;
    std::string path;
    std::string http_version;
    if (!(iss >> method >> path >> http_version))
    {
        return false;
    }
    if (method.empty() || path.empty() || path.front() != '/')
    {
        return false;
    }
    out.method = method;
    out.path = path;
    return true;
}

/// @brief `\r\n` 기준으로 요청 원본에서 첫 줄만 잘라낸다.
/// @param raw_request 소켓에서 읽은 원본 바이트(문자열로 취급).
/// @return 첫 줄(개행 제외). 개행이 없으면 전체를 그대로 돌려준다.
inline std::string FirstLine(const std::string& raw_request)
{
    const auto pos = raw_request.find("\r\n");
    return pos == std::string::npos ? raw_request : raw_request.substr(0, pos);
}

/// @brief 최소 blocking HTTP/1.1 서버. 요청마다 `handler`를 부르고 응답을 쓴다.
class HttpServer
{
public:
    using Handler = std::function<HttpResponse(const HttpRequest&)>;

    /// @brief 서버를 만든다(아직 소켓을 열지 않는다 — `Start()`가 연다).
    /// @param host bind할 주소("0.0.0.0"이면 전 인터페이스).
    /// @param port bind할 TCP 포트.
    /// @param handler 요청마다 부를 콜백.
    HttpServer(std::string host, int port, Handler handler)
        : host_(std::move(host)), port_(port), handler_(std::move(handler))
    {
    }

    ~HttpServer()
    {
        Stop();
    }

    HttpServer(const HttpServer&) = delete;
    HttpServer& operator=(const HttpServer&) = delete;

    /// @brief 소켓을 열고 accept 루프를 별도 스레드로 띄운다.
    /// @param error_out 실패 시 사람이 읽을 사유를 채운다.
    /// @return true면 기동 성공. false면 `error_out`을 보고 CN-15에 따라 기동 실패로
    ///         처리한다(호출부 — `dashboard_node.cpp` `main`).
    bool Start(std::string& error_out)
    {
        listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_fd_ < 0)
        {
            error_out = "socket() failed: " + std::string(std::strerror(errno));
            return false;
        }
        const int reuse = 1;
        setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(static_cast<std::uint16_t>(port_));
        if (host_ == "0.0.0.0" || host_.empty())
        {
            addr.sin_addr.s_addr = INADDR_ANY;
        }
        else if (inet_pton(AF_INET, host_.c_str(), &addr.sin_addr) != 1)
        {
            error_out = "invalid host: " + host_;
            close(listen_fd_);
            listen_fd_ = -1;
            return false;
        }

        if (bind(listen_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0)
        {
            error_out = "bind(" + host_ + ":" + std::to_string(port_) +
                        ") failed: " + std::string(std::strerror(errno));
            close(listen_fd_);
            listen_fd_ = -1;
            return false;
        }
        if (listen(listen_fd_, kBacklog) != 0)
        {
            error_out = "listen() failed: " + std::string(std::strerror(errno));
            close(listen_fd_);
            listen_fd_ = -1;
            return false;
        }

        running_ = true;
        accept_thread_ = std::thread(&HttpServer::AcceptLoop, this);
        return true;
    }

    /// @brief accept 루프를 멈추고 소켓을 닫는다. 여러 번 불러도 안전하다.
    void Stop()
    {
        if (!running_.exchange(false))
        {
            return;
        }
        if (listen_fd_ >= 0)
        {
            shutdown(listen_fd_, SHUT_RDWR);
            close(listen_fd_);
            listen_fd_ = -1;
        }
        if (accept_thread_.joinable())
        {
            accept_thread_.join();
        }
    }

private:
    static constexpr int kBacklog = 16;
    static constexpr std::size_t kMaxRequestBytes = 8192;

    void AcceptLoop()
    {
        while (running_)
        {
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            const int client_fd =
                accept(listen_fd_, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
            if (client_fd < 0)
            {
                if (!running_)
                {
                    break;  // Stop()이 소켓을 닫아 accept가 깨어난 정상 종료.
                }
                continue;
            }
            // 접속마다 짧게 사는 스레드 — 느린 클라이언트가 다음 accept를 막지 않는다.
            std::thread(&HttpServer::HandleConnection, this, client_fd).detach();
        }
    }

    void HandleConnection(int client_fd)
    {
        std::string raw;
        char buf[2048];
        while (raw.size() < kMaxRequestBytes)
        {
            const ssize_t n = recv(client_fd, buf, sizeof(buf), 0);
            if (n <= 0)
            {
                break;
            }
            raw.append(buf, static_cast<std::size_t>(n));
            if (raw.find("\r\n\r\n") != std::string::npos)
            {
                break;  // 헤더 끝 — GET 요청이라 바디를 더 읽지 않는다.
            }
        }

        HttpResponse resp;
        HttpRequest req;
        if (raw.empty() || !ParseRequestLine(FirstLine(raw), req))
        {
            resp.status = 400;
            resp.status_text = "Bad Request";
            resp.body = "bad request";
        }
        else if (req.method != "GET")
        {
            // 이 대시보드는 관찰 전용이다 — GET 밖은 만들지 않았다(374§「금지」).
            resp.status = 405;
            resp.status_text = "Method Not Allowed";
            resp.body = "only GET is served (observation-only dashboard)";
        }
        else
        {
            resp = handler_(req);
        }

        WriteResponse(client_fd, resp);
        close(client_fd);
    }

    static void WriteResponse(int fd, const HttpResponse& resp)
    {
        std::ostringstream head;
        head << "HTTP/1.1 " << resp.status << " " << resp.status_text << "\r\n"
             << "Content-Type: " << resp.content_type << "\r\n"
             << "Content-Length: " << resp.body.size() << "\r\n"
             << "Connection: close\r\n"
             << "\r\n";
        const std::string head_str = head.str();
        SendAll(fd, head_str);
        SendAll(fd, resp.body);
    }

    static void SendAll(int fd, const std::string& data)
    {
        std::size_t sent = 0;
        while (sent < data.size())
        {
            const ssize_t n = send(fd, data.data() + sent, data.size() - sent, 0);
            if (n <= 0)
            {
                return;  // 클라이언트가 끊었다 — 응답을 지어내지 않고 그냥 멈춘다.
            }
            sent += static_cast<std::size_t>(n);
        }
    }

    std::string host_;
    int port_;
    Handler handler_;
    int listen_fd_ = -1;
    std::atomic<bool> running_{false};
    std::thread accept_thread_;
};

}  // namespace mrs_viz::node

#endif  // MRS_VIZ_NODE_HTTP_SERVER_HPP
