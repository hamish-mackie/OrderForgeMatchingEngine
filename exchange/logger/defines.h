#pragma once

#include <fmt/core.h>

template <std::size_t...Idxs>
constexpr auto substring_as_array(std::string_view str, std::index_sequence<Idxs...>)
{
    return std::array{str[Idxs]...};
}

constexpr std::string compile_time_string_merge(std::string_view s1, std::string_view s2, std::string_view s3) {
    return fmt::format(FMT_STRING("[{}] [{}::{}]"), s1 ,s2, s3);
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
    // constexpr auto suffix   = std::string_view{"&]"};
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

#define TYPE_FUNC_NAME(str) compile_time_string_merge(str, type_name<decltype(*this)>(), std::string_view(__FUNCTION__))

#ifdef DISABLE_LOGGING
#define LOG_DEBUG(format_str, ...)
#define LOG_INFO(format_str, ...)
#define LOG_WARN(format_str, ...)
#define LOG_ERROR(format_str, ...)
#elif RELEASE_LOGGING
#define LOG_DEBUG(format_str, ...)
#define LOG_ORDER(function, ...) Logger::get_instance().log_order(TYPE_FUNC_NAME("ORDER"), function ##)
#define LOG_INFO(format_str, ...) Logger::get_instance().log(TYPE_FUNC_NAME("INFO"), format_str, ##__VA_ARGS__)
#define LOG_WARN(format_str, ...) Logger::get_instance().log(TYPE_FUNC_NAME("WARN"), format_str, ##__VA_ARGS__)
#define LOG_ERROR(format_str, ...) Logger::get_instance().log(TYPE_FUNC_NAME("ERROR"), format_str, ##__VA_ARGS__)
#else
#define LOG_DEBUG(format_str, ...) Logger::get_instance().log(TYPE_FUNC_NAME("DEBUG"), format_str, ##__VA_ARGS__)
#define LOG_INFO(format_str, ...) Logger::get_instance().log(TYPE_FUNC_NAME("INFO"), format_str, ##__VA_ARGS__)
#define LOG_WARN(format_str, ...) Logger::get_instance().log(TYPE_FUNC_NAME("WARN"), format_str, ##__VA_ARGS__)
#define LOG_ERROR(format_str, ...) Logger::get_instance().log(TYPE_FUNC_NAME("ERROR"), format_str, ##__VA_ARGS__)
#endif

