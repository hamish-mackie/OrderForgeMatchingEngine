#pragma once

#include <string>

enum LogType : uint8_t {
    TEST_STRUCT = 1,
    DEBUG = 2,
    ORDER = 10,
    TRADE = 11,

    LEVEL_UPDATE = 20,
    LAST_TRADE_UPDATE = 21,
};

#define REGISTER_TYPE(enum, type)                                                                                      \
    Logger::get_instance().register_type(LogType::enum, [](std::string_view prepend, char *pointer) -> auto {          \
        Logger::get_instance().write<type##Log>(prepend, reinterpret_cast<type##Log *>(pointer));                      \
        return sizeof(type##Log);                                                                                      \
    })

template<std::size_t... Idxs>
constexpr auto substring_as_array(std::string_view str, std::index_sequence<Idxs...>) {
    return std::array{str[Idxs]...};
}

template<typename T>
constexpr auto type_name_array() {
#if defined(__clang__)
    constexpr auto prefix = std::string_view{"[T = "};
    constexpr auto suffix = std::string_view{"]"};
    constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__GNUC__)
    constexpr auto prefix = std::string_view{"with T = "};
    // constexpr auto suffix   = std::string_view{"&]"};
    constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(_MSC_VER)
    constexpr auto prefix = std::string_view{"type_name_array<"};
    constexpr auto suffix = std::string_view{">(void)"};
    constexpr auto function = std::string_view{__FUNCSIG__};
#else
#error Unsupported compiler
#endif

    constexpr auto start = function.find(prefix) + prefix.size();
    constexpr auto end = function.find_first_of("(<&[", start);

    static_assert(start < end);

    constexpr auto name = function.substr(start, (end - start));
    return substring_as_array(name, std::make_index_sequence<name.size()>{});
}

template<typename T>
struct type_name_holder {
    static inline constexpr auto value = type_name_array<T>();
};

template<typename T>
constexpr auto type_name() -> std::string_view {
    constexpr auto &value = type_name_holder<T>::value;
    return std::string_view{value.data(), value.size()};
}

template<std::size_t N1, std::size_t N2, std::size_t N3>
constexpr auto generate_str(std::string_view str, std::string_view str2, std::string_view str3) {
    constexpr std::size_t total_size = N1 + N2 + N3 + 8; // +8 accounts for the brackets, spaces, and "::"
    std::array<char, total_size> result{};

    std::size_t index = 0;

    result[index++] = '[';

    for (char ch: str) {
        result[index++] = ch;
    }

    result[index++] = ':';
    result[index++] = ':';

    for (char ch: str2) {
        result[index++] = ch;
    }

    result[index++] = ']';
    result[index++] = ' ';
    result[index++] = '[';

    for (char ch: str3) {
        result[index++] = ch;
    }

    result[index++] = ']';
    return result;
}

// this is just a wrapper because we cannot declare a static variable in generate_str until c++23
// using the n template is a bit of a hack to get the compiler making different functions and therefore different static
// strings, probably can be done better
template<std::size_t N1, std::size_t N2, std::size_t N3, std::size_t n>
const char *get_str(std::string_view str, std::string_view str2, std::string_view str3) {
    static const auto result = generate_str<N1, N2, N3>(str, str2, str3);
    return result.data();
}

// Defines
// These defines are quite messy and ugly, and you might wonder why?
// The reason is static compile time strings.
// The prepend part of the log "[class::function] [INFO]" is statically generated at compile time.
// This gives a significant speed boost when logging, instead of copying a string,
// the logger just copies a string view, and the background logging thread can take care of the rest.
// There is also independent int provided on each call to get str, Again, compile time static strings.
// If there is not some sort of unique identifier on each, the static string will be overwritten.

// The defines which use custom objects like trade and order and custom-made, and are very fast to log.
// The second struct exists so we can copy data, and then format in another thread later.
// Debug, Info, Warn, Error use a struct but the formatting of the string is done by the main thread.
// Be careful where these log lines are used.

#define TYPENAME type_name<decltype(*this)>()
#define STR_VIEW(str) std::string_view(str)
#define FUNCTION STR_VIEW(__FUNCTION__)

#define LOG_PREPEND(str, n)                                                                                            \
    get_str<TYPENAME.size(), FUNCTION.size(), STR_VIEW(str).size(), n>(TYPENAME, FUNCTION, STR_VIEW(str))

#ifdef DISABLE_LOGGING

#define LOG_TRADE(trade)
#define LOG_ORDER(order)
#define LOG_UPDATE_LEVEL(level_update)
#define LOG_UPDATE_LAST_TRADE(last_trade_update)
#define LOG_DEBUG(format_str, ...)
#define LOG_INFO(format_str, ...)
#define LOG_WARN(format_str, ...)
#define LOG_ERROR(format_str, ...)

#elif RELEASE_LOGGING

#define LOG_ORDER(order)                                                                                               \
    Logger::get_instance().write_buffer<Order, OrderLog>(LogType::ORDER, LOG_PREPEND("ORDER", 1), order)
#define LOG_TRADE(trade)                                                                                               \
    Logger::get_instance().write_buffer<Trade, TradeLog>(LogType::TRADE, LOG_PREPEND("TRADE", 2), trade)
#define LOG_UPDATE_LEVEL(level_update)                                                                                 \
    Logger::get_instance().write_buffer<LevelUpdate, LevelUpdateLog>(LogType::LEVEL_UPDATE,                            \
                                                                     LOG_PREPEND("LEVEL UPDATE", 3), level_update)
#define LOG_UPDATE_LAST_TRADE(last_trade_update)                                                                       \
    Logger::get_instance().write_buffer<LastTradeUpdate, LastTradeUpdateLog>(                                          \
            LogType::LAST_TRADE_UPDATE, LOG_PREPEND("LAST TRADE UPDATE", 4), last_trade_update)
#define LOG_DEBUG(format_str, ...)
#define LOG_INFO(format_str, ...)
#define LOG_WARN(format_str, ...)
#define LOG_ERROR(format_str, ...)

#else

#define LOG_ORDER(order)                                                                                               \
    Logger::get_instance().write_buffer<Order, OrderLog>(LogType::ORDER, LOG_PREPEND("ORDER", 1), order)
#define LOG_TRADE(trade)                                                                                               \
    Logger::get_instance().write_buffer<Trade, TradeLog>(LogType::TRADE, LOG_PREPEND("TRADE", 2), trade)
#define LOG_UPDATE_LEVEL(level_update)                                                                                 \
    Logger::get_instance().write_buffer<LevelUpdate, LevelUpdateLog>(LogType::LEVEL_UPDATE,                            \
                                                                     LOG_PREPEND("LEVEL UPDATE", 3), level_update)
#define LOG_UPDATE_LAST_TRADE(last_trade_update)                                                                       \
    Logger::get_instance().write_buffer<LastTradeUpdate, LastTradeUpdateLog>(                                          \
            LogType::LAST_TRADE_UPDATE, LOG_PREPEND("LAST TRADE UPDATE", 4), last_trade_update)

#define LOG_DEBUG(format_str, ...)                                                                                     \
    Logger::get_instance().log(LogType::DEBUG, LOG_PREPEND("DEBUG", 5), format_str, ##__VA_ARGS__)
#define LOG_INFO(format_str, ...)                                                                                      \
    Logger::get_instance().log(LogType::DEBUG, LOG_PREPEND("INFO", 6), format_str, ##__VA_ARGS__)
#define LOG_WARN(format_str, ...)                                                                                      \
    Logger::get_instance().log(LogType::DEBUG, LOG_PREPEND("WARN", 7), format_str, ##__VA_ARGS__)
#define LOG_ERROR(format_str, ...)                                                                                     \
    Logger::get_instance().log(LogType::DEBUG, LOG_PREPEND("ERROR", 8), format_str, ##__VA_ARGS__)
#endif
