//
// Created by alberto on 13/04/18.
//

#ifndef AS_DISCORDE_H
#define AS_DISCORDE_H

#include <discorde/discorde_cpp.h>
#include <csetjmp>
#include <cstdint>
#include <vector>
#include <csignal>
#include <algorithm>

#include "tsplib.h"
#include "numeric.h"

namespace as {
    /** @namespace tsp
     *  @brief     This namespace contains utilities to solve the TSP.
     */
    namespace tsp {
        namespace {
            inline sigjmp_buf concorde_signal_buffer;

            inline void concorde_crash_handler(int) {
                siglongjmp(concorde_signal_buffer, -1);
            }

            enum class DiscordeStatus : std::uint32_t {
                SUCCESS,
                CONCORDE_CRASH,
                DISCORDE_FAIL,
                DISCORDE_NOT_OPTIMAL
            };
        }

        /** @brief Solves a TSP Instance using the Concorde solver via the Discorde C++ API.
         *
         *  Discorde is a thin C++ API around the TSP solver Concorde.
         *  It can throw, in case an error occurs when calling Concorde.
         *
         *  @param  instance The TSP instance.
         *  @param  nodes    The subset of nodes of the instance to consider.
         *  @return          The optimal tour.
         */
        inline std::vector<std::uint32_t> discorde_solve_tsp(const tsplib::TSPInstance& instance, const std::vector<std::uint32_t>& nodes) {
            // Mapping of node numbers between the subgraph to give Discorde, and the original graph.
            std::map<int, std::uint32_t> subgraph_mapping;

            // Prepare data for ::discorde::concorde_full

            assert(numeric::can_type_fit_value<int>(nodes.size()));
            auto n_nodes = static_cast<const int>(nodes.size());

            int** cost_matrix = new int*[n_nodes];
            for(auto i = 0; i < n_nodes; ++i) {
                cost_matrix[i] = new int[n_nodes];
                subgraph_mapping[i] = nodes[i];

                for(auto j = 0; j < n_nodes; ++j) {
                    cost_matrix[i][j] = static_cast<int>(instance.get_distance(nodes[i], nodes[j]));
                }
            }

            int* out_tour = new int[n_nodes];
            double out_cost;
            int out_status;
            int ret_code;
            DiscordeStatus status = DiscordeStatus::SUCCESS;

            // This will contain a flag indicating whether a longjump occurred.
            int signal_handler_jump_state;

            // Save the stack into the buffer. When the function is called normally,
            // the return value will be 0; when we get here from a signal, the return
            // value will be non-0.
            signal_handler_jump_state = sigsetjmp(concorde_signal_buffer, 1);

            if(signal_handler_jump_state != 0) {
                // We are getting here from a signal handler longjump!
                status = DiscordeStatus::CONCORDE_CRASH;
            }

            if(status == DiscordeStatus::SUCCESS) {
                // Catch SIGSEGV signals happening in Concorde
                std::signal(SIGSEGV, concorde_crash_handler);

                ret_code = ::discorde::concorde_full(n_nodes, cost_matrix, out_tour, &out_cost, &out_status);

                // Reset the SIGSEGV handler
                std::signal(SIGSEGV, SIG_DFL);

                if(ret_code == DISCORDE_RETURN_FAILURE) {
                    status = DiscordeStatus::DISCORDE_FAIL;
                }

                if(out_status != DISCORDE_STATUS_OPTIMAL) {
                    status = DiscordeStatus::DISCORDE_NOT_OPTIMAL;
                }
            }

            std::vector<std::uint32_t> tour_nodes(nodes.size());
            if(status == DiscordeStatus::SUCCESS) {
                for(auto i = 0; i < n_nodes; ++i) {
                    tour_nodes[i] = subgraph_mapping.at(out_tour[i]);
                }
            }

            delete out_tour;
            for(auto i = 0; i < n_nodes; ++i) {
                delete cost_matrix[i];
            }
            delete cost_matrix;

            if(status == DiscordeStatus::SUCCESS) {
                return tour_nodes;
            } else {
                throw std::runtime_error("Discorde failed to provide the optimal solution to the TSP.");
            }
        }

        /** @brief Solves a TSP Instance using the Concorde solver via the Discorde C++ API.
         *
         *  Discorde is a thin C++ API around the TSP solver Concorde.
         *  It can throw, in case an error occurs when calling Concorde.
         *
         *  @param  instance The TSP instance.
         *  @return          The optimal tour.
         */
        inline std::vector<std::uint32_t> discorde_solve_tsp(const tsplib::TSPInstance& instance) {
            std::vector<std::uint32_t> nodes(instance.number_of_vertices());
            std::iota(nodes.begin(), nodes.end(), 0u);
            return discorde_solve_tsp(instance, nodes);
        }
    }
}

#endif //AS_DISCORDE_H
