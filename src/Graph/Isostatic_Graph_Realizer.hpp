#ifndef ISOSTATIC_GRAPH_REALIZER_HPP
#define ISOSTATIC_GRAPH_REALIZER_HPP

#include "Graph.hpp"

#include <vector>
#include <map>
#include <utility>



struct graph_copy {
    Graph *orig;

    Graph copy;
    std::map<Vertex_ID, Vertex_ID> orig_to_copy, copy_to_orig;

    graph_copy(Graph *g);
};


class Isostatic_Graph_Realizer {
public:
    // assumes that input is isostatic
    Isostatic_Graph_Realizer(Graph *g);

    void realize();

private:
    Graph *in_graph;

    std::vector<std::pair<Vertex_ID, Vertex_ID> > contractions;

    // returns a list of dropped edges
    std::vector<std::pair<Vertex_ID, Vertex_ID> > make_partial_2_tree();

    // assumes in_graph is now a partial 2 tree
    // returns a list of added edges
    std::vector<Edge_ID> make_2_tree();

    // makes a copy of in_graph,
    // destructively determines if it's partial 2-tree (series parallel, k4 minor avoiding...)
    bool is_partial_2_tree(graph_copy &gc);
};

#endif
