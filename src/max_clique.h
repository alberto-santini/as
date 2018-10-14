//
// Created by alberto on 03/05/18.
//

#ifndef AS_MAX_CLIQUE_H
#define AS_MAX_CLIQUE_H

// Required by CPLEX:
#ifndef IL_STD
#define IL_STD
#endif

#include <ilcplex/ilocplex.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <vector>
#include <cstring>
#include <type_traits>

extern "C" {
    #include <libpmc.h>
};

#include "graph.h"
#include "numeric.h"

namespace as {
    namespace details {
        // The following two TMP definitions check for a ::weight member in a struct.
        // This is used to determine whether the graph has a vertex property which
        // has such a member. In that case, we solve the weighted version of the problem.
        
        template<typename T, typename = float>
        struct has_weight : std::false_type {};

        template<typename T>
        struct has_weight<T, decltype((void) T::weight, 0.0f)> : std::true_type {};

        // The get_weight function gets the ::weight member of a struct, if such
        // a member exist and is publicly accessible. Otherwise, it returns 1.0f.

        template<typename VertexProperty>
        float get_weight(std::false_type, const VertexProperty&) {
            return 1.0f;
        }

        template<typename VertexProperty>
        float get_weight(std::true_type, const VertexProperty& prop) {
            return prop.weight;
        }

        template<typename VertexProperty>
        float get_weight(const VertexProperty& prop) {
            return get_weight(has_weight<VertexProperty>{}, prop);
        }
    }

    /** @namespace  max_clique
     *  @brief      This namespace contains utilities to solve the Maximum Clique Problem on boost graphs.
     */
    namespace max_clique {
        /** @brief  Solves the Maximum (Weight) Clique Problem via a simple MIP model through CPLEX.
         *
         *  It supports solving two variants of the problem:
         *
         *  * The MCP, where we simply try to find a clique of maximal size (i.e., number of vertices).
         *  * The MWCP, where we associate a weight to each vertex, and we attempt to find a clique
         *    with maximal sum of the weights of its vertices.
         *
         *  The detection of the problem to solve is automatic: if the \ref BoostGraph has a vertex
         *  property with a publicly accessible weight member, then we solve the weighted version.
         *  Otherwise, we solve the unweighted version.
         *
         *  @tparam BoostGraph  The underlying graph type.
         *  @param  g           The graph.
         *  @return             The largest clique in the graph.
         */
        template<typename BoostGraph>
        inline std::vector<typename boost::graph_traits<BoostGraph>::vertex_descriptor> solve_with_mip(const BoostGraph& g, std::optional<float> timeout = std::nullopt) {
            static_assert(
                std::is_same<typename BoostGraph::vertex_list_selector, boost::vecS>::value,
                "solve_with_mip relies on vertices to be stored in a vertex to map their indices to the variables' indices."
            );

            IloEnv env;
            IloModel model{env};
            const auto n = boost::num_vertices(g);

            assert(numeric::can_type_fit_value<IloInt>(n));
            IloNumVarArray x{env, static_cast<IloInt>(n)};

            IloExpr expr{env};

            for(const auto& v : graph::vertices(g)) {
                x[v] = IloNumVar{env, 0, 1, IloNumVar::Bool};
                expr += details::get_weight(g[v]) * x[v];
            }

            model.add(IloObjective{env, expr, IloObjective::Maximize});
            expr.end();

            for(auto v = 0u; v < n; ++v) {
                for(auto w = v + 1; w < n; ++w) {
                    if(!graph::are_connected(v, w, g)) {
                        model.add(x[v] + x[w] <= 1);
                    }
                }
            }

            IloCplex cplex{model};
            IloBool solved{false};

            if(timeout) {
                cplex.setParam(IloCplex::Param::TimeLimit, *timeout);
            }

            try {
                solved = cplex.solve();
            } catch(IloException& e) {
                env.end();
                throw std::runtime_error("Cplex crashed when solving the problem");
            }

            if(!solved) {
                cplex.exportModel("error.lp");
                env.end();
                throw std::runtime_error("Cplex could not find a solution for this instance of the problem");
            }

            std::vector<typename boost::graph_traits<BoostGraph>::vertex_descriptor> clique;

            for(const auto& v : graph::vertices(g)) {
                if(details::get_weight(g[v]) && cplex.getValue(x[v]) > 0) {
                    clique.push_back(v);
                }
            }

            env.end();
            return clique;
        };

        template<typename BoostGraph>
        inline std::vector<typename boost::graph_traits<BoostGraph>::vertex_descriptor> solve_with_pmc(const BoostGraph& g) {
            static_assert(
                    std::is_same<typename BoostGraph::vertex_list_selector, boost::vecS>::value,
                    "solve_with_pmc relies on vertices to be stored in a vertex to map their indices to the variables' indices."
            );

            const long long number_of_edges = boost::num_edges(g);
            std::vector<int> edge_tails;
            std::vector<int> edge_heads;

            edge_tails.reserve(number_of_edges);
            edge_heads.reserve(number_of_edges);

            for(const auto& edge : graph::edges(g)) {
                const auto tail = boost::source(edge, g);
                const auto head = boost::target(edge, g);

                assert(numeric::can_type_fit_value<int>(head));
                assert(numeric::can_type_fit_value<int>(tail));

                const int first = std::max(static_cast<int>(head), static_cast<int>(tail));
                const int second = std::min(static_cast<int>(head), static_cast<int>(tail));

                edge_tails.push_back(first);
                edge_heads.push_back(second);
            }

            const auto num_vertices = boost::num_vertices(g);

            assert(numeric::can_type_fit_value<int>(num_vertices));

            const auto max_clique_size = static_cast<int>(num_vertices);
            auto* clique = new int[max_clique_size];
            std::memset(clique, -1, sizeof *clique);

            auto clique_sz = ::max_clique(number_of_edges, edge_tails.data(), edge_heads.data(), 0, max_clique_size, clique);

            std::vector<typename boost::graph_traits<BoostGraph>::vertex_descriptor> ret_clique;
            int i = 0;

            while(clique[i] >= 0 && i < clique_sz && i < max_clique_size) {
                ret_clique.push_back(clique[i++]);
            }

            delete[] clique;

            return ret_clique;
        }
    }
}

#endif //AS_MAX_CLIQUE_H
