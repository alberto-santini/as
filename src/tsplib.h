//
// Created by alberto on 20/11/17.
//

#ifndef AS_TSPLIB_H
#define AS_TSPLIB_H

#include <map>
#include <regex>
#include <cmath>
#include <string>
#include <vector>
#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <experimental/filesystem>

#include "point.h"
#include "console.h"
#include "string.h"

namespace as {
    /** @namespace  tsplib
     *  @brief      This namespace contains classes and functions related to the TSPLIB.
     *
     *              The TSPLIB is a classical collection of instances of the Travelling
     *              Salesman Problem. The instance files have a perverted format which
     *              is vexing to parse. These utilities aim at making the parsing of the
     *              instances easier for the user.
     */
    namespace tsplib {
        namespace detail {
            /** @class TSPLIBInput
             *  @brief  This class represents the syntactic structure of a TSPLIB Instance,
             *          as it is read from a TSPLIB input file.
             *
             *          The instance has two parts: a specification part which gives metadata
             *          on the instance, and a data part which contains the actual instance
             *          data. The first part contains one key-value pair per line, separated
             *          by a colon and with whitespace wherever the original authors felt like
             *          putting it. The second part contains blocks made by a label on a single
             *          line, followed by multiple lines, each containing an unspecified number
             *          of numerical data. The number of lines following each label is also
             *          unspecified.
             */
            class TSPLIBInput {
                /** @brief The specification part.
                 */
                std::map<std::string, std::string> specification;

                /** @brief The data part.
                 */
                std::map<std::string, std::vector<float>> data;

            public:

                /** @brief      Adds a key-value pair to the specification part.
                 *
                 *  @param key   The key.
                 *  @param value The value.
                 */
                void add_specification(std::string key, std::string value) {
                    string::trim(key);
                    string::trim(value);

                    if(specification.find(key) != specification.end()) {
                        std::cerr << console::error << "Duplicate specification keyword: " << key << "\n";
                        std::cerr << "\tAlready saved with value: " << specification.at(key) << "\n";
                        std::cerr << "\tTrying to update with value: " << value << "\n";
                        throw std::logic_error("Duplicate specification key");
                    }
                    specification[key] = value;
                }

                /** @brief      Adds a new, empty data bloack to the data part.
                 *
                 *  @param label The label.
                 */
                void add_data_block(std::string label) {
                    string::trim(label);

                    if(data.find(label) != data.end()) {
                        std::cerr << console::error << "Duplicate data label: " << label << "\n";
                        throw std::logic_error("Duplicate data label");
                    }
                    data[label] = std::vector<float>();
                }

                /** @brief          Adds a single value to a data block.
                 *
                 *  @param label    The data block label.
                 *  @param value    The value to add.
                 */
                void add_data_block_element(std::string label, float value) {
                    string::trim(label);

                    if(data.find(label) == data.end()) {
                        throw std::out_of_range("There is no such data label: " + label);
                    }
                    data[label].push_back(value);
                }

                /** @brief          Adds a whitespace-separated list of values to a data block.
                 *
                 *  @param label    The data block label.
                 *  @param elements A string containing a whitespace-separated list of values to add.
                 *  @return         The number of elements added.
                 */
                std::size_t add_data_block_elements(std::string label, const std::string& elements) {
                    string::trim(label);

                    if(data.find(label) == data.end()) {
                        throw std::out_of_range("There is no such data label: " + label);
                    }

                    std::stringstream ss(elements);
                    float element;
                    std::size_t n_added = 0u;

                    while(ss >> element) {
                        // We do not to use add_data_block_element to avoid
                        // the check on the label at each iteration.
                        data[label].push_back(element);
                        ++n_added;
                    }

                    return n_added;
                }

                /** @brief      Checks if a specification exists with the given key.
                 *
                 *  @param key  The key.
                 *  @return     True iff there is a specification with the given key.
                 */
                bool has_specification(std::string key) const {
                    return specification.find(key) != specification.end();
                }

                /** @brief          Checks if a data block exists with the given label.
                 *
                 *  @param label    The label.
                 *  @return         True iff there is a data block with the given label.
                 */
                bool has_data(std::string label) const {
                    return data.find(label) != data.end();
                }

                /** @brief      Deleted function, as the user can only use the
                 *              specialised versions.
                 */
                template<typename T>
                T get_specification(std::string key) const = delete;

                /** @brief      Gets the vector of values associated with a label
                 *              from the data part.
                 *  @param label The data label.
                 *  @return      The values.
                 */
                std::vector<float> get_data(std::string label) const {
                    if(data.find(label) == data.end()) {
                        throw std::out_of_range("There is no such data label: " + label);
                    }
                    return data.at(label);
                }
            };

            /** @brief      Gets an unsigned integer value from the specification part.
             *
             *  @param key  Specification key.
             *  @return     The value corresponding to the key, converted from string to size_t.
             */
            template<>
            inline std::size_t TSPLIBInput::get_specification(std::string key) const {
                if(specification.find(key) == specification.end()) {
                    throw std::out_of_range("There is no such specification key: " + key);
                }

                try {
                    return std::stoul(specification.at(key));
                } catch(std::invalid_argument&) {
                    std::cerr << console::error << "Value at key " << key << " is " << specification.at(key) << "\n";
                    std::cerr << "\tThis value is not convertible to std::size_t\n";
                    throw std::domain_error("Value is not convertible to std::size_t");
                }
            }

            /** @brief      Gets a single-precision floating point value from the specification part.
             *
             *  @param key  Specification key.
             *  @return     The value corresponding to the key, converted from string to float.
             */
            template<>
            inline float TSPLIBInput::get_specification(std::string key) const {
                if(specification.find(key) == specification.end()) {
                    throw std::out_of_range("There is no such specification key: " + key);
                }

                try {
                    return std::stof(specification.at(key));
                } catch(std::invalid_argument&) {
                    std::cerr << console::error << "Value at key " << key << " is " << specification.at(key) << "\n";
                    std::cerr << "\tThis value is not convertible to float\n";
                    throw std::domain_error("Value is not convertible to float");
                }
            }

            /** @brief      Gets a string value from the specification part.
             *
             *  @param key  Specification key.
             *  @return     The value corresponding to the key.
             */
            template<>
            inline std::string TSPLIBInput::get_specification(std::string key) const {
                if(specification.find(key) == specification.end()) {
                    throw std::out_of_range("There is no such specification key: " + key);
                }
                return specification.at(key);
            }

            /** @brief              Reads the syntax of a TSPLIB file and makes sure it is correct,
             *                      without delving into the semantics of what is being read.
             *
             *  @param tsplib_file  The path of the file with the TSPLIB instance specs.
             *  @return             A structure with the syntactic components of the instance.
             */
            inline TSPLIBInput read_tsplib_file(std::string tsplib_file) {
                std::ifstream ifs(tsplib_file);

                if(ifs.fail()) {
                    throw std::runtime_error("Cannot read from file: " + tsplib_file);
                }

                TSPLIBInput tsp;

                const std::regex specification_regex("([A-Z_]+)\\s*:\\s*(.+)");
                std::string line;
                std::size_t line_number = 0u;

                // We start reading the specification part, which must come first and
                // before the data part. Data in the specification part must be in the
                // format specified by specification_regex.
                while(std::getline(ifs, line)) {
                    ++line_number;

                    std::smatch match;

                    if(std::regex_match(line, match, specification_regex)) {
                        // We want the regex to match fully.
                        // match[0] will contain the whole string;
                        // match[1] will contain the first parenthesised match;
                        // match[2] will contain the second parenthesised match.
                        if(match.size() != 3u) {
                            std::cerr << console::error << "Invalid format for a specification line. The line is:\n";
                            std::cerr << "\t" << line << "\n";
                            throw std::runtime_error("Invalid specification line on line number " + std::to_string(line_number));
                        }

                        tsp.add_specification(match[1].str(), match[2].str());
                    } else {
                        // The specification part is probably over, we break and
                        // move to the data part.
                        break;
                    }
                }

                const std::regex data_label_regex("^\\s*([A-Z_]+)\\s*$");
                const std::string eof_marker = "EOF";
                std::string current_label = "";

                // We start the data part. The first line of it has already been read,
                // and used to break the previous loop. Therefore, we now switch the
                // loop structure from a while-do to a do-while. The labels for the data
                // part must be in the format specified by data_label_regex.
                do {
                    if(line == eof_marker) { break; }

                    std::smatch match;

                    if(std::regex_match(line, match, data_label_regex)) {
                        // We want the regex to match fully.
                        // match[0] will contain the whole string;
                        // match[1] will contain the first parenthesised match.
                        if(match.size() != 2u) {
                            std::cerr << console::error << "Invalid format for a data label line. The line is:\n";
                            std::cerr << "\t" << line << "\n";
                            throw std::runtime_error("Invalid data label line on line number " + std::to_string(line_number));
                        }

                        current_label = match[1].str();
                        tsp.add_data_block(current_label);
                    } else {
                        if(current_label == "") {
                            std::cerr << console::error << "Data line without a parent label. The line is:\n";
                            std::cerr << "\t" << line << "\n";
                            throw std::runtime_error("Invalid data line on line number " + std::to_string(line_number));
                        }

                        tsp.add_data_block_elements(current_label, line);
                    }

                    ++line_number;
                } while(std::getline(ifs, line));

                return tsp;
            }

            /** @brief  Constant to use for TSPLIB instances distance calculations.
             *          Similar enough to pi.
             */
            static constexpr float tsp_pi = 3.141596f;

            /** @brief  Constant to use for TSPLIB instances distance calculations.
             *          Similar enough to the average earth radius.
             */
            static constexpr float tsp_earth_radius = 6378.388f;

            /** @brief              Function to be used to get the latitude or longitude
             *                      from, respectively, the x or y coordinates of points
             *                      in TSPLIB instances with geographical distances.
             *
             *  @param coordinate   The x or y coordinate of the point.
             *  @return             The corresponding latitude or longitude of the point.
             */
            constexpr inline float latlon(float coordinate) {
                const auto deg = std::trunc(coordinate);
                const auto rem = coordinate - deg;
                return tsp_pi * (deg + 5.0f * rem / 3.0f) / 180.0f;
            }

            /** @brief          Gets the distance function to use to compute TSPLIB
             *                  distances from vertex coordinates.
             *
             *  @param f_name   The function name as it appears in TSPLIB files.
             *  @return         A function that takes four coordinates (x1, y1, x2, y2) and
             *                  returns the distance between points (x1, y1) and (x2, y2).
             */
            inline std::function<float(float,float,float,float)> get_distance_function(std::string f_name) {
                // Distances must be computed according to a specific formula
                // for the results to be comparable with other methods in the
                // literature. These formulas might not always seem the most
                // intuitive...
                if(f_name == "EUC_2D") {
                    return [] (float x1, float y1, float x2, float y2) -> float {
                        const auto xd = x1 - x2;
                        const auto yd = y1 - y2;
                        const auto d = std::sqrt(std::pow(xd, 2.0f) + std::pow(yd, 2.0f));

                        return std::round(d); // Notice the rounding here!
                    };
                } else if(f_name == "CEIL_2D") {
                    return [] (float x1, float y1, float x2, float y2) -> float {
                        const auto xd = x1 - x2;
                        const auto yd = y1 - y2;
                        const auto d = std::sqrt(std::pow(xd, 2.0f) + std::pow(yd, 2.0f));

                        return std::ceil(d); // Notice the ceiling here!
                    };
                } else if(f_name == "GEO") {
                    return [] (float x1, float y1, float x2, float y2) -> float {
                        const auto lat1 = latlon(x1), lon1 = latlon(y1);
                        const auto lat2 = latlon(x2), lon2 = latlon(y2);
                        const auto q1 = std::cos(lon1 - lon2);
                        const auto q2 = std::cos(lat1 - lat2);
                        const auto q3 = std::cos(lat1 + lat2);
                        const auto q = (1.0f + q1) * q2 - (1.0f - q1) * q3;
                        const auto qa = std::acos(0.5f * q);
                        const auto dist = tsp_earth_radius * qa + 1.0f;

                        return dist;
                    };
                } else if(f_name == "ATT") {
                    return [] (float x1, float y1, float x2, float y2) -> float {
                        const auto xd = x1 - x2;
                        const auto yd = y1 - y2;
                        const auto r = std::sqrt((std::pow(xd, 2.0f) + std::pow(yd, 2.0f)) / 10.0f);
                        const auto t = std::trunc(r);

                        if(t < r) {
                            return t + 1;
                        } else {
                            return t;
                        }
                    };
                } else {
                    throw std::domain_error("Distance function not supported: " + f_name);
                }
            }

            /** @brief  Returns the two intersection points between two circles.
             *
             *          It is assumed that the first circle is centred ad (0,0) and the
             *          second one at (\ref dist_centre_1_centre_2, 0). Furthermore, the
             *          radius of the first circle is \ref dist_centre_1_point and the
             *          radius of the second circle is \ref dist_centre_2_point.
             *          We also try to adjust eventual inconsistencies in the data.
             *
             *  @param dist_centre_1_point       Radius of the first circle.
             *  @param dist_centre_2_point       Radius of the second circle.
             *  @param dist_centre_1_centre_2    Distance between the circles.
             *  @return                          Coordinates of the two intersection points
             *                                   between the two circles.
             */
            inline std::pair<TwoDimPoint, TwoDimPoint> get_circle_intersection(
                float dist_centre_1_point,
                float dist_centre_2_point,
                float dist_centre_1_centre_2
            ) {
                if(dist_centre_1_point + dist_centre_2_point <= dist_centre_1_centre_2) {
                    // The distances violate the triangle inequality: we have to
                    // consider one as true, and adjust the other one. In other words,
                    // we impose that the triangle inequality is satisfied with
                    // equality, that is, the three points are aligned. So there
                    // will only be one intersection, lying on the x axis.
                    const auto adj_x = dist_centre_1_centre_2 - dist_centre_2_point;
                    return std::make_pair(TwoDimPoint{adj_x, 0.0f}, TwoDimPoint{adj_x, 0.0f});
                }

                const float delta = std::pow(dist_centre_1_point, 2.0f) -
                                    std::pow(dist_centre_2_point, 2.0f) +
                                    std::pow(dist_centre_1_centre_2, 2.0f);

                const float beta = 4.0f *
                                   std::pow(dist_centre_1_point, 2.0f) *
                                   std::pow(dist_centre_1_centre_2, 2.0f) -
                                   std::pow(delta, 2.0f);

                const float x = delta / (2.0f * dist_centre_1_centre_2);
                const float y = std::sqrt(std::abs(beta)) / (2.0f * dist_centre_1_centre_2);

                return std::make_pair(TwoDimPoint{x, y}, TwoDimPoint{x, -y});
            }

            /** @brief      Calculates the euclidean distance between two points.
             *
             *  @param pt1  The first point.
             *  @param pt2  The second point.
             *  @return     The euclidean distance.
             */
            inline float euclidean_dist(const TwoDimPoint& pt1, const TwoDimPoint& pt2) {
                return std::sqrt(
                    std::pow(pt1.x - pt2.x, 2.0f) +
                    std::pow(pt1.y - pt2.y, 2.0f)
                );
            }
        }

        /** @class TSPInstance
         *  @brief This class represents a valid instance of the TSP.
         */
        class TSPInstance {
        protected:

            /** @brief Input object used to access the info contained in the instance file.
             */
            detail::TSPLIBInput tsp;

            /** @brief The file name of the file which contained the instance.
             */
            std::string tsplib_file;

            /** @brief Number of vertices in the instance.
             */
            std::size_t n_vertices;

            /** @brief Vertex coordinates.
             */
            std::vector<TwoDimPoint> coordinates;

            /** @brief  Original vertex coordinates, as per how they appear in
             *          the instance file (if they appear at all).
             */
            std::vector<TwoDimPoint> original_coordinates;

            /** @brief Distance matrix.
             */
            std::vector<std::vector<float>> distances;

        public:

            /** @brief              Builds an instance from a TSPLIB file.
             *
             *  @param tsplib_file  The file containing the instance data.
             */
            TSPInstance(std::string tsplib_file) : tsplib_file{tsplib_file} {
                tsp = detail::read_tsplib_file(tsplib_file);
                n_vertices = tsp.get_specification<std::size_t>("DIMENSION");

                if(tsp.get_specification<std::string>("EDGE_WEIGHT_TYPE") == "EXPLICIT") {
                    set_explicit_weights();

                    if(tsp.has_data("NODE_COORD_SECTION")) {
                        set_coordinates();
                    } else {
                        reverse_engineer_coordinates();
                    }
                } else {
                    set_coordinates_and_weights();
                }
            }

            /** @brief  Gives the number of vertices in the graph.
             *
             *  @return The number of vertices in the graph.
             */
            std::size_t number_of_vertices() const {
                return n_vertices;
            }

            /** @brief          Gives the (x,y)-coordinates of a vertex.
             *
             *                  The coordinates are returned in a simple struct with
             *                  only two publicly-accessible members: x and y.
             *
             *  @param vertex   The vertex whose coordinate we want.
             *  @return         The (x,y)-coordinates.
             */
            TwoDimPoint get_coordinates(std::size_t vertex) const {
                if(vertex >= n_vertices) {
                    throw std::out_of_range("No such vertex: " + std::to_string(vertex));
                }
                return coordinates[vertex];
            }

            /** @brief          Gives the distance between two vertices in the graph.
             *
             * @param v1        The first vertex.
             * @param v2        The second vertex.
             * @return          The distance between the vertices.
             */
            float get_distance(std::size_t v1, std::size_t v2) const {
                if(v1 >= n_vertices) {
                    throw std::out_of_range("No such vertex: " + std::to_string(v1));
                }
                if(v2 >= n_vertices) {
                    throw std::out_of_range("No such vertex: " + std::to_string(v2));
                }
                return distances[v1][v2];
            }

        private:

            void set_explicit_weights() {
                const auto format = tsp.get_specification<std::string>("EDGE_WEIGHT_FORMAT");

                if(format == "UPPER_ROW") {
                    set_explicit_weights_upper_row();
                } else if(format == "LOWER_DIAG_ROW") {
                    set_explicit_weights_lower_diag_row();
                } else {
                    throw std::domain_error("Weight format not supported: " + format);
                }
            }

            void set_explicit_weights_upper_row() {
                distances.resize(n_vertices, std::vector<float>(n_vertices, 0.0f));

                const auto weights = tsp.get_data("EDGE_WEIGHT_SECTION");
                std::size_t w_index = 0u;

                for(auto i = 0u; i < n_vertices; ++i) {
                    for(auto j = i + 1; j < n_vertices; ++j) {
                        distances[i][j] = weights[w_index];
                        distances[j][i] = weights[w_index];
                        ++w_index;

                        if(w_index > weights.size()) {
                            throw std::out_of_range(
                                "Index " + std::to_string(w_index) +
                                " out of range for EDGE_WEIGHT_SECTION, which has " +
                                std::to_string(weights.size()) + " elements"
                            );
                        }
                    }
                }

                if(w_index != weights.size()) {
                    throw std::logic_error(
                        "Did not read all EDGE_WEIGHT_SECTION values, only " +
                        std::to_string(w_index + 1) + " out of " +
                        std::to_string(weights.size())
                    );
                }
            }

            void set_explicit_weights_lower_diag_row() {
                distances.resize(n_vertices, std::vector<float>(n_vertices, 0.0f));

                const auto weights = tsp.get_data("EDGE_WEIGHT_SECTION");
                auto i = 0u;
                auto j = 0u;

                for(const auto& weight : weights) {
                    if(i >= n_vertices) {
                        throw std::out_of_range("Value for i is " + std::to_string(i) + " for a vector of size " + std::to_string(n_vertices));
                    }

                    if(j >= n_vertices) {
                        throw std::out_of_range("Value for j is " + std::to_string(j) + " for a vector of size " + std::to_string(n_vertices));
                    }

                    distances[i][j] = weight;
                    distances[j][i] = weight;

                    ++j;

                    if(j > i) {
                        ++i;
                        j = 0u;
                    }
                }
            }

            void set_coordinates() {
                const auto type = tsp.get_specification<std::string>("EDGE_WEIGHT_TYPE");

                if(type == "GEO") {
                    set_coordinates_geo();
                } else {
                    set_coordinates_euclidean();
                }
            }

            void set_coordinates_and_weights() {
                const auto type = tsp.get_specification<std::string>("EDGE_WEIGHT_TYPE");

                if(type == "GEO") {
                    set_coordinates_and_weights_geo();
                } else {
                    set_coordinates_and_weights_euclidean();
                }
            }

            void set_coordinates_and_weights_geo() {
                set_coordinates_geo(true);
            }

            void set_coordinates_geo(bool set_weights = false) {
                // First get the coordinates as if they were
                // Euclidean (x,y) coordinates, i.e. by just using
                // the raw values given in the instance file.
                set_coordinates_euclidean();

                // Save the coordinates as per how they are in the
                // file.
                original_coordinates = coordinates;

                // Then calculate the weights with these coordinates,
                // using the GEO function.
                if(set_weights) {
                    set_weights_from_coordinates();
                }

                // Then project the coords using an Azimuthal Equidistant Projection
                // centred on the depot. For more information see:
                // http://mathworld.wolfram.com/AzimuthalEquidistantProjection.html
                const auto eucl_coordinates = coordinates;
                const auto centre_lat = detail::latlon(eucl_coordinates[0].x);
                const auto centre_lon = detail::latlon(eucl_coordinates[0].y);

                coordinates[0] = {0.0f, 0.0f};

                for(auto i = 1u; i < n_vertices; ++i) {
                    const auto v_lat = detail::latlon(eucl_coordinates[i].x);
                    const auto v_lon = detail::latlon(eucl_coordinates[i].y);
                    const auto cos_c = std::sin(centre_lat) * std::sin(v_lat) +
                                       std::cos(centre_lat) * std::cos(v_lat) * std::cos(v_lon - centre_lon);
                    const auto c = std::acos(cos_c);
                    const auto k = c / std::sin(c);
                    const auto x = detail::tsp_earth_radius * k *
                                   (std::cos(centre_lat) * std::sin(v_lat) - std::sin(centre_lat) * std::cos(v_lat) * std::cos(v_lon - centre_lon));
                    const auto y = detail::tsp_earth_radius * k * std::cos(v_lat) * std::sin(v_lon - centre_lon);

                    coordinates[i] = {x, y};
                }
            }

            void set_coordinates_and_weights_euclidean() {
                set_coordinates_euclidean();
                set_weights_from_coordinates();
            }

            void set_coordinates_euclidean() {
                const auto coords = tsp.get_data("NODE_COORD_SECTION");
                coordinates.resize(n_vertices);

                // Coordinates come in triplets:
                // - The first number is the vertex id
                // - The second number is the x coordinate
                // - The third number is the y coordinate
                // Here we assume the vertex id is always sequential.
                for(auto i = 0u; i < coords.size(); i += 3) {
                    std::size_t vertex_id = static_cast<std::size_t>(coords[i]) - 1;

                    if(vertex_id != i / 3) {
                        throw std::logic_error("Node coordinates not given in sequential order");
                    }

                    coordinates[vertex_id] = {coords[i+1], coords[i+2]};
                }

                // We do not modify the original coordinates when they
                // are already euclidean.
                original_coordinates = coordinates;
            }

            void set_weights_from_coordinates() {
                const auto dist_f = detail::get_distance_function(tsp.get_specification<std::string>("EDGE_WEIGHT_TYPE"));
                distances.resize(n_vertices, std::vector<float>(n_vertices, 0.0f));

                for(auto i = 0u; i < n_vertices; ++i) {
                    for(auto j = i + 1; j < n_vertices; ++j) {
                        const auto dist = dist_f(
                            coordinates[i].x, coordinates[i].y,
                            coordinates[j].x, coordinates[j].y
                        );
                        distances[i][j] = dist;
                        distances[j][i] = dist;
                    }
                }
            }

            void reverse_engineer_coordinates() {
                coordinates.resize(n_vertices);

                assert(n_vertices >= 3u);

                // We place the first point on the origin.
                coordinates[0] = {0.0f, 0.0f};

                // We place the second point on the positive x-semiaxis.
                coordinates[1] = {distances[0][1], 0.0f};

                // We place the third point on the positive x-halfplane.
                coordinates[2] = detail::get_circle_intersection(distances[0][2], distances[1][2], distances[0][1]).first;

                // We place all other points in the position that better matches their distance
                // with the third point.
                for(auto i = 3u; i < n_vertices; ++i) {
                    const auto positions = detail::get_circle_intersection(
                        distances[0][i], distances[1][i], distances[0][1]
                    );

                    const auto distance_first = detail::euclidean_dist(coordinates[2], positions.first);
                    const auto distance_second = detail::euclidean_dist(coordinates[2], positions.second);

                    if(std::abs(distance_first - distances[2][i]) < std::abs(distance_second - distances[2][i])) {
                        coordinates[i] = positions.first;
                    } else {
                        coordinates[i] = positions.second;
                    }
                }

                // When we have to reverse-engineer the coordinates,
                // there are no original coordinates! :-)
                original_coordinates = std::vector<TwoDimPoint>();
            }
        };
    }
}

#endif //AS_TSLIB_H
