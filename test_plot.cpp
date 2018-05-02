//
// Created by alberto on 01/05/18.
//

#include <gtest/gtest.h>

#include "src/graph_plot.h"
#include "src/tsplib.h"
#include "src/geometry.h"

namespace {
    class GraphPlotTest : public ::testing::Test {
    public:
        using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, as::geo::TwoDimPoint>;
        using Vertex = boost::graph_traits<Graph>::vertex_descriptor;
        using Edge = boost::graph_traits<Graph>::edge_descriptor;

        Graph graph;

        GraphPlotTest() {
            as::tsplib::TSPInstance instance("../test/tsplib/pr10.tsp");

            for(auto i = 0u; i < instance.number_of_vertices(); ++i) {
                boost::add_vertex(instance.get_coordinates(i), graph);
            }

            for(auto i = 0u; i < instance.number_of_vertices(); ++i) {
                for(auto j = i + 1; j < instance.number_of_vertices(); ++j) {
                    boost::add_edge(i, j, graph);
                }
            }
        }
    };

    TEST_F(GraphPlotTest, ProducesPNG) {
        using namespace as::graph::plot;

        const std::vector<Vertex> v1 = { 3u, 4u, 5u };
        const std::vector<Vertex> v2 = { 6u, 7u };

        const Edge e1 = boost::edge(1u, 2u, graph).first;
        const Edge e2 = boost::edge(2u, 5u, graph).first;
        const Edge e3 = boost::edge(5u, 1u, graph).first;
        const std::vector<Edge> tour = { e1, e2, e3 };

        PlottedGraph{graph}.plot_png("graph.png");
        PlottedGraph{graph}.set_width(1000).set_height(1000).plot_png("graph200x200.png");

        PlottedGraph{graph}
                .set_width(1000)
                .set_height(1000)
                .hide_edges()
                .add_highlighted_vertices(v1)
                .add_highlighted_vertices(v2)
                .plot_png("graph-hv.png");

        PlottedGraph{graph}
                .set_width(1000)
                .set_height(1000)
                .hide_edges()
                .add_highlighted_edges(tour)
                .plot_png("graph-he.png");

        // Manual test! Open the graph files and check them! :-D
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}