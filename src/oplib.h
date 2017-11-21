//
// Created by alberto on 21/11/17.
//

#ifndef AS_OPLIB_H
#define AS_OPLIB_H

#include <string>
#include <vector>

#include "tsplib.h"

namespace as {
    /** @namespace  oplib
     *  @brief      This namespace contains classes and functions related to the OPLIB.
     *
     *              The OPLIB is a collection of instances of the Orienteering Problem.
     *              The instance files have a perverted format which is vexing to parse,
     *              and is an extension of the TSPLIB format. These utilities aim at
     *              making the parsing of the instances easier for the user.
     */
    namespace oplib {
        class OPInstance : public tsplib::TSPInstance {
        protected:
            /** @brief Maximum travel time.
             */
            float max_travel_time;

            /** @brief List of prizes collectable at vertices.
             */
            std::vector<float> prizes;

        public:

            /** @brief              Builds an instance from an OPLIB file.
             *
             *  @param oplib_file   The file containing the instance data.
             */
            OPInstance(std::string oplib_file) : TSPInstance(oplib_file) {
                max_travel_time = tsp.get_specification<float>("COST_LIMIT");
                set_prizes();
            }

            float get_prize(std::size_t vertex) const {
                if(vertex >= n_vertices) {
                    throw std::out_of_range("No such vertex: " + std::to_string(vertex));
                }
                return prizes[vertex];
            }

        private:

            void set_prizes() {
                auto p_list = tsp.get_data("NODE_SCORE_SECTION");
                prizes.resize(n_vertices);

                // Coordinates come in couples:
                // - The first number is the vertex id
                // - The second number is the prize
                // Here we assume the vertex id is always sequential.
                for(auto i = 0u; i < p_list.size(); i += 2) {
                    std::size_t vertex_id = static_cast<std::size_t>(p_list[i]) - 1;

                    if(vertex_id != i / 2) {
                        throw std::logic_error("Node prizes not given in sequential order");
                    }

                    prizes[vertex_id] = p_list[i + 1];
                }
            }
        };
    }
}

#endif //AS_OPLIB_H
