//
// Created by alberto on 15/10/17.
//

#ifndef AS_ITERATOR_PAIR_H
#define AS_ITERATOR_PAIR_H

#include <utility>

namespace as {
    /**
     * This class wraps a pair of begin and end iterators, and exposes them via
     * begin() and end() function, which can be used in range-based for loops.
     *
     * @tparam Iterator The iterator type.
     */
    template<class Iterator>
    class iterator_pair {
        /**
         * Begin iterator.
         */
        Iterator _begin;

        /**
         * End iterator.
         */
        Iterator _end;

    public:
        /**
         * Constructor receiving the iterators.
         * @param b Begin iterator.
         * @param e End iterator.
         */
        iterator_pair(Iterator b, Iterator e) : _begin(b), _end(e) {}

        /**
         * Gives the begin iterator.
         * @return The begin iterator.
         */
        Iterator begin() const { return _begin; }

        /**
         * Gives the end iterator.
         * @return The end iterator.
         */
        Iterator end() const { return _end; }
    };

    /**
     * Gives an IteratorPair object constructed from the beginning and end iterators,
     * given as a pair. Many boost functions return iterators in pairs, for example
     * boost::vertices or boost::edges. This function can be used with the pairs
     * returned by these methods, to create an object that can be used with range-based
     * for loops. Example:
     *
     * for(const auto& vertex : make_iter(boost::vertices(graph)) { ... }
     *
     * @tparam Iterator Iterator type.
     * @param iters     A pair containing the beginning and end iterator.
     * @return          An IteratorPair storing the beginning and end iterators.
     */
    template<class Iterator>
    inline iterator_pair<Iterator> make_iter(std::pair<Iterator, Iterator> iters) {
        return iterator_pair<Iterator>(iters.first, iters.second);
    }
}

#endif //AS_ITERATOR_PAIR_H
