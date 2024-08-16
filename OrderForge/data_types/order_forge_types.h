#pragma once

#include "enums.h"
#include "hashed_linked_list.h"
#include "order.h"
#include "trade.h"
#include "types.h"

namespace of {} // namespace of

template<typename T, uint64_t Scale>
struct fmt::formatter<of::ScaledInt<T, Scale>> : formatter<double> {
    template<typename FormatContext>
    auto format(const of::ScaledInt<T, Scale> &s, FormatContext &ctx) const {
        return formatter<double>::format(s.descaled_value(), ctx);
    }
};
