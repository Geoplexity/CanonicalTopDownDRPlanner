#ifndef ISOSTATIC_GRAPH_REALIZER_HPP
#define ISOSTATIC_GRAPH_REALIZER_HPP

#include "Graph.hpp"

#include <vector>
#include <list>
#include <map>
#include <utility>


class Isostatic_Graph_Realizer {
public:
    // assumes that input is isostatic
    Isostatic_Graph_Realizer(const Graph *g);
    ~Isostatic_Graph_Realizer();

    std::list<Mapped_Graph_Copy*> realize();


    Mapped_Graph_Copy *working_copy;

private:
    const Graph *in_graph;

    // used by methods
    // std::vector<std::pair<Vertex_ID, Vertex_ID> > contractions;
    // std::map<Edge_ID, bool> is_nonedge;
    // std::map<Edge_ID, std::pair<double, double> > nonedge_interval;
    // std::map<Vertex_ID, bool> realized;

    // destructively determines if gc.copy is a partial 2-tree (series parallel, k4 minor avoiding...)
    static bool is_partial_2_tree(Mapped_Graph_Copy &gc);

    // returns a list of dropped edges from in_graph, in terms of working_copy vertex_id's
    std::vector<std::pair<Vertex_ID, Vertex_ID> > make_partial_2_tree();

    // assumes working_copy is now a partial 2 tree
    // returns a list of added edges in working_copy
    // std::vector<Edge_ID> make_2_tree();
    std::list<Edge_ID> make_2_tree();

    // assumes working_copy is a 2 tree
    // uses edge lengths and directly edits vertex properties (x and y)
    // returns true if it can be realized, false otherwise
    std::list<Mapped_Graph_Copy*> realize_2_tree();
    // std::list<Mapped_Graph_Copy*> _realize_2_tree_aux(
    //     Mapped_Graph_Copy *g,
    //     Vertex_ID v0,
    //     Vertex_ID v1,
    //     std::map<Vertex_ID, bool> &already_realized);
    void _realize_2_tree_aux(
        std::list<Mapped_Graph_Copy*> &already_made,
        Vertex_ID orig_v0, Vertex_ID orig_v1, Vertex_ID orig_v_ignore);

    std::pair<double, double> interval_of_nonedge(
        std::set<Edge_ID> &nonedges,
        Edge_ID e);

    std::list<Mapped_Graph_Copy*> sample(
        std::set<Edge_ID> &nonedges,
        std::list<Edge_ID> &nonedges_ordered,
        std::vector<std::pair<Vertex_ID, Vertex_ID> > &dropped_edges,
        Edge_ID e);
};

#endif
