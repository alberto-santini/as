//
// Created by alberto on 09/05/18.
//

#ifndef AS_ALNS_ACCEPTANCE_H
#define AS_ALNS_ACCEPTANCE_H

#include "alns.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace as {
    namespace alns {
        enum class MainTerminationCriterion {
            ITERATIONS,
            TIME
        };

        template<class Solution>
        struct LinearRecordToRecordTravel {
            MainTerminationCriterion main_termination_criterion;

            std::size_t iterations_limit;
            float time_limit;

            float start_threshold;
            float end_threshold;

            LinearRecordToRecordTravel() :
                main_termination_criterion{MainTerminationCriterion::ITERATIONS},
                iterations_limit{1'000'000u},
                time_limit{3600},
                start_threshold{0.1},
                end_threshold{0.0} {}

            LinearRecordToRecordTravel(std::string params_file) {
                using namespace boost::property_tree;

                ptree pt;
                read_json(params_file, pt);

                try {
                    auto termination_cr = pt.get<std::string>("acceptance.main_termination_criterion");

                    if(termination_cr == "iterations") {
                        main_termination_criterion = MainTerminationCriterion::ITERATIONS;
                    } else if(termination_cr == "time") {
                        main_termination_criterion = MainTerminationCriterion::TIME;
                    }
                } catch(...) {
                    main_termination_criterion = MainTerminationCriterion::ITERATIONS;
                }

                try {
                    iterations_limit = pt.get<std::size_t>("iterations_limit");
                } catch(...) {
                    iterations_limit = 1'000'000u;
                }

                try {
                    time_limit = pt.get<float>("time_limit");
                } catch(...) {
                    time_limit = 3600;
                }

                try {
                    start_threshold = pt.get<float>("acceptance.start_threshold");
                } catch(...) {
                    start_threshold = 0.1f;
                }

                try {
                    end_threshold = pt.get<float>("acceptance.end_threshold");
                } catch(...) {
                    end_threshold = 0.0f;
                }
            }

            bool operator()(AlgorithmStatus<Solution>& status) {
                float threshold;

                if(main_termination_criterion == MainTerminationCriterion::ITERATIONS) {
                    threshold = start_threshold + (start_threshold - end_threshold) * (iterations_limit - status.get_iteration_number());
                } else {
                    threshold = start_threshold + (start_threshold - end_threshold) * (time_limit - status.get_elapsed_time_sec());
                }

                const auto gap = (status.get_new_solution().cost() - status.get_best_solution().cost()) / status.get_new_solution().cost();

                return (gap <= threshold);
            }
        };
    }
}

#endif //AS_ALNS_ACCEPTANCE_H
