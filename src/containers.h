//
// Created by alberto on 16/10/17.
//

#ifndef AS_CONTAINERS_H
#define AS_CONTAINERS_H

#include "tmp.h"
#include <algorithm>
#include <set>
#include <iostream>

#if __has_include(<experimental/iterator>)
    #include <experimental/iterator>
    #define WITH_OSTREAM_JOINER
#else
    #undef WITH_OSTREAM_JOINER
#endif

namespace as {
    /** @namespace containers
     *  @brief     Utilities related to container types.
     */
    namespace containers {
        /** @typdef count_method_type
         *  @brief  This is the type of a generic .count() method for a container.
         *
         *  This method is implemented in some stl containers and takes an element as its only parameter.
         */
        template<class Container, class T>
        using count_method_type = decltype(std::declval<Container>().count(std::declval<T>()));

        /** @typedef    has_count_method
         *  @brief      This will inherit from true_type iff class Container has an appopriate .count() method.
         */
        template<class Container, class T>
        using has_count_method = tmp::can_apply<count_method_type, Container, T>;

        namespace detail {
            // Overload of contains for when .count() is not available.
            template<class Container, class T>
            inline bool contains(std::false_type, const Container& container, const T& element) {
                return std::find(container.begin(), container.end(), element) != container.end();
            }

            // Overload of contains for when .count() is available.
            template<class Container, class T>
            inline bool contains(std::true_type, const Container& container, const T& element) {
                return container.count(element) > 0u;
            }
        }

        /** @brief  Tells whether a container contains a certain element.
         *
         *  The standard library's functions to find elements (e.g. std::find)
         *  always return an iterator. Sometimes, though, we just want to know
         *  whether an element is in a container or not. This helper function
         *  lets us do this in a concise way. This function also has specialisation
         *  for when the container implements a .count() method, i.e. a more
         *  efficient way of searching elements than simple linear search.
         *
         *  @tparam Container    Container type.
         *  @tparam T            Containee type.
         *  @param  container    The container.
         *  @param  element      The element we are searching in \p container.
         *  @return              True iff \p element was found in \p container.
         */
        template<class Container, class T>
        inline bool contains(const Container& container, const T& element) {
            return detail::contains(has_count_method<const Container&, const T&>{}, container, element);
        };

        namespace detail {
            // Prints a value to an output stream.
            template<class Value>
            inline std::ostream& iterated_value(const Value& value, std::ostream& out) {
                return out << value;
            }

            // Prints a key-value pair to an output stream.
            template<class Key, class Value>
            inline std::ostream& iterated_value(const std::pair<Key, Value>& key_val, std::ostream& out) {
                return out << key_val.first << ": " << key_val.second;
            }
        }

        /** @brief  Joins the elements of a container using the separator, and prints the result to
         *          an output stream.
         *
         *  It works both with key-value and value-only containers.
         *  In the first case, it prints both key and value.
         *
         *  @tparam Container    Container type.
         *  @param  container    The container to print.
         *  @param  out          Output stream.
         *  @param  separator    A string interposed between two adjacent elements.
         */
        template<class Container>
        inline void join_and_print(const Container& container, std::ostream& out = std::cout, std::string separator = ", ") {
            if(container.begin() == container.end()) { out << "\n"; return; }
            for(auto it = container.begin(); it != std::prev(container.end()); ++it) {
                detail::iterated_value(*it, out) << separator;
            }
            auto last = std::prev(container.end());
            detail::iterated_value(*last, out) << "\n";
        }

        /** @brief  Joins the elements of a container using the separator, and prints the result to
         *          the standard output stream.
         *
         *  It works both with key-value and value-only containers.
         *  In the first case, it prints both key and value.
         *
         *  @tparam Container    Container type.
         *  @param  container    The container to print.
         *  @param  separator    A string interposed between two adjacent elements.
         */
        template<typename Container>
        inline void join_and_print(const Container& container, std::string separator) {
            return join_and_print(container, std::cout, separator);
        }

        namespace detail {
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
        using is_associative = detail::is_associative<Container>;
    }
}

#endif //AS_CONTAINERS_H
