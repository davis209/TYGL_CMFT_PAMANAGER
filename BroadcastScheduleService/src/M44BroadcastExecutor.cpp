#include "BroadcastSchedulerService.h"
#include "ServiceLogger.h"

#include <algorithm>
#include <array>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
using Socket = SOCKET;
constexpr Socket InvalidSocket = INVALID_SOCKET;
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
using Socket = int;
constexpr Socket InvalidSocket = -1;
#endif

namespace pa_scheduler {
namespace {
constexpr unsigned char DLE = 0xAA;
constexpr unsigned char STX = 0xBB;
constexpr unsigned char ETX = 0xCC;
constexpr unsigned char M44 = 44;
constexpr unsigned char A44 = 0x90;

class SocketRuntime {
public:
    SocketRuntime() {
#ifdef _WIN32
        WSADATA data = {};
        if (WSAStartup(MAKEWORD(2, 2), &data) != 0) throw std::runtime_error("WSAStartup failed");
#endif
    }
    ~SocketRuntime() {
#ifdef _WIN32
        WSACleanup();
#endif
    }
};

class SocketHandle {
public:
    explicit SocketHandle(Socket socket = InvalidSocket) : socket_(socket) {}
    ~SocketHandle() { Close(); }
    Socket get() const { return socket_; }
    void Reset(Socket socket) { Close(); socket_ = socket; }
    Socket Release() { const Socket socket = socket_; socket_ = InvalidSocket; return socket; }
    void Close() {
        if (socket_ == InvalidSocket) return;
#ifdef _WIN32
        closesocket(socket_);
#else
        close(socket_);
#endif
        socket_ = InvalidSocket;
    }
private:
    Socket socket_;
};

std::string SocketError(const std::string& operation) {
#ifdef _WIN32
    return operation + " failed: " + std::to_string(WSAGetLastError());
#else
    return operation + " failed: " + std::string(std::strerror(errno));
#endif
}

bool IsConnectionPending() {
#ifdef _WIN32
    const int error = WSAGetLastError();
    return error == WSAEWOULDBLOCK || error == WSAEINPROGRESS;
#else
    return errno == EINPROGRESS || errno == EWOULDBLOCK;
#endif
}

bool IsWouldBlock() {
#ifdef _WIN32
    return WSAGetLastError() == WSAEWOULDBLOCK;
#else
    return errno == EWOULDBLOCK || errno == EAGAIN;
#endif
}

void SetNonBlocking(Socket socket) {
#ifdef _WIN32
    u_long enabled = 1;
    if (ioctlsocket(socket, FIONBIO, &enabled) != 0) throw std::runtime_error(SocketError("ioctlsocket"));
#else
    const int flags = fcntl(socket, F_GETFL, 0);
    if (flags < 0 || fcntl(socket, F_SETFL, flags | O_NONBLOCK) < 0) throw std::runtime_error(SocketError("fcntl"));
#endif
}

bool WaitFor(Socket socket, bool write, const std::atomic_bool& stopRequested,
             const std::chrono::steady_clock::time_point& deadline) {
    while (!stopRequested.load()) {
        const auto now = std::chrono::steady_clock::now();
        if (now >= deadline) return false;
        const auto milliseconds = std::min<long long>(200, std::chrono::duration_cast<std::chrono::milliseconds>(deadline - now).count());
        fd_set set;
        FD_ZERO(&set);
        FD_SET(socket, &set);
        timeval timeout = { static_cast<long>(milliseconds / 1000), static_cast<long>((milliseconds % 1000) * 1000) };
#ifdef _WIN32
        const int selected = select(0, write ? nullptr : &set, write ? &set : nullptr, nullptr, &timeout);
#else
        const int selected = select(socket + 1, write ? nullptr : &set, write ? &set : nullptr, nullptr, &timeout);
#endif
        if (selected > 0) return true;
        if (selected < 0) throw std::runtime_error(SocketError("select"));
    }
    return false;
}

Socket Connect(const PaDeviceConfig& config, const std::atomic_bool& stopRequested) {
    addrinfo hints = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    addrinfo* addresses = nullptr;
    const int lookup = getaddrinfo(config.host.c_str(), std::to_string(config.port).c_str(), &hints, &addresses);
    if (lookup != 0) throw std::runtime_error("getaddrinfo failed for " + config.host);

    SocketHandle socket;
    std::string lastError;
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(config.connectTimeoutMilliseconds);
    for (addrinfo* address = addresses; address != nullptr && !stopRequested.load(); address = address->ai_next) {
        socket.Reset(::socket(address->ai_family, address->ai_socktype, address->ai_protocol));
        if (socket.get() == InvalidSocket) { lastError = SocketError("socket"); continue; }
        try {
            SetNonBlocking(socket.get());
            if (::connect(socket.get(), address->ai_addr, static_cast<int>(address->ai_addrlen)) != 0) {
                if (!IsConnectionPending() || !WaitFor(socket.get(), true, stopRequested, deadline)) { lastError = SocketError("connect"); continue; }
                int error = 0;
#ifdef _WIN32
                int length = sizeof(error);
#else
                socklen_t length = sizeof(error);
#endif
                if (getsockopt(socket.get(), SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &length) != 0 || error != 0) {
                    lastError = "connect failed: " + std::to_string(error); continue;
                }
            }
            const Socket connected = socket.Release();
            freeaddrinfo(addresses);
            return connected;
        } catch (const std::exception& error) { lastError = error.what(); }
    }
    freeaddrinfo(addresses);
    throw std::runtime_error(lastError.empty() ? "Unable to connect to PA device" : lastError);
}

void SendAll(Socket socket, const std::vector<unsigned char>& bytes, const std::atomic_bool& stopRequested,
             unsigned int timeoutMilliseconds) {
    std::size_t sent = 0;
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMilliseconds);
    while (sent < bytes.size()) {
        if (!WaitFor(socket, true, stopRequested, deadline)) throw std::runtime_error(stopRequested.load() ? "Stop requested" : "PA send timed out");
        const int count = send(socket, reinterpret_cast<const char*>(bytes.data() + sent), static_cast<int>(bytes.size() - sent), 0);
        if (count <= 0) throw std::runtime_error(SocketError("send"));
        sent += static_cast<std::size_t>(count);
    }
}

bool ExtractFrame(std::vector<unsigned char>& bytes, std::vector<unsigned char>& payload) {
    while (bytes.size() >= 2 && (bytes[0] != DLE || bytes[1] != STX)) bytes.erase(bytes.begin());
    if (bytes.size() < 7) return false;
    const std::size_t length = (static_cast<std::size_t>(bytes[2]) << 8) | bytes[3];
    const std::size_t total = length + 7;
    if (bytes.size() < total) return false;
    if (bytes[total - 2] != DLE || bytes[total - 1] != ETX) { bytes.erase(bytes.begin()); return false; }
    unsigned char lrc = 0;
    for (std::size_t index = 0; index < length; ++index) lrc ^= bytes[4 + index];
    if (lrc != bytes[4 + length]) { bytes.erase(bytes.begin(), bytes.begin() + static_cast<std::ptrdiff_t>(total)); return false; }
    payload.assign(bytes.begin() + 4, bytes.begin() + 4 + static_cast<std::ptrdiff_t>(length));
    bytes.erase(bytes.begin(), bytes.begin() + static_cast<std::ptrdiff_t>(total));
    return true;
}

std::vector<unsigned char> BuildM44Payload(const BroadcastSchedule& schedule) {
    if (schedule.messageId < 0 || schedule.messageId > 65535 || schedule.seatId < 0 || schedule.seatId > 255 ||
        schedule.zones > 255 || schedule.language == 0 || schedule.language > 15 || schedule.playCount == 0 || schedule.playCount > 255 ||
        schedule.playIntervalSeconds > 255) throw std::runtime_error("Schedule contains values outside M44 protocol range");
    std::vector<unsigned char> stations;
    std::stringstream source(schedule.stations);
    std::string token;
    while (std::getline(source, token, ',')) {
        if (token.empty()) continue;
        const unsigned long id = std::stoul(token);
        if (id == 0 || id > 255) throw std::runtime_error("Station ID outside M44 protocol range: " + token);
        stations.push_back(static_cast<unsigned char>(id));
    }
    if (stations.empty() || stations.size() > 255) throw std::runtime_error("M44 requires between 1 and 255 stations");

    std::vector<unsigned char> payload;
    payload.reserve(3 + stations.size() * 8);
    payload.push_back(M44);
    payload.push_back(static_cast<unsigned char>(schedule.seatId));
    payload.push_back(static_cast<unsigned char>(stations.size()));
    for (const auto station : stations) {
        payload.push_back(station);
        payload.push_back(static_cast<unsigned char>(schedule.zones));
        payload.push_back(1); // one prerecorded message per station
        payload.push_back(static_cast<unsigned char>(schedule.messageId >> 8));
        payload.push_back(static_cast<unsigned char>(schedule.messageId & 0xFF));
        payload.push_back(static_cast<unsigned char>(schedule.language));
        payload.push_back(static_cast<unsigned char>(schedule.playCount));
        payload.push_back(static_cast<unsigned char>(schedule.playIntervalSeconds));
    }
    return payload;
}

void CopyAscii(std::array<unsigned char, 16>& header, std::size_t offset, std::size_t maximum, const std::string& value) {
    const std::size_t count = std::min(maximum, value.size());
    for (std::size_t index = 0; index < count; ++index) header[offset + index] = static_cast<unsigned char>(value[index]);
}

std::vector<unsigned char> BuildFrame(const PaDeviceConfig& config, const std::vector<unsigned char>& packet) {
    std::array<unsigned char, 16> header = {};
    header[0] = config.serverId;
    header[1] = config.consoleId;
    CopyAscii(header, 2, 5, config.version);
    CopyAscii(header, 7, 3, config.lineId);
    CopyAscii(header, 10, 6, config.stationId);
    std::vector<unsigned char> data(header.begin(), header.end());
    data.insert(data.end(), packet.begin(), packet.end());
    if (data.size() > 65535) throw std::runtime_error("M44 frame exceeds protocol length limit");
    unsigned char lrc = 0;
    for (const auto byte : data) lrc ^= byte;
    std::vector<unsigned char> frame = {DLE, STX, static_cast<unsigned char>(data.size() >> 8), static_cast<unsigned char>(data.size() & 0xFF)};
    frame.insert(frame.end(), data.begin(), data.end());
    frame.push_back(lrc);
    frame.push_back(DLE);
    frame.push_back(ETX);
    return frame;
}

} // namespace

class M44BroadcastExecutor::Session {
public:
    SocketRuntime runtime;
    SocketHandle socket;
    std::mutex mutex;
    std::vector<unsigned char> received;
    std::chrono::steady_clock::time_point lastHeartbeat = std::chrono::steady_clock::time_point::min();
};

namespace {
void CloseSession(M44BroadcastExecutor::Session& session) {
    session.socket.Reset(InvalidSocket);
    session.received.clear();
    session.lastHeartbeat = std::chrono::steady_clock::time_point::min();
}

void EnsureConnected(M44BroadcastExecutor::Session& session, const PaDeviceConfig& config,
                     const std::atomic_bool& stopRequested) {
    if (session.socket.get() != InvalidSocket) return;
    session.socket.Reset(Connect(config, stopRequested));
    ServiceLogger::Info("Connected to PA device " + config.host + ":" + std::to_string(config.port));
}

void SendHeartbeat(M44BroadcastExecutor::Session& session, const PaDeviceConfig& config,
                   const std::atomic_bool& stopRequested) {
    const std::vector<unsigned char> heartbeat = {0};
    SendAll(session.socket.get(), BuildFrame(config, heartbeat), stopRequested, config.connectTimeoutMilliseconds);
    session.lastHeartbeat = std::chrono::steady_clock::now();
    ServiceLogger::Info("Sent PA heartbeat");
}

void EnsureHeartbeat(M44BroadcastExecutor::Session& session, const PaDeviceConfig& config,
                     const std::atomic_bool& stopRequested) {
    const auto now = std::chrono::steady_clock::now();
    if (session.lastHeartbeat == std::chrono::steady_clock::time_point::min() ||
        now - session.lastHeartbeat >= std::chrono::seconds(60)) {
        SendHeartbeat(session, config, stopRequested);
    }
}
} // namespace

M44BroadcastExecutor::M44BroadcastExecutor(PaDeviceConfig config)
    : config_(std::move(config)), session_(new Session()) {
    if (config_.host.empty() || config_.port == 0) throw std::invalid_argument("PA device host and port are required");
}

M44BroadcastExecutor::~M44BroadcastExecutor() { Stop(); }

void M44BroadcastExecutor::Start() {
    if (heartbeatThread_.joinable()) return;
    heartbeatStopRequested_.store(false);
    heartbeatThread_ = std::thread(&M44BroadcastExecutor::HeartbeatLoop, this);
}

void M44BroadcastExecutor::Stop() {
    heartbeatStopRequested_.store(true);
    if (heartbeatThread_.joinable()) heartbeatThread_.join();
    std::lock_guard<std::mutex> lock(session_->mutex);
    CloseSession(*session_);
}

void M44BroadcastExecutor::HeartbeatLoop() {
    while (!heartbeatStopRequested_.load()) {
        bool connected = false;
        try {
            std::lock_guard<std::mutex> lock(session_->mutex);
            EnsureConnected(*session_, config_, heartbeatStopRequested_);
            EnsureHeartbeat(*session_, config_, heartbeatStopRequested_);
            connected = session_->socket.get() != InvalidSocket;
        } catch (const std::exception& error) {
            std::lock_guard<std::mutex> lock(session_->mutex);
            CloseSession(*session_);
            ServiceLogger::Error("PA connection maintenance failed: " + std::string(error.what()));
        }

        const unsigned int waitSeconds = connected ? 1U : 5U;
        for (unsigned int second = 0; second < waitSeconds && !heartbeatStopRequested_.load(); ++second)
            std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

ExecutionResult M44BroadcastExecutor::Execute(const BroadcastSchedule& schedule, const std::atomic_bool& stopRequested) {
    std::lock_guard<std::mutex> lock(session_->mutex);
    try {
        if (stopRequested.load()) return {ExecutionStatus::Cancelled, 0, "STOP_REQUESTED", "Service stop requested"};
        EnsureConnected(*session_, config_, stopRequested);
        EnsureHeartbeat(*session_, config_, stopRequested);
        ServiceLogger::Info("Sending M44 for schedule " + std::to_string(schedule.scheduleId) + " through shared PA connection");
        SendAll(session_->socket.get(), BuildFrame(config_, BuildM44Payload(schedule)), stopRequested, config_.connectTimeoutMilliseconds);

        const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(config_.responseTimeoutMilliseconds);
        while (!stopRequested.load() && WaitFor(session_->socket.get(), false, stopRequested, deadline)) {
            unsigned char buffer[2048];
            const int length = recv(session_->socket.get(), reinterpret_cast<char*>(buffer), sizeof(buffer), 0);
            if (length == 0) {
                CloseSession(*session_);
                return {ExecutionStatus::Failed, 0, "CONNECTION_CLOSED", "PA device closed the persistent connection before A44"};
            }
            if (length < 0) {
                if (IsWouldBlock()) continue;
                const std::string error = SocketError("recv");
                CloseSession(*session_);
                return {ExecutionStatus::Failed, 0, "SOCKET_RECEIVE", error};
            }
            session_->received.insert(session_->received.end(), buffer, buffer + length);
            std::vector<unsigned char> payload;
            while (ExtractFrame(session_->received, payload)) {
                if (payload.size() <= 16) continue;
                if (payload[16] == A44) {
                    ServiceLogger::Info("Received A44 for schedule " + std::to_string(schedule.scheduleId));
                    return {ExecutionStatus::Success, 0, std::string(), std::string()};
                }
                if (payload[16] == 0x64) ServiceLogger::Info("Received PA heartbeat acknowledgement");
                else ServiceLogger::Info("Received PA packet " + std::to_string(payload[16]) + " while waiting for A44");
            }
        }
        return stopRequested.load()
            ? ExecutionResult{ExecutionStatus::Cancelled, 0, "STOP_REQUESTED", "Service stop requested"}
            : ExecutionResult{ExecutionStatus::Failed, 0, "A44_TIMEOUT", "Timed out waiting for PA A44 response"};
    } catch (const std::exception& error) {
        CloseSession(*session_);
        ServiceLogger::Error("M44 failed for schedule " + std::to_string(schedule.scheduleId) + ": " + error.what());
        return {ExecutionStatus::Failed, 0, "M44_TRANSPORT", error.what()};
    }
}

} // namespace pa_scheduler
