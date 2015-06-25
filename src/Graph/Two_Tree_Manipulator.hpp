#ifndef TWO_TREE_MANIPULATOR_HPP
#define TWO_TREE_MANIPULATOR_HPP

#include "Graph.hpp"

#include <vector>
#include <list>
#include <map>
#include <utility>


// invalidated if you add or delete edges or vertices in the input graph
class Two_Tree_Manipulator : public Mapped_Graph_Copy {
public:
    // assumes that input is isostatic
    Two_Tree_Manipulator(const Graph& graph);
    ~Two_Tree_Manipulator();

    // in terms of the input graph
    const std::set<Edge_ID>& list_of_dropped() const;
    const std::vector<std::pair<Vertex_ID, Vertex_ID> >& list_of_added() const;

    // // in terms of this
    // const std::set<Edge_ID>& list_of_dropped() const;
    // const std::vector<std::pair<Vertex_ID, Vertex_ID> >& list_of_added() const;

    // assumes working_copy is now a 2 tree
    // uses edge lengths and directly edits vertex properties (x and y)
    std::list<Mapped_Graph_Copy*> realize_2_tree();

    // in terms of input graph
    bool check_dropped_edge_length(Edge_ID e);

    // checks all of them
    bool check_all_dropped_edge_lengths();
private:
    // in terms of the original... for caller, may want to know these things
    std::set<Edge_ID> in_graph_dropped;
    std::vector<std::pair<Vertex_ID, Vertex_ID> > in_graph_added;

    // in terms of the working_copy
    std::vector<std::pair<Vertex_ID, Vertex_ID> > wc_graph_dropped;
    std::set<Edge_ID> wc_graph_added;
    std::list<Edge_ID> wc_graph_added_ordered;



    // realization stuff
    struct realization_triplet {
        Vertex_ID v, v_parent_0, v_parent_1;
        realization_triplet(Vertex_ID v, Vertex_ID vp0, Vertex_ID vp1) :
            v(v), v_parent_0(vp0), v_parent_1(vp1) {}
    };
    std::vector<realization_triplet> realization_order;


    // destructively determines if graph is a partial 2-tree (series parallel, k4 minor avoiding...)
    static bool is_partial_2_tree(Graph* graph);

    // Returns a list of edges dropped from graph, such that it is now a partial 2-tree
    static std::vector<std::pair<Vertex_ID, Vertex_ID> > make_partial_2_tree(Graph* graph);

    // Assumes the input graph is a partial 2 tree
    // Returns a list of added edges in graph
    static std::list<Edge_ID> make_2_tree(Graph* graph);


    // returns a list of vertices that are incident to both v0 and v1 in graph g
    static std::vector<Vertex_ID> all_triangles_with_edge(
        const Graph &g,
        Vertex_ID v0,
        Vertex_ID v1);
    static std::vector<Vertex_ID> all_triangles_with_edge(
        const Graph &g,
        Vertex_ID v0,
        Vertex_ID v1,
        const std::set<Edge_ID>& edges_to_ignore);

    // determines a consistent order in which to realize a 2-tree
    static std::vector<realization_triplet> realize_2_tree_order(const Graph& graph);
    static void _realize_2_tree_order_aux(
        const Graph& graph,
        std::vector<realization_triplet> &already_ordered,
        Vertex_ID v0, Vertex_ID v1, Vertex_ID v_ignore);

    // Returns true if the solution is real, and false otherwise. If true, the
    // solutions are stored in (v2x0, v2y0) and (v2x1, v2y1).
    static bool triangulate(
        double v0x, double v0y,
        double v1x, double v1y,
        double e01, double e02, double e12,
        double *v2x0, double *v2y0,
        double *v2x1, double *v2y1);

    // returns the minimum and maximum value edge e could take, given the lengths
    // of all the other edges in the graph (excluding those in edges_to_ignore)
    static std::pair<double, double> interval_of_edge(
        const Graph& graph,
        Edge_ID e,
        const std::set<Edge_ID>& edges_to_ignore);
};

#endif
