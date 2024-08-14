#pragma once

#include <iostream>
#include <string_view>
#include <vector>
#include <map>
#include <unordered_map>
#include <optional>

#include <magic_enum.hpp>
#include <logger.h>

#include "scaled_int.h"
#include "id_generator.h"

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