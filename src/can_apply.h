//
// Created by alberto on 16/10/17.
//

#ifndef AS_CAN_APPLY_H
#define AS_CAN_APPLY_H

#include <type_traits>

namespace as {
    /*
     * The following is TMP magic used to see if some template is a valid expression. See:
     * https://stackoverflow.com/questions/29521010/enable-template-function-if-class-has-specific-member-function
     */

    /**
     * Template-meta-programming namespace.
     */
    namespace tmp {
        template<class...>
        struct types {
            using type = types;
        };

        namespace details {
            template<template<class...> class Z, class types, class=void>
            struct can_apply : std::false_type {
            };

            template<template<class...> class Z, class...Ts>
            struct can_apply<Z, types<Ts...>, std::void_t<Z<Ts...>>> : std::true_type {
            };
        }

        /**
         * can_apply<my_template, my_arguments...> will inherit from true_type iff
         * my_template<my_arguments...> is a valid expression.
         */
        template<template<class...> class Z, class...Ts>
        using can_apply = details::can_apply<Z, types<Ts...>>;
    }
}

#endif //AS_CAN_APPLY_H
