//
// Created by alberto on 09/05/18.
//

#ifndef AS_ALNS_H
#define AS_ALNS_H

#include <vector>
#include <random>
#include <string>
#include <chrono>
#include <memory>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "random.h"

namespace as {
    /** @namespace  alns
     *  @brief      Implementation of the Adaptive Large Neighbourhood Search metaheuristic
     *              framework and related utilities.
     */
    namespace alns {
        /** @brief  General ALNS parameters.
         */
        struct AlgorithmParams {
            /**
             * How fast does the score of destroy and repair methods change from an iteration
             * to the next, when it is updated.
             */
            float score_decay;

            /**
             * Destroy and repair methods score multiplier when the method found a new
             * best solution.
             */
            float new_best_multiplier;

            /**
             * Destroy and repair methods score multiplier when the method found a new
             * solution improving on the current (but not on the best).
             */
            float new_improving_multiplier;

            /**
             * Destroy and repair methods score multiplier when the method found a new
             * solution which ends up being accepted (but it's not better than the
             * current solution).
             */
            float new_accepted_multiplier;

            /** @brief Default constructor.
             */
            AlgorithmParams() :
                score_decay{0.9f},
                new_best_multiplier{10.0f},
                new_improving_multiplier{4.0f},
                new_accepted_multiplier{1.5f} {}

            /** @brief  Builds the params object from a json file.
             *
             *          Relevant parameters will be inside a "score" object at the root
             *          level of the json file, and will be named: score_decay,
             *          new_best_multiplier, new_improving_multiplier, and
             *          new_accepted_multiplier.
             *
             *  @param  params_file The json file with the parameter values.
             */
            AlgorithmParams(std::string params_file) {
                using namespace boost::property_tree;

                ptree pt;
                read_json(params_file, pt);

                try {
                    score_decay = pt.get<float>("scores.score_decay");
                } catch(...) {
                    score_decay = 0.9f;
                }

                try {
                    new_best_multiplier = pt.get<float>("scores.new_best_multiplier");
                } catch(...) {
                    new_best_multiplier = 10.0f;
                }

                try {
                    new_improving_multiplier = pt.get<float>("scores.new_improving_multiplier");
                } catch(...) {
                    new_improving_multiplier = 4.0f;
                }

                try {
                    new_accepted_multiplier = pt.get<float>("scores.new_accepted_multiplier");
                } catch(...) {
                    new_accepted_multiplier = 1.5f;
                }
            }
        };

        /** @brief  Virtual base class for destroy methods.
         *
         *          A destroy method needs to derive from this class and implement
         *          the operator(Solution&), which will destroy the solution in place.
         *
         *  @tparam Solution    The problem-specific solution type.
         */
        template<class Solution>
        struct DestroyMethod {
            /** @brief  Destroys a solution in place.
             */
            virtual void operator()(Solution&) = 0;
        };

        /** @brief  Virtual base class for repair methods.
         *
         *          A repair method needs to derive from this class and implement
         *          the operator(Solution&), which will repair the solution in place.
         *
         *  @tparam Solution    The problem-specific solution type.
         */
        template<class Solution>
        struct RepairMethod {
            /** @brief  Repairs a solution in place.
             */
            virtual void operator()(Solution&) = 0;
        };

        // Forward-declaration.
        template<
                class Solution,
                class AcceptanceCriterion,
                class AlgorithmVisitor
        > class ALNSSolver;

        /** @brief  A class containing current informations on the algorithm's
         *          status during the solution process.
         *
         *          This class groups together variables used by the
         *          \ref ALNSSolver class during its execution. The data is
         *          grouped together into this class, so that it can be passed
         *          to the algorithm visitor, thereby providing it with the
         *          possibility of controlling most parts of the algorithm.
         *
         *          The class has many getters (some constant) and setters, to
         *          mark the distinction between the elements which are accessible
         *          and editable by the visitor, and those which aren't.
         *
         * @tparam  Solution    The problem-specific solution type.
         */
        template<class Solution>
        class AlgorithmStatus {
            /**
             * The algorithm parameters.
             */
            const AlgorithmParams& params;

            /**
             * A pseud-random number generator.
             */
            std::mt19937 mt;

            /**
             * Current iteration number.
             */
            std::uint32_t iteration_number;

            /**
             * Current number of seconds elapsed.
             */
            float elapsed_time_sec;

            /**
             * Pointers to the destroy methods.
             */
            std::vector<std::unique_ptr<DestroyMethod<Solution>>> destroy_methods;

            /**
             * Pointers to the repair methods.
             */
            std::vector<std::unique_ptr<RepairMethod<Solution>>> repair_methods;

            /**
             * Destroy methods scores. Indices match those of \ref destroy_methods.
             */
            std::vector<float> destroy_scores;

            /**
             * Repair methods scores. Indices match those of \ref repair_methods.
             */
            std::vector<float> repair_scores;

            /**
             * Best solution encountered so far.
             */
            Solution best_solution;

            /**
             * Current solution.
             */
            Solution current_solution;

            /**
             * New solution produced by the destroy/repair methods at the current
             * iteration.
             */
            Solution new_solution;

            /**
             * Vector index of the last destroy method used.
             */
            std::size_t latest_destroy_id;

            /**
             * Vector index of the last repair method used.
             */
            std::size_t latest_repair_id;

            template<class FSolution, class AcceptanceCriterion, class AlgorithmVisitor>
            friend class ALNSSolver;

        public:
            /** @brief Builds the status used to start the algorithm.
             *
             *  @param params           Algorithm parameters.
             *  @param initial_solution Initial solution.
             */
            AlgorithmStatus(const AlgorithmParams& params, Solution initial_solution) :
                    params{params},
                    mt{as::rnd::get_seeded_mt()},
                    iteration_number{0u},
                    elapsed_time_sec{0.0f},
                    destroy_scores{std::vector<float>(destroy_methods.size(), 1.0f)},
                    repair_scores{std::vector<float>(repair_methods.size(), 1.0f)},
                    best_solution{initial_solution},
                    current_solution{initial_solution},
                    new_solution{initial_solution} {}

            /** @brief  Get the current iteration number.
             *
             *  @return The current iteration number.
             */
            std::size_t get_iteration_number() const {
                return this->iteration_number;
            }

            /** @brief  Get the current elapsed time in seconds.
             *
             *  @return The current elapsed time in seconds.
             */
            float get_elapsed_time_sec() const {
                return this->elapsed_time_sec;
            }

            /** @brief  Get an editable (non-const) reference to the vector of
             *          destroy methods.
             *
             *  @return The vector of destroy methods.
             */
            std::vector<std::unique_ptr<DestroyMethod<Solution>>>& get_destroy_methods() {
                return this->destroy_methods;
            }

            /** @brief  Get an editable (non-const) reference to the vector of
             *          repair methods.
             *
             *  @return The vector of repair methods.
             */
            std::vector<std::unique_ptr<RepairMethod<Solution>>>& get_repair_methods() {
                return this->repair_methods;
            }

            /** @brief  Get a const reference to the vector of scores of destroy methods.
             *
             *  @return The vector of scores of destroy methods.
             */
            const std::vector<float>& get_destroy_scores() const {
                return this->destroy_scores;
            }

            /** @brief  Get a const reference to the vector of scores of repair methods.
             *
             *  @return The vector of scores of repair methods.
             */
            const std::vector<float>& get_repair_scores() const {
                return this->repair_scores;
            }

            /** @brief  Get an editable (non-const) reference to the best solution
             *          encountered so far.
             *
             *  @return The best solution encountered so far.
             */
            Solution& get_best_solution() {
                return this->best_solution;
            }

            /** @brief  Get an editable (non-const) reference to the current solution.
             *
             *  @return The current solution.
             */
            Solution& get_current_solution() {
                return this->current_solution;
            }

            /** @brief  Get an editable (non-const) reference to the new solution,
             *          produced during the current iteration by applying the destroy
             *          and repair methods to the current solution.
             *
             *  @return The new solution.
             */
            Solution& get_new_solution() {
                return this->new_solution;
            }

        private:

            const std::unique_ptr<DestroyMethod<Solution>>& get_roulette_destroy() {
                latest_destroy_id = rnd::roulette_wheel(destroy_scores, mt);
                return destroy_methods[latest_destroy_id];
            }

            const std::unique_ptr<RepairMethod<Solution>>& get_roulette_repair() {
                latest_repair_id = rnd::roulette_wheel(repair_scores, mt);
                return repair_methods[latest_repair_id];
            }

            void update_score_best() {
                update_score(latest_destroy_id, params.new_best_multiplier, destroy_scores);
                update_score(latest_repair_id, params.new_best_multiplier, repair_scores);
            }

            void update_score_improving() {
                update_score(latest_destroy_id, params.new_improving_multiplier, destroy_scores);
                update_score(latest_repair_id, params.new_improving_multiplier, repair_scores);
            }

            void update_score_accepted() {
                update_score(latest_destroy_id, params.new_accepted_multiplier, destroy_scores);
                update_score(latest_repair_id, params.new_accepted_multiplier, repair_scores);
            }

            void update_score(std::size_t method_id, float multiplier, std::vector<float>& scores) {
                scores[method_id] *= params.score_decay;
                scores[method_id] += (1 - params.score_decay) * multiplier;
            }
        };

        /** @brief      An acceptance criterion used by default, if the user does not
         *              provide one. It just accepts all solutions.
         *
         *  @tparam     Solution    The problem-specific solution type.
         */
        template<class Solution>
        struct DefaultAcceptanceCriterion {
            /** @brief  Decides whether or not to accept a solution, given within
             *          an \ref AlgorithmStatus object.
             *
             * @return  True iff the new solution should replace the current one.
             */
            bool operator()(AlgorithmStatus<Solution>&) { return true; }
        };

        /** @brief      An algorithm visitor used by default, if the user does not
         *              provide one. It doesn't do anything.
         *
         *  @tparam     Solution    The problem-specific solution type.
         */
        template<class Solution>
        struct DefaultAlgorithmVisitor {
            /** @brief  Method called at the end of each iteration.
             *
             *          It can be used to inspect the algorithm status, apply
             *          some local search to the current or best solutions,
             *          decide to terminate the algorithm, gather statistics, etc.
             *
             *  @return False iff the algorithm should stop.
             */
            bool on_iteration_end(AlgorithmStatus<Solution>&) { return true; }
        };

        /** @brief  The ALNS solver.
         *
         *  @tparam Solution                The problem-specific solution type.
         *  @tparam AcceptanceCriterion     The acceptance criterion type.
         *  @tparam AlgorithmVisitor        The solution visitor type.
         */
        template<
                class Solution,
                class AcceptanceCriterion = DefaultAcceptanceCriterion<Solution>,
                class AlgorithmVisitor = DefaultAlgorithmVisitor<Solution>
        >
        class ALNSSolver {
            /**
             * Algorithm parameters.
             */
            AlgorithmParams params;

            /**
             * Algorithm visitor.
             */
            AlgorithmVisitor visitor;

            /**
             * Acceptance criterion.
             */
            AcceptanceCriterion acceptance;

            /**
             * Algorithm status.
             */
            AlgorithmStatus<Solution> status;

        public:
            /** @brief  Creates a new solver, given the parameters and an initial solution.
             *
             *  @param params               The parameters.
             *  @param initial_solution     The initial solution.
             */
            ALNSSolver(AlgorithmParams params, Solution initial_solution) :
                    params{params}, status{this->params, initial_solution} {}

            /** @brief  Resets the algorithm status, given a new initial solution.
             *
             *          When the algorithm solution process is stopped, the last
             *          encountered status is preserved, in case the user wants
             *          to modify the solver and resume the process.
             *          If, instead, the user wants to start a new solution process
             *          altogether, it needs to reset the algorithm status,
             *          invoking this solution.
             *          This will clear the iterations and elapsed time indicators,
             *          reset best and current solution to the initial one, and
             *          reset the destroy/repair methods scores.
             *
             * @param   initial_solution    The initial solution.
             */
            void reset_status(Solution initial_solution) {
                status = AlgorithmStatus<Solution>{params, initial_solution};
            }

            /** @brief  Gets an editable (non-const) reference to the
             *          algorithm status.
             *
             *  @return The algorithm status.
             */
            AlgorithmStatus<Solution>& get_status() {
                return status;
            }

            /** @brief  Sets new algorithm parameters.
             *
             *  @param params   The algorithm parameters.
             */
            void set_params(AlgorithmParams params) {
                this->params = params;
            }

            /** @brief  Gets the current algorithm parameters.
             *
             *  @return The current algorithm parameters.
             */
            const AlgorithmParams& get_params() const {
                return params;
            }

            /** @brief  Sets a new algorithm visitor.
             *
             *  @param  visitor The algorithm visitor.
             */
            void set_visitor(AlgorithmVisitor visitor) {
                this->visitor = visitor;
            }

            /** @brief  Sets a new acceptance criterion.
             *
             *  @param  acceptance  The acceptance criterion.
             */
            void set_acceptance_criterion(AcceptanceCriterion acceptance) {
                this->acceptance = acceptance;
            }

            /** @brief  Gets an editable (non-const) reference to the current
             *          acceptance criterion.
             *
             *  @return The current acceptance criterion.
             */
            AcceptanceCriterion& get_acceptance_criterion() {
                return acceptance;
            }

            /** @brief  Adds a new destroy method to the destroy methods pool.
             *
             *  @param  method  The destroy method.
             *  @return         The index of the newly added destroy method in the destroy methods vector.
             */
            std::size_t add_destroy_method(std::unique_ptr<DestroyMethod<Solution>> method) {
                assert(status.destroy_methods.size() == status.destroy_scores.size());
                status.destroy_methods.push_back(std::move(method));
                status.destroy_scores.push_back(1.0f);
                return status.destroy_methods.size() - 1u;
            }

            /** @brief  Adds a new repair method to the repair methods pool.
             *
             *  @param  method  The repair method.
             *  @return         The index of the newly added repair method in the repair methods vector.
             */
            std::size_t add_repair_method(std::unique_ptr<RepairMethod<Solution>> method) {
                assert(status.repair_methods.size() == status.repair_scores.size());
                status.repair_methods.push_back(std::move(method));
                status.repair_scores.push_back(1.0f);
                return status.repair_methods.size() - 1u;
            }

            /** @brief  Launches the algorithm.
             *
             *          The algorithm can be stopped via the visitor.
             *          The overall best solution can be accessed via the
             *          algorithm status.
             */
            void solve() {
                using namespace std::chrono;
                const auto start_time = high_resolution_clock::now();

                while(true) {
                    const auto& destroy = status.get_roulette_destroy();
                    const auto& repair = status.get_roulette_repair();

                    status.new_solution = status.current_solution;
                    (*destroy)(status.new_solution);
                    (*repair)(status.new_solution);

                    if(acceptance(status)) {
                        if(status.new_solution.cost() < status.current_solution.cost()) {
                            if(status.new_solution.cost() < status.best_solution.cost()) {
                                status.best_solution = status.new_solution;
                                status.update_score_best();
                            } else {
                                status.update_score_improving();
                            }
                        } else {
                            status.update_score_accepted();
                        }

                        status.current_solution = status.new_solution;
                    }

                    if(!visitor.on_iteration_end(status)) {
                        return;
                    }

                    const auto current_time = high_resolution_clock::now();
                    status.elapsed_time_sec = duration_cast<duration<float>>(current_time - start_time).count();
                    ++status.iteration_number;
                }
            }
        };
    }
}

#endif //AS_ALNS_H
