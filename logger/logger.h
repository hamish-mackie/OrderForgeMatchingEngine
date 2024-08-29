#pragma once

#include <fstream>
#include <functional>
#include <queue>
#include <ring_buffer.h>
#include <thread>

#include "data_structs.h"
#include "defines.h"
#include "logger_config.h"

struct LogInfo {
    LogType log_type_;
    std::string_view prepend_;
};

class Logger {
    using RegisterFunc = std::function<uint64_t(std::string_view, char*)>;

public:
    static Logger& get_instance(const LoggerConfig& cfg = get_default_config()) {
        static Logger instance(cfg);

        return instance;
    }

    static const LoggerConfig& get_default_config() {
        static const LoggerConfig default_cfg;
        return default_cfg;
    }

    void register_type(const LogType type, RegisterFunc func) {
        function_register_[static_cast<u_int8_t>(type)] = func;
    }

    template<typename T>
    void write(std::string_view prepend, T* t) {
        auto str = t->get_str();
        log_file_ << fmt::format("{} {}", prepend, str) << "\n";
        if (write_std_out_) {
            fmt::print("{} {}\n", prepend, str);
        }
    }

    template<typename T, typename BufferStruct>
    void write_buffer(LogType type, std::string_view prepend, T& item) {
        uint64_t needed_space = sizeof(LogInfo) + sizeof(BufferStruct);
        auto* pointer = ring_buffer_.get_write_pointer(needed_space);

        // write info struct to buffer
        auto log_info = new (pointer) LogInfo(static_cast<LogType>(type), prepend);
        pointer += sizeof(LogInfo);

        // write struct to buffer
        auto* buffer_item = reinterpret_cast<BufferStruct*>(pointer);
        buffer_item->write(item);

        ring_buffer_.forward_write_pointer(needed_space);
    }

    void read_buffer() {
        while (true) {
            if (!ring_buffer_.can_read()) {
                if (!should_run()) {
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                continue;
            }

            char* pointer = ring_buffer_.get_read_pointer();
            auto* log_info = reinterpret_cast<LogInfo*>(pointer);
            auto prepend = std::string_view(log_info->prepend_);
            pointer += sizeof(LogInfo);

            auto func = function_register_[static_cast<uint8_t>(log_info->log_type_)];
            auto read_size = func(prepend, pointer);

            ring_buffer_.forward_read_pointer(sizeof(LogInfo) + read_size);
        }
    }

    template<typename... Args>
    void log(LogType log_type, std::string_view log_prepend, std::string_view format_str, Args&&... args) {
        auto debug = Debug(fmt::format("{}", fmt::vformat(format_str, fmt::make_format_args(args...))));
        write_buffer<Debug, DebugLog>(log_type, log_prepend, debug);
    }

    void stop() {
        run_.store(false);
        if (log_thread_ && log_thread_->joinable()) {
            log_thread_->join();
        }
        if (log_file_.is_open()) {
            log_file_.close();
        }
    }

    bool should_run() const { return run_.load(std::memory_order_acquire); }

    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;

private:
    RingBuffer ring_buffer_;
    std::vector<RegisterFunc> function_register_;
    bool write_std_out_;

    std::ofstream log_file_;
    std::unique_ptr<std::thread> log_thread_;
    std::atomic<bool> run_{true};
    LoggerConfig cfg_;

    Logger(const LoggerConfig& cfg) :
        cfg_(cfg), write_std_out_(cfg.write_std_out), ring_buffer_(cfg.mem_block_size, cfg.number_blocks),
        function_register_(UINT8_MAX) {

        log_file_.open(cfg.log_file_path, std::ios::out | std::ios::ate);
        if (!log_file_.is_open()) {
            fmt::println("could not open log file");
        }
        log_thread_ = std::make_unique<std::thread>([&]() { read_buffer(); });
    }

    ~Logger() { stop(); }
};
