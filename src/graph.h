//
// Created by alberto on 15/10/17.
//

#ifndef AS_GRAPH_H
#define AS_GRAPH_H

#include "iterator_pair.h"
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

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
        inline iterator_pair<typename boost::graph_traits<BoostGraph>::vertex_iterator> vertices(const BoostGraph& graph) {
            return make_iter(boost::vertices(graph));
        }

        /** @brief  Gives an \ref iterator_pair that can be used in range-based for loops to cycle
         *          through the edges of a graph.
         *
         *  @tparam BoostGraph   The underlying graph type.
         *  @param graph         The graph.
         *  @return              The iterator_pair.
         */
        template<class BoostGraph>
        inline iterator_pair<typename boost::graph_traits<BoostGraph>::edge_iterator> edges(const BoostGraph& graph) {
            return make_iter(boost::edges(graph));
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
        inline iterator_pair<typename boost::graph_traits<BoostGraph>::out_edge_iterator> out_edges(
            const typename boost::graph_traits<BoostGraph>::vertex_descriptor& vertex,
            const BoostGraph& graph
        ) {
            return make_iter(boost::out_edges(vertex, graph));
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
        inline iterator_pair<typename boost::graph_traits<BoostGraph>::in_edge_iterator> in_edges(
            const typename boost::graph_traits<BoostGraph>::vertex_descriptor& vertex,
            const BoostGraph& graph
        ) {
            return make_iter(boost::in_edges(vertex, graph));
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
        inline iterator_pair<typename boost::graph_traits<BoostGraph>::adjacency_iterator> neighbours(
            const typename boost::graph_traits<BoostGraph>::vertex_descriptor& vertex,
            const BoostGraph& graph
        ) {
            static_assert(
                std::is_same<typename boost::graph_traits<BoostGraph>::directed_category, boost::undirected_tag>::value,
                "neighbours is intended to be used with undirected graphs."
            );
            return make_iter(boost::adjacent_vertices(vertex, graph));
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
    }
}

#endif //AS_GRAPH_H
