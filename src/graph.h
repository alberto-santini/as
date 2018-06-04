//
// Created by alberto on 15/10/17.
//

#ifndef AS_GRAPH_H
#define AS_GRAPH_H

#include "iterator_pair.h"
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <numeric>

namespace as {
    /** @namespace  graph
     *  @brief      This namespace provides functions which work with boost graphs.
     */
    namespace graph {
        /** @brief  Gives an \ref iterator_pair that can be used in range-based for loops to cycle
         *          through the vertices of a graph.
         *
         *  @tparam BoostGraph   The underlying graph type.
         *  @param  graph        The graph.
         *  @return              The iterator_pair.
         */
        template<class BoostGraph>
        inline iterators::iterator_pair<typename boost::graph_traits<BoostGraph>::vertex_iterator> vertices(const BoostGraph& graph) {
            return iterators::make_iter(boost::vertices(graph));
        }

        /** @brief  Gives an \ref iterator_pair that can be used in range-based for loops to cycle
         *          through the edges of a graph.
         *
         *  @tparam BoostGraph   The underlying graph type.
         *  @param graph         The graph.
         *  @return              The iterator_pair.
         */
        template<class BoostGraph>
        inline iterators::iterator_pair<typename boost::graph_traits<BoostGraph>::edge_iterator> edges(const BoostGraph& graph) {
            return iterators::make_iter(boost::edges(graph));
        }

        /** @brief  Gives an \ref iterator_pair that can be used in range-based for loops to cycle
         *          through the out-edges of a vertex of a graph.
         *
         *  @tparam BoostGraph   The underlying graph type.
         *  @param  graph        The graph.
         *  @param  vertex       The vertex.
         *  @return              The iterator_pair.
         */
        template<class BoostGraph>
        inline iterators::iterator_pair<typename boost::graph_traits<BoostGraph>::out_edge_iterator> out_edges(
            const typename boost::graph_traits<BoostGraph>::vertex_descriptor& vertex,
            const BoostGraph& graph
        ) {
            return iterators::make_iter(boost::out_edges(vertex, graph));
        }

        /** @brief  Gives an \ref iterator_pair that can be used in range-based for loops to cycle
         *          through the in-edges of a vertex of a graph.
         *
         *  @tparam BoostGraph   The underlying graph type.
         *  @param  vertex       The vertex.
         *  @param  graph        The graph.
         *  @return              The iterator_pair.
         */
        template<class BoostGraph>
        inline iterators::iterator_pair<typename boost::graph_traits<BoostGraph>::in_edge_iterator> in_edges(
            const typename boost::graph_traits<BoostGraph>::vertex_descriptor& vertex,
            const BoostGraph& graph
        ) {
            return iterators::make_iter(boost::in_edges(vertex, graph));
        }

        /** @brief  Gives an \ref iterator_pair that can be used in range-based for loops to cycle
         *          through the vertices adjacent to a given vertex, in an undirected graph.
         *
         *          Unlike boost::adjacent_vertices, this function statically checks that the
         *          graph is undirected.
         *
         *  @tparam BoostGraph      The underlying graph type.
         *  @param vertex           The vertex.
         *  @param graph            The graph.
         *  @return                 The iterator_pair.
         */
        template<class BoostGraph>
        inline iterators::iterator_pair<typename boost::graph_traits<BoostGraph>::adjacency_iterator> neighbours(
            const typename boost::graph_traits<BoostGraph>::vertex_descriptor& vertex,
            const BoostGraph& graph
        ) {
            static_assert(
                std::is_same<typename boost::graph_traits<BoostGraph>::directed_category, boost::undirected_tag>::value,
                "neighbours is intended to be used with undirected graphs."
            );
            return iterators::make_iter(boost::adjacent_vertices(vertex, graph));
        }

        /** @brief  Tells whether two edges of an undirected graph are incident to
         *          at least one common vertex.
         *
         *  @tparam BoostGraph   The underlying graph type. Must be undirected.
         *  @param  edge1        The first edge.
         *  @param  edge2        The second edge.
         *  @param  graph        The graph.
         *  @return              True iff the two edges are incident to at least on common vertex.
         */
        template<class BoostGraph>
        inline bool incident_to_the_same_vertex(
            const typename boost::graph_traits<BoostGraph>::edge_descriptor& edge1,
            const typename boost::graph_traits<BoostGraph>::edge_descriptor& edge2,
            const BoostGraph& graph
        ) {
            static_assert(
                std::is_same<typename boost::graph_traits<BoostGraph>::directed_category, boost::undirected_tag>::value,
                "incident_to_the_same_vertex is intended to be used with undirected graphs."
            );

            const auto s1 = boost::source(edge1, graph);
            const auto t1 = boost::target(edge1, graph);
            const auto s2 = boost::source(edge2, graph);
            const auto t2 = boost::target(edge2, graph);

            return (s1 == s2 || s1 == t2 || t1 == s2 || t1 == t2);
        }

        /** @brief Tells whether two vertices are connected by an edge (undirected
         *         graphs) or an arc (in either direction, for directed graphs).
         *
         *  @tparam BoostGraph  The underlying graph type. Can be directed or undirected.
         *  @param  vertex1     The first vertex.
         *  @param  vertex2     The second vertex.
         *  @param  graph       The graph.
         *  @return             True iff the vertices are connected by an edge or arc.
         */
        template<class BoostGraph>
        inline bool are_connected(
                const typename boost::graph_traits<BoostGraph>::vertex_descriptor& vertex1,
                const typename boost::graph_traits<BoostGraph>::vertex_descriptor& vertex2,
                const BoostGraph& graph
        ) {
            return boost::edge(vertex1, vertex2, graph).second || boost::edge(vertex2, vertex1, graph).second;
        }

        /** @brief  Tells whether a pair of arcs forms a simplicial pair.
         *
         *          Two arcs form a simplicial pair if they have the same tail and
         *          their heads are connected by another arc, in any direction.
         *
         *  @tparam BoostGraph  The underlying graph type. Must be directed.
         *  @param  arc1        The first arc.
         *  @param  arc2        The second arc.
         *  @param  graph       The graph.
         *  @return             True iff the two arcs form a simplicial pair.
         */
        template<class BoostGraph>
        inline bool is_simplicial_pair(
                const typename boost::graph_traits<BoostGraph>::edge_descriptor& arc1,
                const typename boost::graph_traits<BoostGraph>::edge_descriptor& arc2,
                const BoostGraph& graph
        ) {
            static_assert(
                    std::is_same<typename boost::graph_traits<BoostGraph>::directed_category, boost::directed_tag>::value,
                    "simplicial pairs only make sense in directed graphs"
            );

            return boost::source(arc1, graph) == boost::source(arc2, graph) &&
                   are_connected(boost::target(arc1, graph), boost::target(arc2, graph), graph);
        }

        /** @brief  Tells whether a vertex is an extreme of an edge, i.e.
         *          if it is either its source or its target. For an undirected
         *          graph, therefore, it tells whether the edge is incident
         *          to the vertex.
         *
         *  @tparam BoostGraph  The underlying graph type. Can be both directed or undirected.
         *  @param  vertex      The vertex.
         *  @param  edge        The edge.
         *  @param  graph       The graph.
         *  @return             True iff the vertex is an extreme of the edge.
         */
        template<class BoostGraph>
        inline bool is_extreme(
            const typename boost::graph_traits<BoostGraph>::vertex_descriptor& vertex,
            const typename boost::graph_traits<BoostGraph>::edge_descriptor& edge,
            const BoostGraph& graph
        ) {
            return (boost::source(edge, graph) == vertex || boost::target(edge, graph) == vertex);
        }

        /** @brief  Given an edge and a vertex which is one of the two extremes
         *          of the edge, it returns the other extreme.
         *
         *  @tparam BoostGraph  The underlying graph type. Can be both directed or undirected.
         *  @param  vertex      The vertex.
         *  @param  edge        The edge.
         *  @param  graph       The graph.
         *  @return             The other extreme of the edge.
         */
        template<class BoostGraph>
        inline typename boost::graph_traits<BoostGraph>::vertex_descriptor other_extreme(
            const typename boost::graph_traits<BoostGraph>::vertex_descriptor& vertex,
            const typename boost::graph_traits<BoostGraph>::edge_descriptor& edge,
            const BoostGraph& graph
        ) {
            assert(is_extreme(vertex, edge, graph));

            auto source = boost::source(edge, graph);

            if(vertex == source) {
                return boost::target(edge, graph);
            } else {
                return source;
            }
        }

        /** @brief  Given a subset of vertices, return the complement of the subset,
         *          i.e. all vertices not contained in the subset.
         *
         *  @tparam BoostGraph  The underlying graph type.
         *  @param vertices     The set of vertices, given as a vector.
         *  @param graph        The graph.
         *  @return             The complement set, returned as a vector.
         */
        template<class BoostGraph>
        inline std::vector<typename boost::graph_traits<BoostGraph>::vertex_descriptor> vertex_complement(
            std::vector<typename boost::graph_traits<BoostGraph>::vertex_descriptor> vertices,
            const BoostGraph& graph
        ) {
            static_assert(
                std::is_same<typename BoostGraph::vertex_list_selector, boost::vecS>::value,
                "vertex_complement only works when vertices are stored in a vector."
            );

            // Check that there are no duplicates in the vertex set passed by the user.
            assert(
                std::set<typename boost::graph_traits<BoostGraph>::vertex_descriptor>(
                    vertices.begin(), vertices.end()
                ).size() == vertices.size()
            );

            // Get the (ordered) set of all vertices in the graph.
            std::vector<typename boost::graph_traits<BoostGraph>::vertex_descriptor> all_vertices(boost::num_vertices(graph));
            std::iota(all_vertices.begin(), all_vertices.end(), typename boost::graph_traits<BoostGraph>::vertex_descriptor{});

            // Order the set of vertices passed by the user.
            std::sort(vertices.begin(), vertices.end());

            // Return the set difference between the two.
            std::vector<typename boost::graph_traits<BoostGraph>::vertex_descriptor> difference;
            std::set_difference(all_vertices.begin(), all_vertices.end(),
                                vertices.begin(), vertices.end(),
                                std::inserter(difference, difference.begin())
            );
            return difference;
        }

        /** @brief Computes an acyclic orientation of an undirected boost graph.
         *
         *  An acyclic orientation of an undirected graph consists in assigning an orientation
         *  to each of its edges, so that the resulting directed graph is acyclic.
         *
         *  Clearly, the starting graph should be undirected (boost::undirectedS).
         *  Furthermore, since the orientation depends on a total order relationship between
         *  the vertices, we constrain the starting graph to store vertices in a vector
         *  (boost::vecS), so that we can use their index as an ordering.
         *
         *  All other characteristics of the starting graph are preserved, including the
         *  types of the containers storing edges and out-edges, as well as vertex, edge,
         *  and graph properties.
         *  Since we preserve graph, edge, and vertex properties, we require them to be
         *  default-constructible and assignable.
         *
         *  @tparam OutEdgeListS        The starting boost graph out-edge storage container type.
         *  @tparam VertexProperties    The starting boost graph vertex property type.
         *  @tparam EdgeProperties      The starting boost graph edge property type.
         *  @tparam GraphProperty       The starting boost graph graph-property type.
         *  @tparam EdgeListS           The starting boost graph edge storage container type.
         *  @param  graph               The starting undirected graph.
         *  @return                     The acyclic orientation of the starting graph.
         */
        template<
                class OutEdgeListS = boost::vecS,             // boost's default
                class VertexProperties = boost::no_property,  // boost's default
                class EdgeProperties = boost::no_property,    // boost's default
                class GraphProperty = boost::no_property,     // boost's default
                class EdgeListS = boost::listS                // boost's default
        > inline boost::adjacency_list<OutEdgeListS, boost::vecS, boost::directedS, VertexProperties, EdgeProperties, GraphProperty, EdgeListS>
        acyclic_orientation(
                const boost::adjacency_list<OutEdgeListS, boost::vecS, boost::undirectedS, VertexProperties, EdgeProperties, GraphProperty, EdgeListS>& graph
        ) {
            boost::adjacency_list<OutEdgeListS, boost::vecS, boost::directedS, VertexProperties, EdgeProperties, GraphProperty, EdgeListS> digraph;

            for(const auto& v : vertices(graph)) {
                boost::add_vertex(graph[v], digraph);
            }

            for(auto i = 0u; i < boost::num_vertices(graph); ++i) {
                for(auto j = i + 1; j < boost::num_vertices(graph); ++j) {
                    const auto& [edge, edge_exists] = boost::edge(i, j, graph);

                    if(edge_exists) {
                        boost::add_edge(i, j, graph[edge], digraph);
                    }
                }
            }

            digraph[boost::graph_bundle] = graph[boost::graph_bundle];

            return digraph;
        }

        /** @brief  Returns the complement of an undirected graph, i.e. a
         *          graph with the same vertex set and with an edge beteween
         *          two vertices iff the original graph does not have such
         *          edge.
         *
         *          Since we preserve graph and vertex properties, we require
         *          them to be default constructible and assignable.
         *
         * @tparam  BoostGraph  The underlying graph type. Must be undirected.
         * @param   graph       The graph.
         * @return              The complement of the graph.
         */
        template<class BoostGraph>
        inline BoostGraph complementary(const BoostGraph& graph) {
            static_assert(
                    std::is_same<typename boost::graph_traits<BoostGraph>::directed_category, boost::undirected_tag>::value,
                    "complimentary only works on undirected graphs"
            );

            BoostGraph comp;

            for(const auto& v : vertices(graph)) {
                boost::add_vertex(graph[v], comp);
            }

            for(auto i = 0u; i < boost::num_vertices(graph); ++i) {
                for(auto j = i + 1; j < boost::num_vertices(graph); ++j) {
                    const bool edge_exists = boost::edge(i, j, graph).second;
                    if(!edge_exists) {
                        boost::add_edge(i, j, comp);
                    }
                }
            }

            comp[boost::graph_bundle] = graph[boost::graph_bundle];

            return comp;
        }
    }
}

#endif //AS_GRAPH_H
