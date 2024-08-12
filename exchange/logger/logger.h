#pragma once

#include <fstream>
#include <queue>
#include <iostream>
#include <ring_buffer.h>
#include <thread>
#include <utility>

#include "data_structs.h"
#include "defines.h"

struct LogInfo {
    LogType log_type_;
    uint64_t prepend_size_;
};

constexpr uint64_t prepend_max_len = 128;

class Logger {
    using RegisterFunc = std::function<uint64_t(std::string_view, char*)>;
public:
    static Logger& get_instance(bool write_std_out = false, uint64_t mem_block_size = MB * 20, uint32_t no_blocks = 10) {
        static Logger instance(write_std_out, mem_block_size, no_blocks);
        return instance;
    }

    void register_type(const LogType type, RegisterFunc func) {
        function_register_[static_cast<u_int8_t>(type)] = func;
    }

    template<typename T>
    void write(std::string_view prepend, T* t) {
        auto str = t->get_str();
        log_file_ << fmt::format("{} {}", prepend, str) << "\n";
        if(write_std_out_) {
            fmt::print("{} {}\n", prepend, str);
        }
    }

    template<typename T, typename BufferStruct>
    void write_buffer(LogType type, std::string_view prepend, T &item) {
        uint64_t needed_space = sizeof(LogInfo) + prepend.size() + sizeof(BufferStruct);
        auto* pointer = ring_buffer_.get_write_pointer(needed_space);

        // write info struct to buffer
        auto log_info = new(pointer) LogInfo(static_cast<LogType>(type), prepend.size());
        pointer += sizeof(LogInfo);

        // write string view to buffer
        std::memcpy(pointer, prepend.data(), prepend.size());
        pointer += prepend.size();

        // write struct to buffer
        auto* buffer_item = reinterpret_cast<BufferStruct*>(pointer);
        buffer_item->write(item);

        ring_buffer_.forward_write_pointer(needed_space);

        // std::cout << "write block " << current_write_block << " offset " << current_write_offset << std::endl;
    }

    void read_buffer() {
        while (true) {
            if(!ring_buffer_.can_read()) {
                if(!should_run()) { break; }
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                continue;
            }

            char *pointer = ring_buffer_.get_read_pointer();
            auto* log_info = reinterpret_cast<LogInfo*>(pointer);
            pointer += sizeof(LogInfo);

            auto prepend = std::string_view(pointer, log_info->prepend_size_);
            pointer += prepend.size();

            auto func = function_register_[static_cast<u_int8_t>(log_info->log_type_)];
            auto read_size = func(prepend, pointer);

            ring_buffer_.forward_read_pointer(sizeof(LogInfo) + log_info->prepend_size_ + read_size);
        }
    }

    template<typename... Args>
    void log(std::string_view log_prepend, std::string_view format_str, Args &&... args) {
        fmt::print("{} {}\n", log_prepend, fmt::vformat(format_str, fmt::make_format_args(args...)));
    }

    void stop() {
        run_.store(false);
    }

    bool should_run() const {
        return run_.load(std::memory_order_acquire);
    }

    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;
    Logger &operator=(const Logger &) = delete;
    Logger &operator=(Logger &&) = delete;

private:
    RingBuffer ring_buffer_;
    std::vector<RegisterFunc> function_register_;
    bool write_std_out_;

    std::ofstream log_file_;
    std::unique_ptr<std::thread> log_thread_;
    std::atomic<bool> run_{true};


    Logger(bool write_std_out_ = false, uint64_t mem_block_size = MB * 20, uint32_t no_blocks = 10)
        : write_std_out_(write_std_out_)
        , ring_buffer_(mem_block_size, no_blocks)
        , function_register_(UINT8_MAX){

        log_file_.open("log.txt", std::ios::out | std::ios::ate);
        if (!log_file_.is_open()) { fmt::print("could not open log file"); }
        log_thread_ = std::make_unique<std::thread>([&]() { read_buffer(); });
    }

    ~Logger() {
        LOG_INFO("stopping logging thread");
        stop();
        LOG_INFO("joining");
        log_thread_->join();
    }
};

