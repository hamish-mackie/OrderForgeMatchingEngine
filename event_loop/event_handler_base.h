#pragma once

#include <cstdint>
#include <defines.h>
#include <logger.h>
#include <netinet/in.h>

using FileDescriptor = int32_t;

class EventHandler {
public:
    EventHandler() : fd_(0) {}
    explicit EventHandler(const FileDescriptor fd) : fd_(fd) {}

    virtual void handle_event(uint64_t events) = 0;

    void* get_handle() { return static_cast<void*>(this); }
    [[nodiscard]] FileDescriptor get_fd() const { return fd_; }

    virtual ~EventHandler() = default;

protected:
    FileDescriptor fd_;
};
