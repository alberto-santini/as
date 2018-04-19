//
// Created by alberto on 15/02/18.
//

#ifndef AS_MWIS_HPP
#define AS_MWIS_HPP

#include "numeric.h"
#include "graph.h"
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <numeric>
#include <vector>
#include <iostream>

namespace as {
    /** @namespace  mwis
     *  @brief      This namespace provides a wrapper to Sewell's Maximum-Weight Independent Set library.
     *
     *  The MWIS is well-known problem in combinatorics and operational research.
     *  Given an undirected graph G = (V, E), in our case represented as a Boost Graph,
     *  and a mapping of each vertex to a certain weight >= 0, it asks to find the
     *  independent set of G, which maximises the sum of the weights of its element.
     *  A set S subset of V is called an independent set if, for any two vertices of S
     *  there is no edge connecting them.
     *
     *  Here we provide a wrapper to Sewell's library, which is distributed as part
     *  of Stephan Held's ``exactcolors'' suite: https://github.com/heldstephan/exactcolors
     */
    namespace mwis {
        namespace detail {
            extern "C" {
                #include <exactcolors/mwis_sewell/mwss.h>
                #include <exactcolors/mwis_sewell/mwss_ext.h>
            }
        }

        /** @brief Finds the maximum-weight independent set in the given graph, with the given weights.
         *
         *  (For more information on this problem, see as::mwis).
         *
         *  @tparam BoostGraph  The underlying undirected graph type. Vertices must be stored in a vector.
         *  @param weights      The vector of weights, indexed as the vertices.
         *  @param graph        The graph.
         *  @return             The maximum-weight independent set. If an error occurs, we return an empty vector.
         */
        template<typename BoostGraph>
        inline std::vector<typename boost::graph_traits<BoostGraph>::vertex_descriptor> mwis(
            const std::vector<std::uint32_t>& weights,
            const BoostGraph& graph
        ) {
            // It's much more convenient to work with vecS graphs, so that
            // vertices are just numbered from 0 to boost::vertex(graph) - 1.
            static_assert(
                std::is_same<typename BoostGraph::vertex_list_selector, boost::vecS>::value,
                "mwis only works when vertices are stored in a vector."
            );

            // We also have to make sure the graph is undeirected.
            static_assert(
                std::is_same<typename boost::graph_traits<BoostGraph>::directed_category, boost::undirected_tag>::value,
                "mwis is intended to be used with undirected graphs."
            );

            // Sewell's library, for some reason, uses int as weight type,
            // even though it says that weights should be all non-negative.
            // So, we have to make sure that, whatever size int is on the
            // current platform, we can put all our uint32_t weights in it.
            assert(std::all_of(weights.begin(), weights.end(),
                [] (const std::uint32_t& weight) { return numeric::can_type_fit_value<int>(weight); }
            ));

            using namespace mwis::detail;

            MWSSgraph m_graph;
            MWSSdata m_data;
            wstable_info m_info;
            wstable_parameters m_params;

            // Minimum weight to achieve.
            int m_weight_lower_bound = static_cast<int>(
                *std::min_element(weights.begin(), weights.end())
            );

            // Maximum weight achievable.
            int m_weight_upper_bound = static_cast<int>(
                std::accumulate(weights.begin(), weights.end(), 0u)
            );

            // Variables that will get error codes from Sewell's library.
            int m_graph_allocated = 0, m_initialised = 0, m_called = 0;

            // Create the stable set vector.
            // We create it here, because MWIS uses goto to jump to a label, and
            // creating it later would mean the jump would cross this initialisation,
            // which is not allowed.
            std::vector<typename boost::graph_traits<BoostGraph>::vertex_descriptor> stable_set{};

            // Initialise the data.
            reset_pointers(&m_graph, &m_data, &m_info);
            default_parameters(&m_params);

            const auto num_vertices = boost::num_vertices(graph);

            // We allocate enough memory for the graph, and check whether
            // we succeeded, with the MWIS-provided function.
            m_graph_allocated = allocate_graph(&m_graph, num_vertices);
            MWIScheck_rval(m_graph_allocated, "Cannot allocate MWIS graph structure.");

            // For some reason, we need to specify this again.
            m_graph.n_nodes = num_vertices;

            // From this moment on, we have to keep in mind that, once more
            // for unknown reasons, in MWIS vertices are indexed starting
            // from 1, and not from 0. (!!!)

            // We set the weights:
            for(std::size_t i = 1u; i <= num_vertices; ++i) {
                m_graph.weight[i] = weights.at(i - 1u);

                // We also use this loop to clear the adjacency matrix:
                for(std::size_t j = 1u; j <= num_vertices; ++j) {
                    m_graph.adj[i][j] = 0;
                }
            }

            // This should be redundant, but we make sure once more that
            // no overflow happened and some weight became negative:
            assert(std::all_of(&m_graph.weight[1], &m_graph.weight[num_vertices],
                [] (const int& weight) { return weight >= 0; }
            ));

            // We set the adjacency matrix:
            for(const auto& edge : graph::edges(graph)) {
                const auto source = boost::source(edge, graph) + 1;
                const auto target = boost::target(edge, graph) + 1;
                m_graph.adj[source][target] = 1;
                m_graph.adj[target][source] = 1;
            }

            // build_graph fills in:
            //  * m_graph.n_edges
            //  * m_graph.edge_list
            //  * m_graph.adj_last
            //  * m_graph.node_list[i].adjacent
            //  * m_graph.node_list[i].name
            //  * m_graph.node_list[i].degree
            //  * m_graph.node_list[i].adjv
            //  * m_graph.node_list[i].adj2
            // (see wstable.c:1562)
            build_graph(&m_graph);

            // Checks consistency of the internal variables of m_graph.
            assert(check_graph(&m_graph) == 1);

            // Initialise the solver, and check init was ok.
            m_initialised = initialize_max_wstable(&m_graph, &m_info);
            MWIScheck_rval(m_initialised, "Cannot initialise MWIS algorithm.");

            // Run the solver, and check it completed ok.
            m_called = call_max_wstable(&m_graph, &m_data, &m_params, &m_info, m_weight_upper_bound, m_weight_lower_bound);

            // If the solver did not work, return an empty set.
            if(m_called != 0) {
                free_max_wstable(&m_graph, &m_data, &m_info);
                return std::vector<typename boost::graph_traits<BoostGraph>::vertex_descriptor>{};
            }

            // Resize the stable set with the size of the set found by MWIS.
            stable_set.resize(m_data.n_best);

            // Fill in the values.
            for(int i = 1; i <= m_data.n_best; ++i) {
                if(m_data.best_sol[i] != NULL) {
                    int vertex_id = m_data.best_sol[i]->name - 1;
                    assert(vertex_id >= 0);

                    stable_set[i - 1] = static_cast<typename boost::graph_traits<BoostGraph>::vertex_descriptor>(vertex_id);
                }
            }

            // Free the memory used by MWIS.
            // Notice that we *have* to use a label called ``CLEANUP'',
            // as the C code has a goto which depends on this.
            CLEANUP: free_max_wstable(&m_graph, &m_data, &m_info);

            return stable_set;
        }
    }
}

#endif //AS_MWIS_HPP
