//
// Created by alberto on 15/02/18.
//

#ifndef AS_NUMERIC_CONVERSIONS_H
#define AS_NUMERIC_CONVERSIONS_H

#include <limits>
#include <cstdint>

namespace as {
    /** @namespace  numeric
     *  @brief      This namespace contains helper functions intended to work with numeric types.
     */
    namespace numeric {
        /** @brief  Tells (at run-time) whether a certain numeric value can fit into another numeric type.
         *
         *  From: https://stackoverflow.com/questions/17224256/
         *
         *  @tparam TargetType  The target type, where we want to fit the numeric value.
         *  @tparam SourceType  The type of the numeric value.
         *  @param value        The numeric value.
         *  @return             True iff the numeric value fits into the target type.
         */
        template <typename TargetType, typename SourceType>
        inline bool can_type_fit_value(const SourceType& value) {
            const std::intmax_t bottom_target = std::intmax_t{std::numeric_limits<TargetType>::min()};
            const std::intmax_t bottom_source = std::intmax_t{std::numeric_limits<SourceType>::min()};
            const std::uintmax_t top_target = std::uintmax_t{std::numeric_limits<TargetType>::max()};
            const std::uintmax_t top_source = std::uintmax_t{std::numeric_limits<SourceType>::max()};
            return !(
                (bottom_target > bottom_source && value < static_cast<SourceType>(bottom_target)) ||
                (top_target < top_source && value > static_cast<SourceType>(top_target))
            );
        }
    }
}

#endif //AS_NUMERIC_CONVERSIONS_H
