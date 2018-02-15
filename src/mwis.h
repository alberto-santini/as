//
// Created by alberto on 15/02/18.
//

#ifndef AS_MWIS_HPP
#define AS_MWIS_HPP

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <vector>

namespace as {
    /** @namespace  mwis
     *  @brief      This namespace provides a wrapper to Sewell's Maximum-Weight Independent Set library.
     *
     *  The MWIS is well-known problem in combinatorics and operational research.
     *  Given an undirected graph G = (V, E), in our case represented as a Boost Graph,
     *  and a mapping of each vertex to a certain weight >= 0, it asks to find the
     *  independent set of G, which maximises the sum of the weights of its element.
     *  A set S subset of V is called an independent set if, for any two vertices of S
     *  there is no edge connecting them.
     *
     *  Here we provide a wrapper to Sewell's library, which is distributed as part
     *  of Stephan Held's ``exactcolors'' suite: https://github.com/heldstephan/exactcolors
     */
    namespace mwis {
        namespace detail {
            extern "C" {
                #include <exactcolors/mwis_sewell/mwss.h>
                #include <exactcolors/mwis_sewell/mwss_ext.h>
            }
        }

        template<typename BoostGraph>
        inline std::vector<typename boost::graph_traits<BoostGraph>::vertex_descriptor> mwis(
            const std::vector<float>& weights,
            const BoostGraph& graph
        ) {
            using namespace mwis::detail;

            MWSSgraph m_graph;
            MWSSdata m_data;
            wstable_info m_info;
            wstable_parameters m_params;

            return std::vector<typename boost::graph_traits<BoostGraph>::vertex_descriptor>{};
        }
    }
}

#endif //AS_MWIS_HPP
