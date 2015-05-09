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

    void print_copy_to_orig(unsigned int indent = 0);
};


class Isostatic_Graph_Realizer {
public:
    // assumes that input is isostatic
    Isostatic_Graph_Realizer(Graph *g);

    bool realize();

private:
    Graph *in_graph;

    // used by methods
    std::vector<std::pair<Vertex_ID, Vertex_ID> > contractions;
    std::map<Vertex_ID, bool> realized;

    // makes a copy of in_graph,
    // destructively determines if it's partial 2-tree (series parallel, k4 minor avoiding...)
    bool is_partial_2_tree(graph_copy &gc);

    // returns a list of dropped edges
    std::vector<std::pair<Vertex_ID, Vertex_ID> > make_partial_2_tree();

    // assumes in_graph is now a partial 2 tree
    // returns a list of added edges
    // std::vector<Edge_ID> make_2_tree();
    std::vector<std::pair<Vertex_ID, Vertex_ID> > make_2_tree();

    // assumes in_graph is a 2 tree
    // uses edge lengths and directly edits vertex properties (x and y)
    // returns true if it can be realized, false otherwise
    bool realize_2_tree();
    bool _realize_2_tree_aux(Vertex_ID v0, Vertex_ID v1);
};

#endif
