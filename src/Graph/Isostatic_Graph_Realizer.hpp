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
    Isostatic_Graph_Realizer(Graph *g);
    ~Isostatic_Graph_Realizer();

    // const Mapped_Graph_Copy* get_working_copy() {return working_copy;}

    // samples based on the current configuration of working_copy
    void sample(bool change_input_vertex_positions = false);

    // returns true until it can take no further steps
    bool step();

    std::list<Mapped_Graph_Copy*>& realizations();

    // in terms of the input graph
    const std::set<Edge_ID>& list_of_dropped() const;
    const std::vector<std::pair<Vertex_ID, Vertex_ID> >& list_of_added() const;
private:
    Graph *in_graph;
    Mapped_Graph_Copy *working_copy;

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
    std::list<Mapped_Graph_Copy*> _realizations;


    const double EPS = 1e-2;
    class IGR_Context {
    public:
        IGR_Context(
            Edge_ID copy_e,
            Edge_ID orig_e_paired,
            std::pair<Vertex_ID, Vertex_ID> orig_e_paired_as_copy_vs,
            std::pair<double, double> interval,
            unsigned int steps);

        // step goes to 0, sets new interval
        void reset(const double begin, const double end);

        // have we sampled the entire interval?
        bool done() {return (_step < steps)? false: true;}

        // increments to the next step
        void step() {_step++;}

        // returns the value at this step
        double point();

        //
        void add_solution(double solution);
    // private:

        // id for edge in copy
        Edge_ID copy_e;
        // dropped edge in original graph, arbitrarily paired with this edge
        Edge_ID orig_e_paired;
        // vertices of paired dropped edge, in terms of copy vertices
        std::pair<Vertex_ID, Vertex_ID> orig_e_paired_as_copy_vs;

        std::pair<double, double> _interval;
        unsigned int _step, steps;

        std::vector<double> solutions;
    };

    std::list<IGR_Context>           igr_contexts;
    std::list<IGR_Context>::iterator igr_context_current;


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

    // assumes working_copy is now a 2 tree
    // uses edge lengths and directly edits vertex properties (x and y)
    std::list<Mapped_Graph_Copy*> realize_2_tree();

    // returns the minimum and maximum value edge e could take, given the lengths
    // of all the other edges in the graph (excluding those in edges_to_ignore)
    static std::pair<double, double> interval_of_edge(
        const Graph& graph,
        Edge_ID e,
        const std::set<Edge_ID>& edges_to_ignore);

    bool check_realization_lengths(const Mapped_Graph_Copy& g, const double epsilon);

    void init_sampling();
};

#endif
