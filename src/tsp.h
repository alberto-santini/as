//
// Created by alberto on 19/04/18.
//

#ifndef AS_TSP_H
#define AS_TSP_H

#include <cstdint>
#include <vector>

#include "tsplib.h"
#include "discorde.h"
#include "mtz.h"

namespace as {
    /** @namespace tsp
     *  @brief     This namespace contains utilities to solve the TSP.
     */
    namespace tsp {
        /** @brief Solves a TSP Instance.
         *
         *  It can use either Concorde (via the Discorde API) or an MTZ model via CPLEX.
         *  It can throw, in case neither solver provides a solution.
         *
         *  @param  instance The TSP instance.
         *  @param  nodes    The subset of nodes of the instance to consider.
         *  @return          The optimal tour.
         */
        inline std::vector<std::uint32_t> solve(const tsplib::TSPInstance& instance, const std::vector<std::uint32_t>& nodes) {
            // Trivial tour.
            if(nodes.size() < 4u) {
                return nodes;
            }

            // Discorde is known to fail if there aren't at least 5 nodes.
            // So for 4 nodes we use a simple MTZ model.
            if(nodes.size() == 4u) {
                try {
                    return mtz_solve_tsp(instance, nodes);
                } catch(const std::runtime_error& error) {
                    throw std::runtime_error("Could not solve a problem of size 4 with Cplex");
                }
            };

            try {
                return discorde_solve_tsp(instance, nodes);
            } catch(const std::runtime_error& error) {
                // If for any other reason discorde/concorde fails, resort back to mtz
                try {
                    return mtz_solve_tsp(instance, nodes);
                } catch(const std::runtime_error& error) {
                    throw std::runtime_error("Could not solve the problem with neither Concorde nor Cplex");
                }
            }
        }

        /** @brief Solves a TSP Instance.
         *
         *  It can use either Concorde (via the Discorde API) or an MTZ model via CPLEX.
         *  It can throw, in case neither solver provides a solution.
         *
         *  @param  instance The TSP instance.
         *  @return          The optimal tour.
         */
        inline std::vector<std::uint32_t> solve(const tsplib::TSPInstance& instance) {
            std::vector<std::uint32_t> nodes(instance.number_of_vertices());
            std::iota(nodes.begin(), nodes.end(), 0u);
            return solve(instance, nodes);
        }

        /** @brief  Computes the cost (i.e., the total distance) of a tour.
         *
         *  @param  instance    The TSP instance.
         *  @param  tour        The tour.
         *  @return             The distances travelled along the tour.
         */
        inline float tour_cost(const tsplib::TSPInstance& instance, const std::vector<std::uint32_t>& tour) {
            float cost = 0.0f;

            for(auto i = 0u; i < tour.size(); ++i) {
                const auto v = tour[i];
                const auto w = tour[(i + 1) % tour.size()];

                assert(v < instance.number_of_vertices());
                assert(w < instance.number_of_vertices());

                cost += instance.get_distance(v, w);
            }

            return cost;
        }
    }
}

#endif //AS_TSP_H
