//
// Created by alberto on 19/04/18.
//

#ifndef AS_TSP_H
#define AS_TSP_H

#include <cstdint>
#include <vector>
#include <array>
#include <cassert>
#include <algorithm>

#include "tsplib.h"
#include "discorde.h"
#include "mtz.h"

namespace as {
    /** @namespace tsp
     *  @brief     This namespace contains utilities to solve the TSP.
     */
    namespace tsp {
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
            // So for 4 nodes we use simple enumeration.
            if(nodes.size() == 4u) {
                const std::array<std::vector<std::uint32_t>, 24> tours{{
                    { nodes[0u], nodes[1u], nodes[2u], nodes[3u] },
                    { nodes[0u], nodes[1u], nodes[3u], nodes[2u] },
                    { nodes[0u], nodes[2u], nodes[1u], nodes[3u] },
                    { nodes[0u], nodes[2u], nodes[3u], nodes[1u] },
                    { nodes[0u], nodes[3u], nodes[1u], nodes[2u] },
                    { nodes[0u], nodes[3u], nodes[2u], nodes[1u] },
                    { nodes[1u], nodes[0u], nodes[2u], nodes[3u] },
                    { nodes[1u], nodes[0u], nodes[3u], nodes[2u] },
                    { nodes[1u], nodes[2u], nodes[0u], nodes[3u] },
                    { nodes[1u], nodes[2u], nodes[3u], nodes[0u] },
                    { nodes[1u], nodes[3u], nodes[0u], nodes[2u] },
                    { nodes[1u], nodes[3u], nodes[2u], nodes[0u] },
                    { nodes[2u], nodes[0u], nodes[1u], nodes[3u] },
                    { nodes[2u], nodes[0u], nodes[3u], nodes[1u] },
                    { nodes[2u], nodes[1u], nodes[0u], nodes[3u] },
                    { nodes[2u], nodes[1u], nodes[3u], nodes[0u] },
                    { nodes[2u], nodes[3u], nodes[0u], nodes[1u] },
                    { nodes[2u], nodes[3u], nodes[1u], nodes[0u] },
                    { nodes[3u], nodes[0u], nodes[1u], nodes[2u] },
                    { nodes[3u], nodes[0u], nodes[2u], nodes[1u] },
                    { nodes[3u], nodes[1u], nodes[0u], nodes[2u] },
                    { nodes[3u], nodes[1u], nodes[2u], nodes[0u] },
                    { nodes[3u], nodes[2u], nodes[0u], nodes[1u] },
                    { nodes[3u], nodes[2u], nodes[1u], nodes[0u] }
                }};

                return *std::min_element(tours.begin(), tours.end(),
                    [&instance] (const auto& tour1, const auto& tour2) -> bool {
                        return tour_cost(instance, tour1) < tour_cost(instance, tour2);
                    }
                );
            };

            try {
                return discorde_solve_tsp(instance, nodes);
            } catch(const std::runtime_error& error) {
                // If for any other reason discorde/concorde fails, resort back to the MTZ model.
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
        *  @param  nodes    A boolean mask of the same size as the instance, indicating
         *                  the subset of nodes to consider.
        *  @return          The optimal tour.
        */
        inline std::vector<std::uint32_t> solve(const tsplib::TSPInstance& instance, const std::vector<bool>& nodes) {
            assert(nodes.size() == instance.number_of_vertices());

            std::vector<std::uint32_t> explicit_nodes;

            for(auto node = 0u; node < nodes.size(); ++node) {
                if(nodes[node]) {
                    explicit_nodes.push_back(node);
                }
            }

            return solve(instance, explicit_nodes);
        }

        /** @brief Solves a TSP Instance where node 0 is assumed to be the depot and is always present.
         *
         *  It can use either Concorde (via the Discorde API) or an MTZ model via CPLEX.
         *  It can throw, in case neither solver provides a solution.
         *
         *  @param  instance The TSP instance.
         *  @param  nodes    A boolean mask of the same size as the instance minus one, indicating
         *                   the subset of customers to consider. The depot (indexed by 0) is always
         *                   present and is not part of the mask vector. Therefore element node[i]
         *                   tells wether or not node i-1 should be included.
         *  @return          The optimal tour.
         */
        inline std::vector<std::uint32_t> solve_with_depot(const tsplib::TSPInstance& instance, const std::vector<bool>& nodes) {
            assert(nodes.size() == instance.number_of_vertices() - 1u);

            std::vector<std::uint32_t> explicit_nodes = { 0u };

            for(auto node = 0u; node < nodes.size(); ++node) {
                if(nodes[node]) {
                    explicit_nodes.push_back(node + 1u);
                }
            }

            return solve(instance, explicit_nodes);
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
    }
}

#endif //AS_TSP_H
