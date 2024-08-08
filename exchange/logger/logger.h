#pragma once
#include <fmt/core.h>

#include <string_view>

class Logger {
public:
    template<typename ... Args>
    static void Info(std::string_view type_name, const char * func, std::string_view format_str, Args&&... args) {
        fmt::print("[INFO] [{}::{}] {}\n", type_name, func, fmt::vformat(format_str, fmt::make_format_args(args...)));
    }

    template<typename ... Args>
    static void Warn(std::string_view type_name, const char * func, std::string_view format_str, Args&&... args) {
        fmt::print("[WARN] [{}::{}] {}\n", type_name, func, fmt::vformat(format_str, fmt::make_format_args(args...)));
    }

    template<typename ... Args>
    static void Error(std::string_view type_name, const char * func, std::string_view format_str, Args&&... args) {
        fmt::print("[ERROR] [{}::{}] {}\n", type_name, func, fmt::vformat(format_str, fmt::make_format_args(args...)));
    }
};

template <std::size_t...Idxs>
constexpr auto substring_as_array(std::string_view str, std::index_sequence<Idxs...>)
{
    return std::array{str[Idxs]...};
}

template <typename T>
constexpr auto type_name_array()
{
#if defined(__clang__)
    constexpr auto prefix   = std::string_view{"[T = "};
    constexpr auto suffix   = std::string_view{"]"};
    constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__GNUC__)
    constexpr auto prefix   = std::string_view{"with T = "};
    constexpr auto suffix   = std::string_view{"&]"};
    constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(_MSC_VER)
    constexpr auto prefix   = std::string_view{"type_name_array<"};
    constexpr auto suffix   = std::string_view{">(void)"};
    constexpr auto function = std::string_view{__FUNCSIG__};
#else
# error Unsupported compiler
#endif

    constexpr auto start = function.find(prefix) + prefix.size();
    constexpr auto end = function.find_first_of("(<&[", start);

    static_assert(start < end);

    constexpr auto name = function.substr(start, (end - start));
    return substring_as_array(name, std::make_index_sequence<name.size()>{});
}

template <typename T>
struct type_name_holder {
    static inline constexpr auto value = type_name_array<T>();
};

template <typename T>
constexpr auto type_name() -> std::string_view
{
    constexpr auto& value = type_name_holder<T>::value;
    return std::string_view{value.data(), value.size()};
}
#define DISABLE_LOGGING

#ifdef DISABLE_LOGGING
#define LOG_INFO(format_str, ...)
#define LOG_WARN(format_str, ...)
#define LOG_ERROR(format_str, ...)
#else
#define LOG_INFO(format_str, ...) Logger::Info(type_name<decltype(*this)>(), __FUNCTION__, format_str, ##__VA_ARGS__)
#define LOG_WARN(format_str, ...) Logger::Warn(type_name<decltype(*this)>(), __FUNCTION__, format_str, ##__VA_ARGS__)
#define LOG_ERROR(format_str, ...) Logger::Error(type_name<decltype(*this)>(), __FUNCTION__, format_str, ##__VA_ARGS__)
#endif
