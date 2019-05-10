//
// Created by alberto on 12/04/18.
//

#ifndef AS_COMBINATORIAL_H
#define AS_COMBINATORIAL_H

#include <cstddef>
#include <vector>
#include <limits>

namespace as {
    /** @namespace combi
     *  @brief     This namespace contains utilities related to combinatorics.
     */
    namespace combi {
        namespace detail {
            template<typename Visitor>
            inline void recursive_visit_subsets(std::vector<bool>& indicator, Visitor *const visitor, bool start_from_smaller, std::size_t pivot) {
                if(pivot == std::numeric_limits<std::size_t>::max()) { // Relies on size_t underflow
                    (*visitor)(indicator);
                } else {
                    indicator[pivot] = !start_from_smaller;
                    recursive_visit_subsets(indicator, visitor, start_from_smaller, pivot - 1);
                    indicator[pivot] = start_from_smaller;
                    recursive_visit_subsets(indicator, visitor, start_from_smaller, pivot - 1);
                }
            }
        }

        /** @brief Visits with a user-specified visitors all the subset of an indicator (0-1)
         *         set of a certain size.
         *
         *  For example, if \p size is 3, the visitor will be called on 8 vectors of booleans:
         *  {0,0,0}, {0,0,1}, {0,1,0}, {0,1,1}, {1,0,0}, {1,0,1}, {1,1,0}, {1,1,1}
         *  If \p start_from_smaller is false, then the sequence would be reversed.
         *
         *  @tparam Visitor             The class of visitor called on each subset (which is passed as a vector<bool>).
         *  @param  size                Size of the original set.
         *  @param  visitor             An instance of the visitor.
         *  @param  start_from_smaller  Tells the order in which the subsets should be visited. Smaller does not refer
         *                              to the size of the subset (i.e. the number of ones in the indicator vector).
         *                              Its meaning is the following: if converting the indicator vector to an unsigned
         *                              integer, we would start from 0 and end at pow(2, \p size) - 1.
         */
        template<typename Visitor>
        inline void visit_subsets(std::size_t size, Visitor *const visitor, bool start_from_smaller = true) {
            std::vector<bool> indicator(size);
            detail::recursive_visit_subsets(indicator, visitor, start_from_smaller, indicator.size() - 1);
        }

        /** @brief Check whether a vector si a rotation (cyclic permutation) of another one.
         *
         *  The check is done using the standard trick of concatenating the first vector with
         *  itself, and then checking if this new long vector contains the second vector.
         *
         *  @tparam Vector  A vector-like class.
         *  @param first    The first vector.
         *  @param second   The second vector.
         *  @return         True iff one vector is a rotation of the other.
         */
        template<typename Vector>
        inline bool is_rotation(Vector first, const Vector& second) {
            if(first.size() != second.size()) {
                return false;
            }

            if(first.empty()) {
                return true;
            }

            first.reserve(2u * first.size());
            std::copy(first.begin(), first.begin() + second.size(), std::back_inserter(first));

            return std::includes(first.begin(), first.end(), second.begin(), second.end());
        }
    }
}

#endif //AS_COMBINATORIAL_H
