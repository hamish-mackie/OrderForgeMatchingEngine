#pragma once

#include "types.h"
#include "enums.h"
#include "order.h"
#include "trade.h"
#include "hashed_linked_list.h"


template <typename T, uint64_t Scale>
struct fmt::formatter<ScaledInt<T, Scale>> : formatter<double>
{
    template <typename FormatContext>
    auto format(const ScaledInt<T, Scale>& s, FormatContext& ctx) const
    {
        return formatter<double>::format(s.descaled_value(), ctx);
    }
};
