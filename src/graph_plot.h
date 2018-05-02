//
// Created by alberto on 01/05/18.
//

#ifndef AS_GRAPH_PLOT_H
#define AS_GRAPH_PLOT_H

#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/graph_traits.hpp>
#include <cimg/CImg.h>

#include <optional>
#include <functional>
#include <vector>
#include <array>
#include <tuple>
#include <string>

#include "graph.h"

namespace as {
    /** @namespace  graph
     *  @brief      This namespace provides functions which work with boost graphs.
     */
    namespace graph {
        /** @namespace plot
         *  @brief     This namespace provides utilities to plot graphs to images.
         */
        namespace plot {
            /** @class PlottedGraph
             *  @brief A class representing a graph which is going to be plotted.
             *
             *  @tparam Graph   The underlying boost graph. We only require this
             *                  graph to have a vertex property with two publicly
             *                  accessible float members, .x and .y to get the
             *                  coordinates of each node.
             */
            template<typename Graph>
            class PlottedGraph {
                /** @brief Graph vertex descriptor.
                 */
                using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;

                /** @brief Graph edge descriptor.
                 */
                using Edge = typename boost::graph_traits<Graph>::edge_descriptor;

                /** @brief Data type representing a collection of vertices.
                 */
                using VertexCollection = std::vector<Vertex>;

                /** @brief Data type representing a collection of edged.
                 */
                using EdgeCollection = std::vector<Edge>;

                /** @brief Image type provided by CImg.
                 */
                using Image = cimg_library::CImg<unsigned char>;

                /** @brief List of colours from the fivethirtyeight theme.
                 */
                static constexpr std::array<std::array<unsigned char, 3>, 7> colours = {{
                        {   0,   0,   0},
                        {   0, 143, 213},
                        { 252,  79,  48},
                        { 229, 174,  56},
                        { 109, 144,  79},
                        { 139, 139, 139},
                        { 129,  15, 124}
                }};

                /** @brief Graph to plot.
                 */
                const Graph& graph;

                /** @brief  If the picture needs to be resized before plotting, this
                 *          is the desired width. If no width resizing is needed, this
                 *          is just nullopt.
                 */
                std::optional<std::uint32_t> resize_width;

                /** @brief  If the picrure needs to be resized before plotting, this
                 *          is the desired height. If no height resizing is needed, this
                 *          is just nullopt.
                 */
                std::optional<std::uint32_t> resize_height;

                /** @brief  Min x-coordinate of any vertex.
                 */
                float min_vertex_x;

                /** @brief  Max x-coordinate of any vertex.
                 */
                float max_vertex_x;

                /** @brief  Min y-coordinate of any vertex.
                 */
                float min_vertex_y;

                /** @brief  Max y-coordinate of any vertex.
                 */
                float max_vertex_y;

                /** @brief  Scaling factor on the x-coordinates.
                 */
                mutable float scaling_x;

                 /** @brief  Scaling factor on the y-coordinates.
                  */
                mutable float scaling_y;

                /** @brief  Number of pixels of padding around the borders of the image.
                 */
                std::uint32_t padding;

                /** @brief  Should the vertices be printed?
                 */
                bool print_vertices;

                /** @brief  Should the edges be printed?
                 */
                bool print_edges;

                /** @brief  Radius of the circle representing a vertex.
                 */
                std::uint32_t vertex_radius;

                /** @brief Collections of vertices that need to be printed in colour.
                 *
                 *  Vertices are normally printed in black. Vertices in these collections
                 *  are printed with other colours from the colour palette. All vertices
                 *  in the same collection are printed with the same colour. The palette
                 *  contains 6 colours; if more collections are passed, some colour will
                 *  have to be reused.
                 */
                std::vector<VertexCollection> highlight_vertices;

                /** @brief Collections of edges that need to be printed in colour.
                 *
                 *  Edges are normally printed in black. Edges in these collections are
                 *  printed with other colours from the colour palette. All edges in the
                 *  same collection are printed with the same colour. The palette contains
                 *  6 colours; if more collections are passed, some colour will have to
                 *  be reused.
                 */
                std::vector<EdgeCollection> highlight_edges;

            public:
                /** @brief Constructs a plottable graph from a boost graph.
                 *
                 *  @param graph    The boost graph.
                 */
                PlottedGraph(const Graph& graph) :
                        graph{graph},
                        print_vertices{true}, print_edges{true},
                        vertex_radius{3},
                        scaling_x{1}, scaling_y{1},
                        padding{20}
                {
                    std::tie(min_vertex_x, max_vertex_x) = minmax_dimensions([&] (const Vertex& v) { return graph[v].x; });
                    std::tie(min_vertex_y, max_vertex_y) = minmax_dimensions([&] (const Vertex& v) { return graph[v].y; });
                }

                /** @brief Sets the picture width.
                 *
                 *  This method returns a reference to the current object, so that
                 *  multiple methods can be chained.
                 *
                 *  @param width    The picture width.
                 *  @return         A reference to self.
                 */
                PlottedGraph& set_width(std::uint32_t width) {
                    resize_width = width;
                    return *this;
                }

                /** @brief Sets the picture height.
                 *
                 *  This method returns a reference to the current object, so that
                 *  multiple methods can be chained.
                 *
                 *  @param height   The picture height.
                 *  @return         A reference to self.
                 */
                PlottedGraph& set_height(std::uint32_t height) {
                    resize_height = height;
                    return *this;
                }

                /** @brief  Sets a global scaling coefficient.
                 * 
                 *  Individual scaling for each dimension can be overridden using
                 *  set_width and set_height.
                 *  This method returns a reference to the current object, so that
                 *  multiple methods can be chained.
                 * 
                 *  @param  factor  The scaling factor.
                 *  @return         A reference to self.
                 */
                PlottedGraph& set_scaling(float factor) {
                    scaling_x = factor;
                    scaling_y = factor;
                }

                /** @brief Sets the picture padding.
                 *
                 *  This method returns a reference to the current object, so that
                 *  multiple methods can be chained.
                 *
                 *  @param height   The picture padding.
                 *  @return         A reference to self.
                 */
                PlottedGraph& set_padding(std::uint32_t padding) {
                    this->padding = padding;
                    return *this;
                }

                /** @brief Sets the radius of the circle representing a vertex.
                 *
                 *  This method returns a reference to the current object, so that
                 *  multiple methods can be chained.
                 *
                 *  @param height   The vertex radius.
                 *  @return         A reference to self.
                 */
                PlottedGraph& set_vertex_radius(std::uint32_t vertex_radius) {
                    this->vertex_radius = vertex_radius;
                    return *this;
                }

                /** @brief Shows the vertices in the picture.
                 *
                 *  This method returns a reference to the current object, so that
                 *  multiple methods can be chained.
                 *
                 *  @return         A reference to self.
                 */
                PlottedGraph& show_vertices() {
                    print_vertices = true;
                    return *this;
                }

                /** @brief Hides the vertices from the picture.
                 *
                 *  This method returns a reference to the current object, so that
                 *  multiple methods can be chained.
                 *
                 *  @return         A reference to self.
                 */
                PlottedGraph& hide_vertices() {
                    print_vertices = false;
                    return *this;
                }

                /** @brief Shows the edges in the picture.
                 *
                 *  This method returns a reference to the current object, so that
                 *  multiple methods can be chained.
                 *
                 *  @return         A reference to self.
                 */
                PlottedGraph& show_edges() {
                    print_edges = true;
                    return *this;
                }

                /** @brief Hides the edges from the picture.
                 *
                 *  This method returns a reference to the current object, so that
                 *  multiple methods can be chained.
                 *
                 *  @return         A reference to self.
                 */
                PlottedGraph& hide_edges() {
                    print_edges = false;
                    return *this;
                }

                /** @brief Add a collection of highlighted vertices, which will be printed in colour.
                 *
                 *  This method returns a reference to the current object, so that
                 *  multiple methods can be chained.
                 *
                 *  @tparam Container   The type of container holding the vertices (e.g. vector, set).
                 *  @param  vertices    The collection of vertices to highlight.
                 *  @return             A reference to self.
                 */
                template<typename Container>
                PlottedGraph& add_highlighted_vertices(const Container& vertices) {
                    highlight_vertices.push_back(VertexCollection(std::begin(vertices), std::end(vertices)));
                    return *this;
                }

                /** @brief Add one highlighted vertex, which will be printed in colour.
                 *
                 *  This method returns a reference to the current object, so that
                 *  multiple methods can be chained.
                 *
                 *  @param  vertex      The vertex to highlight.
                 *  @return             A reference to self.
                 */
                PlottedGraph& add_highlighted_vertex(const Vertex& vertex) {
                    highlight_vertices.push_back({vertex});
                    return *this;
                }

                /** @brief Add a collection of highlighted edges, which will be printed in colour.
                 *
                 *  This method returns a reference to the current object, so that
                 *  multiple methods can be chained.
                 *
                 *  @tparam Container   The type of container holding the edges (e.g. vector, set).
                 *  @param  edges       The collection of edges to highlight.
                 *  @return             A reference to self.
                 */
                template<typename Container>
                PlottedGraph& add_highlighted_edges(const Container& edges) {
                    highlight_edges.push_back(EdgeCollection(std::begin(edges), std::end(edges)));
                    return *this;
                }

                /** @brief Add a highlighted edge, which will be printed in colour.
                 *
                 *  This method returns a reference to the current object, so that
                 *  multiple methods can be chained.
                 *
                 *  @param  edge        The edge to highlight.
                 *  @return             A reference to self.
                 */
                PlottedGraph& add_highlited_edge(const Edge& edge) {
                    highlight_edges.push_back({edge});
                    return *this;
                }

                /** @brief Plots the graph to file, in PNG format.
                 *
                 *  @param filename The path of the file where the png picture will be saved.
                 */
                void plot_png(const std::string& filename) const {
                    const auto original_width = max_vertex_x - min_vertex_x;
                    const auto original_height = max_vertex_y - min_vertex_y;
                    std::uint32_t image_width = original_width;
                    std::uint32_t image_height = original_height;

                    if(resize_width) {
                        scaling_x = static_cast<float>(*resize_width) / original_width;
                    }

                    image_width *= scaling_x;

                    if(resize_height) {
                        scaling_y = static_cast<float>(*resize_height) / original_height;
                    }

                    image_height *= scaling_y;

                    Image img(
                            static_cast<unsigned int>(image_width + 2 * padding),    // X dimension
                            static_cast<unsigned int>(image_height + 2 * padding),   // Y dimension
                            1u,                                                      // Z dimension
                            3u                                                       // Colour depth (RGB -> 3)
                    );

                    cimg_forXYZ(img, x, y, z) { img.fillC(x, y, z, 255, 255, 255); }

                    if(print_vertices) {
                        add_vertices_to(img);
                    }

                    if(print_edges) {
                        add_edges_to(img);
                    }

                    for(auto n = 0u; n < highlight_vertices.size(); ++n) {
                        const auto colour_n = (n % (colours.size() - 1u)) + 1u;
                        colour_vertices(highlight_vertices[n], colours[colour_n].data(), img);
                    }

                    for(auto n = 0u; n < highlight_edges.size(); ++n) {
                        const auto colour_n = (n % (colours.size() - 1u)) + 1u;
                        colour_edges(highlight_edges[n], colours[colour_n].data(), img);
                    }

                    img.save_png(filename.c_str());
                }

            private:
                std::pair<float, float> minmax_dimensions(const std::function<float(const Vertex&)>& dim) const {
                    const auto iters = boost::vertices(graph);
                    const auto minmax = std::minmax_element(iters.first, iters.second,
                        [&dim] (const Vertex& v, const Vertex& w) -> bool {
                            return dim(v) < dim(w);
                        }
                    );

                    return std::make_pair(dim(*minmax.first), dim(*minmax.second));
                }

                unsigned int get_vertex_canvas_x(const Vertex& v) const {
                    return static_cast<unsigned int>((graph[v].x - min_vertex_x) * scaling_x) + padding;
                }

                unsigned int get_vertex_canvas_y(const Vertex& v) const {
                    return static_cast<unsigned int>((graph[v].y - min_vertex_y) * scaling_y) + padding;
                }

                void draw_vertex(const Vertex& v, const unsigned char colour[3], Image& img) const {
                    img.draw_circle(get_vertex_canvas_x(v), get_vertex_canvas_y(v), vertex_radius, colour);
                }

                void draw_edge(const Edge& e, const unsigned char colour[3], Image& img) const {
                    const auto& orig = boost::source(e, graph);
                    const auto& dest = boost::target(e, graph);

                    const auto x1 = get_vertex_canvas_x(orig), y1 = get_vertex_canvas_y(orig);
                    const auto x2 = get_vertex_canvas_x(dest), y2 = get_vertex_canvas_y(dest);

                    // Draw a thick line:
                    img.draw_line(x1, y1, x2, y2, colour);
                    img.draw_line(x1 - 1, y1, x2 - 1, y2, colour);
                    img.draw_line(x1, y1 - 1, x2, y2 - 1, colour);
                }

                void add_vertices_to(Image& img) const {
                    const auto& black = colours[0].data();

                    for(const auto& v : as::graph::vertices(graph)) {
                        draw_vertex(v, black, img);
                    }
                }

                void add_edges_to(Image& img) const {
                    const auto& black = colours[0].data();

                    for(const auto& e : as::graph::edges(graph)) {
                        draw_edge(e, black, img);
                    }
                }

                void colour_vertices(const std::vector<Vertex>& vertices, const unsigned char colour[3], Image& img) const {
                    for(const auto& v : vertices) {
                        draw_vertex(v, colour, img);
                    }
                }

                void colour_edges(const std::vector<Edge>& edges, const unsigned char colour[3], Image& img) const {
                    for(const Edge& e : edges) {
                        draw_edge(e, colour, img);
                    }
                }
            };
        }
    }
}

#endif //AS_GRAPH_PLOT_H
