//
// Created by alberto on 16/10/17.
//

#ifndef AS_IS_ASSOCIATIVE_H
#define AS_IS_ASSOCIATIVE_H

#include <type_traits>

namespace as {
    /** @namespace  tmp
     *  @brief      Template-meta-programming namespace.
     */
    namespace tmp {
        namespace details {
            template<class, class = std::void_t<>>
            struct is_associative : std::false_type {};

            template<class Container>
            struct is_associative<Container, std::void_t<typename Container::key_type>> : std::true_type{};
        }

        /** @typedef    is_associative
         *  @brief      This type will be true_type if the container is associative (i.e.
         *              it has a member type "key_type") or false_type otherwise.
         *
         *  @tparam     Container   The container to check.
         */
        template<class Container>
        using is_associative = details::is_associative<Container>;
    }
}

#endif //AS_IS_ASSOCIATIVE_H
