//
// Created by alberto on 09/05/18.
//

#include "src/alns.h"
#include "src/alns_acceptance.h"
#include "src/random.h"

#include <iostream>
#include <random>

struct Solution {
    float price;
    Solution() = default;
    Solution(float price) : price{price} {}
    float cost() const { return price; }
};

struct DestroySolution : public as::alns::DestroyMethod<Solution> {
    std::mt19937 mt;
    std::uniform_real_distribution<float> dist;

    DestroySolution() : mt{as::rnd::get_seeded_mt()}, dist{0.0f, 1.0f} {}

    void operator()(Solution& sol) override {
        sol.price += dist(mt);
    }
};

struct RepairSolution : public as::alns::RepairMethod<Solution> {
    std::mt19937 mt;
    std::uniform_real_distribution<float> dist;

    RepairSolution() : mt{as::rnd::get_seeded_mt()}, dist{0.0f, 1.0f} {}

    void operator()(Solution& sol) override {
        sol.price -= dist(mt);
    }
};

struct SampleVisitor {
    bool on_iteration_end(as::alns::AlgorithmStatus<Solution>& status) {
        if(status.get_iteration_number() % 100 == 0) {
            std::cout << status.get_iteration_number() << "\t";
            std::cout << status.get_best_solution().cost() << "\n";
            if(status.get_iteration_number() == 10000) {
                return false;
            }
        }

        return true;
    }
};

int main() {
    const Solution initial{100.0f};
    SampleVisitor visitor;

    as::alns::LinearRecordToRecordTravel<Solution> acceptance;
    acceptance.main_termination_criterion = as::alns::MainTerminationCriterion::ITERATIONS;
    acceptance.iterations_limit = 10000;
    acceptance.start_threshold = 0.05;
    acceptance.end_threshold = 0.0;

    const as::alns::AlgorithmParams params;

    as::alns::ALNSSolver<Solution, as::alns::LinearRecordToRecordTravel<Solution>, SampleVisitor> solver{params, initial};
    solver.set_acceptance_criterion(acceptance);
    solver.set_visitor(visitor);
    solver.add_destroy_method(std::make_unique<DestroySolution>());
    solver.add_repair_method(std::make_unique<RepairSolution>());

    solver.solve();

    return 0;
}