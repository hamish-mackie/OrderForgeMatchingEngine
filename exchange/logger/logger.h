#pragma once

#include <fstream>
#include <queue>
#include <thread>

#include "defines.h"

class Logger {
public:
    static Logger& get_instance() {
        static Logger instance;
        return instance;
    }

    template<typename ... Args>
    void log(std::string_view log_prepend, std::string_view format_str, Args&&... args) {
        auto *ptr = new std::string(fmt::format("{} {}\n", log_prepend, fmt::vformat(format_str, fmt::make_format_args(args...))));

        log_queue_.push(ptr);
    }

    void log_order(std::string log_prepend, std::function<std::string()> log_function) {
        func_queue_.emplace(log_prepend, log_function);
    }

    void log_function(std::string log_prepend, std::function<std::string()> log_function) {
        log_file_ << fmt::format("{} {}\n", log_prepend, log_function());
    }

    void write_log_queue() {
        while(run_ || !log_queue_.empty() || !func_queue_.empty()) {
            if(!func_queue_.empty()) {
                log_function(func_queue_.front().first, func_queue_.front().second);
                func_queue_.pop();
            }

            if(!log_queue_.empty()) {
                // log_file_ << *log_queue_.front();
                log_queue_.pop();
            }
        }
    }

    void stop() {
        run_.store(false);
    }

    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;

    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;

private:
    std::ofstream log_file_;
    std::queue<std::string*> log_queue_;
    std::queue<std::pair<std::string, std::function<std::string()>>> func_queue_;
    std::unique_ptr<std::thread> log_thread_;
    std::atomic<bool> run_ {true};

    Logger() {
        log_file_.open("log.txt", std::ios::out | std::ios::ate);
        if(!log_file_.is_open()) { fmt::print("could not open log file"); }
        log_thread_ = std::make_unique<std::thread>([&](){write_log_queue();});
    }

    ~Logger() {
        LOG_INFO("stopping logging thread");
        stop();
        log_thread_->join();
    }

};

