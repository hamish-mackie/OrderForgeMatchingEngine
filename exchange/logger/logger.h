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

#define LOG_INFO(...) Logger::Info(CLASS_NAME, __VA_ARGS__)
#define LOG_WARN(...) Logger::Warn(CLASS_NAME, __VA_ARGS__)
#define LOG_ERROR(...) Logger::Error(CLASS_NAME, __VA_ARGS__)

class Logger {
public:
    template<typename T, typename X>
    static void Info(std::string_view class_name, T&& title, X&& message) {
        fmt::print("[INFO] [{}] [{}]: {}\n", class_name, title, message);
    }

    template<typename T>
    static void Info(std::string_view class_name, T&& message) {
        fmt::print("[INFO] [{}]: {}\n", class_name, message);
    }

    template<typename T, typename X>
    static void Warn(std::string_view class_name, T&& title, X&& message) {
        fmt::print("[WARN] [{}]: {}\n", class_name, title, message);
    }

    template<typename T>
    static void Warn(std::string_view class_name, T&& message) {
        fmt::print("[Warn] [{}]: {}\n", class_name, message);
    }

    template<typename T, typename X>
    static void Error(std::string_view class_name, T&& title, X&& message) {
        fmt::print("[ERRO] [{}]: {}\n", class_name, title, message);
    }

    template<typename T>
    static void Error(std::string_view class_name, T&& message) {
        fmt::print("[Error] [{}]: {}\n", class_name, message);
    }
};