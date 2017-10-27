//
// Created by alberto on 15/10/17.
//

#ifndef AS_GRAPH_H
#define AS_GRAPH_H

#include "iterator_pair.h"
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

namespace as {
    /**
     * This namespace provides functions which work with boost graphs.
     */
    namespace graph {
        /**
         * Gives an iterator_pair that can be used in range-based for loops to cycle
         * through the vertices of a graph.
         *
         * @tparam BoostGraph   The underlying graph type.
         * @param graph         The graph.
         * @return              The iterator_pair.
         */
        template<class BoostGraph>
        inline iterator_pair<typename boost::graph_traits<BoostGraph>::vertex_iterator> vertices(const BoostGraph& graph) {
            return make_iter(boost::vertices(graph));
        }

        /**
         * Gives an iterator_pair that can be used in range-based for loops to cycle
         * through the edges of a graph.
         *
         * @tparam BoostGraph   The underlying graph type.
         * @param graph         The graph.
         * @return              The iterator_pair.
         */
        template<class BoostGraph>
        inline iterator_pair<typename boost::graph_traits<BoostGraph>::edge_iterator> edges(const BoostGraph& graph) {
            return make_iter(boost::edges(graph));
        }

        /**
         * Gives an iterator_pair that can be used in range-based for loops to cycle
         * through the out-edges of a vertex of a graph.
         *
         * @tparam BoostGraph   The underlying graph type.
         * @param graph         The graph.
         * @param vertex        The vertex.
         * @return              The iterator_pair.
         */
        template<class BoostGraph>
        inline iterator_pair<typename boost::graph_traits<BoostGraph>::out_edge_iterator> out_edges(
            const typename boost::graph_traits<BoostGraph>::vertex_descriptor& vertex,
            const BoostGraph& graph
        ) {
            return make_iter(boost::out_edges(vertex, graph));
        }

        /**
         * Gives an iterator_pair that can be used in range-based for loops to cycle
         * through the in-edges of a vertex of a graph.
         *
         * @tparam BoostGraph   The underlying graph type.
         * @param graph         The graph.
         * @param vertex        The vertex.
         * @return              The iterator_pair.
         */
        template<class BoostGraph>
        inline iterator_pair<typename boost::graph_traits<BoostGraph>::in_edge_iterator> in_edges(
            const typename boost::graph_traits<BoostGraph>::vertex_descriptor& vertex,
            const BoostGraph& graph
        ) {
            return make_iter(boost::in_edges(vertex, graph));
        }

        /**
         * Tells whether two edges of an undirected graph are incident to
         * at least one common vertex.
         *
         * @tparam BoostGraph   The underlying graph type. Must be undirected.
         * @param edge1         The first edge.
         * @param edge2         The second edge.
         * @param graph         The graph.
         * @return              True iff the two edges are incident to at least on common vertex.
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
    }
}

#endif //AS_GRAPH_H
