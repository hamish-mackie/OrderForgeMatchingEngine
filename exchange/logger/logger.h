#pragma once
#include <magic_enum.hpp>
#include <fmt/core.h>
#include <string>

#include <string_view>

constexpr std::string_view extract_class_name(std::string_view pretty_function) {

    int prefix = 0;
    for(int i = 0; i < pretty_function.size(); ++i) {
        if(pretty_function[i] == ' ') {
            prefix = i + 1;
            break;
        }
    }

    int suffix = 0;
    for(int i = pretty_function.size(); i > -1; --i) {
        if(pretty_function[i] == '(') {
            suffix =  pretty_function.size() - i;
            break;
        }
    }
    pretty_function.remove_prefix(prefix);
    pretty_function.remove_suffix(suffix);
    return pretty_function;
}

#define CLASS_NAME extract_class_name(__PRETTY_FUNCTION__)

#define LOG_INFO(title, message) Logger::Info(CLASS_NAME, title, message)
#define LOG_WARN(title, message) Logger::Warn(CLASS_NAME, title, message)
#define LOG_ERROR(title, message) Logger::Error(CLASS_NAME, title, message)

class Logger {
public:
    template<typename T, typename X>
    static void Info(std::string_view class_name, T&& title, X&& message) {
        fmt::print("[INFO] [{}] [{}]: {}\n", class_name, title, message);
    }

    template<typename T, typename X>
    static void Warn(T&& title, X&& message) {
        fmt::print("[WARN] [{}]: {}\n", title, message);
    }

    template<typename T, typename X>
    static void Error(T&& title, X&& message) {
        fmt::print("[ERRO] [{}]: {}\n", title, message);
    }
};