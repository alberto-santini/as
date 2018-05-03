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
        inline std::vector<typename boost::graph_traits<BoostGraph>::vertex_descriptor> solve_with_mip(const BoostGraph& g) {
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
                    if(!graph::are_adjacent(v, w, g)) {
                        model.add(x[v] + x[w] <= 1);
                    }
                }
            }

            IloCplex cplex{model};
            IloBool solved{false};

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
    }
}

#endif //AS_MAX_CLIQUE_H
