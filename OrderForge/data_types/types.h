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

using Symbol = std::string_view;
using Price = ScaledInt<int64_t, static_cast<uint64_t>(1e2)>;
using Quantity = ScaledInt<uint64_t, static_cast<uint64_t>(1e8)>;
using TickSize = Price;
using AccountId = uint64_t;
using OrderId = uint64_t;

using ErrorMessage = std::optional<std::string_view>;