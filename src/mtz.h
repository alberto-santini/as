//
// Created by alberto on 18/04/18.
//

#ifndef AS_MTZ_H
#define AS_MTZ_H

// Required by CPLEX:
#ifndef IL_STD
#define IL_STD
#endif

#include <ilcplex/ilocplex.h>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <numeric>
#include <map>

#include "tsplib.h"

namespace as {
    /** @namespace tsp
     *  @brief     This namespace contains utilities to solve the TSP.
     */
    namespace tsp {
        /** @brief Solves a TSP Instance using the Cplex solver via an MTZ model.
         *
         *  It can throw, in case Cplex fails to provide the optimal solution.
         *  If Cplex does not crash, in case of error, the corresponding model is saved to file "error.lp".
         *
         *  @param  instance The TSP instance.
         *  @param  nodes    The subset of nodes of the instance to consider.
         *  @return          The optimal tour.
         */
        inline std::vector<std::uint32_t> mtz_solve_tsp(const tsplib::TSPInstance& instance, const std::vector<std::uint32_t>& nodes) {
            const auto n = nodes.size();

            IloEnv env;
            IloModel model(env);
            IloArray<IloNumVarArray> x(env, n);
            IloNumVarArray u(env, n);
            IloExpr expr(env), fxpr(env);

            std::map<std::size_t, std::uint32_t> var_to_node;

            for(auto i = 0u; i < n; ++i) {
                var_to_node[i] = nodes[i];
                x[i] = IloNumVarArray(env, n);
                u[i] = IloNumVar(env, 1, n - 1, IloNumVar::Int);

                for(auto j = 0u; j < n; ++j) {
                    if(i == j) { continue; }

                    x[i][j] = IloNumVar(env, 0, 1, IloNumVar::Bool);
                    expr += x[i][j] * instance.get_distance(nodes[i], nodes[j]);
                }

                if(nodes[i] == 0u) {
                    u[i].setBounds(0, 0);
                }
            }

            model.add(IloObjective(env, expr));
            expr.clear();

            for(auto i = 0u; i < n; ++i) {
                for(auto j = 0u; j < n; ++j) {
                    if(i != j) {
                        expr += x[i][j];
                        fxpr += x[j][i];

                        if(i > 0u && j > 0u) {
                            model.add(u[i] - u[j] + 1 <= (n - 1) * (1 - x[i][j]));
                        }
                    }
                }
                model.add(expr == 1);
                model.add(fxpr == 1);
                expr.clear();
                fxpr.clear();
            }

            expr.end();
            fxpr.end();

            IloCplex cplex(model);

            IloBool solved = false;

            try {
                solved = cplex.solve();
            } catch(const IloException& e) {
                env.end();
                throw std::runtime_error("Cplex crashed when solving the problem");
            }

            if(!solved) {
                cplex.exportModel("error.lp");
                env.end();
                throw std::runtime_error("Cplex could not find a solution for this instance of the problem");
            }

            std::size_t current_index = 0u;
            std::vector<std::uint32_t> tour;
            do {
                tour.push_back(var_to_node[current_index]);

                for(auto i = 0u; i < n; ++i) {
                    if(i == current_index) { continue; }
                    if(cplex.getValue(x[current_index][i]) > 0.5) {
                        current_index = i;
                        break;
                    }
                }
            } while(current_index != 0u);

            env.end();
            return tour;
        }

        /** @brief Solves a TSP Instance using the Cplex solver via an MTZ model.
         *
         *  It can throw, in case Cplex fails to provide the optimal solution.
         *  If Cplex does not crash, in case of error, the corresponding model is saved to file "error.lp".
         *
         *  @param  instance The TSP instance.
         *  @return          The optimal tour.
         */
        inline std::vector<std::uint32_t> mtz_solve_tsp(const tsplib::TSPInstance& instance) {
            std::vector<std::uint32_t> nodes(instance.number_of_vertices());
            std::iota(nodes.begin(), nodes.end(), 0u);
            return mtz_solve_tsp(instance, nodes);
        }
    }
}

#endif //AS_MTZ_H
