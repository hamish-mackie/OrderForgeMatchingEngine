#pragma once

#include <cstdint>
#include <iostream>

using FileDescriptor = int32_t;

class EventHandler {
public:
    explicit EventHandler(const FileDescriptor fd): fd_(fd) {}

    virtual void handle_event(uint64_t events) = 0;

    void* get_handle() { return static_cast<void*>(this); }
    FileDescriptor get_fd() { return fd_; }

    virtual ~EventHandler() = default;

protected:
    const FileDescriptor fd_;
};
