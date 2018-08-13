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

extern "C" {
    #include <libpmc.h>
};

#include "graph.h"
#include "numeric.h"

namespace as {
    /** @namespace  max_clique
     *  @brief      This namespace contains utilities to solve the Maximum Clique Problem on boost graphs.
     */
    namespace max_clique {
        /** @brief  Solves the Maximum Clique Problem via a simple MIP model through CPLEX.
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
                expr += x[v];
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

            cplex.exportModel("error.lp");

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
                if(cplex.getValue(x[v]) > 0) {
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
