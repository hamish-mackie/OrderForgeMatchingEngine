#pragma once

#include <iostream>
#include <map>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "magic_enum.hpp"

#include "id_generator.h"
#include "logger.h"
#include "scaled_int.h"

namespace of {

using Symbol = std::string_view;
using Price = ScaledInt<int64_t, static_cast<uint64_t>(1e2)>;
using Quantity = ScaledInt<uint64_t, static_cast<uint64_t>(1e8)>;
using TickSize = Price;
using AccountId = uint64_t;
using OrderId = uint64_t;
using TradeId = uint64_t;

using ErrorMessage = std::optional<std::string_view>;

template<typename Enum>
auto enum_str(Enum enum_val) -> auto {
    return std::string(magic_enum::enum_name(enum_val));
}

// templated so we can use different singletons for orders and trades
template<typename T>
auto get_id() {
    return IdGenerator<T>::instance().generate_id();
}
} // namespace of
