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

    const Mapped_Graph_Copy* get_working_copy() {return working_copy;}
    const Mapped_Graph_Copy* get_display_graph() {return display_graph;}

    // samples based on the current configuration of working_copy
    void sample();

    // returns true until it can take no further steps
    bool step();

    std::list<Mapped_Graph_Copy*>& realizations();

    // in terms of the input graph
    const std::set<Edge_ID>& list_of_dropped() const;
    const std::vector<std::pair<Vertex_ID, Vertex_ID> >& list_of_added() const;
private:
    Graph *in_graph;
    Mapped_Graph_Copy *working_copy;

    Mapped_Graph_Copy *display_graph;
    bool save_display_graph; // i.e. it got put in _realizations

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


    // destructively determines if gc.copy is a partial 2-tree (series parallel, k4 minor avoiding...)
    static bool is_partial_2_tree(Mapped_Graph_Copy &gc);

    // returns a list of dropped edges from in_graph, in terms of working_copy vertex_id's
    std::vector<std::pair<Vertex_ID, Vertex_ID> > make_partial_2_tree();

    // assumes working_copy is now a partial 2 tree
    // returns a list of added edges in working_copy
    // std::vector<Edge_ID> make_2_tree();
    std::list<Edge_ID> make_2_tree();

    // determines a consistent order in which to realize a 2-tree
    std::vector<realization_triplet> realize_2_tree_order();
    void _realize_2_tree_order_aux(
        std::vector<realization_triplet> &already_ordered,
        Vertex_ID v0, Vertex_ID v1, Vertex_ID v_ignore);

    // assumes working_copy is a 2 tree
    // uses edge lengths and directly edits vertex properties (x and y)
    // returns true if it can be realized, false otherwise
    std::list<Mapped_Graph_Copy*> realize_2_tree();
    void _realize_2_tree_aux(
        std::list<Mapped_Graph_Copy*> &already_made,
        Vertex_ID orig_v0, Vertex_ID orig_v1, Vertex_ID orig_v_ignore);

    std::pair<double, double> interval_of_nonedge(
        std::set<Edge_ID> &nonedges,
        Edge_ID e);

    bool check_realization_lengths(const Mapped_Graph_Copy& g);

    void init_sampling();

    // std::list<Mapped_Graph_Copy*> sample(
    //     std::set<Edge_ID> &nonedges,
    //     std::list<Edge_ID> &nonedges_ordered,
    //     std::vector<std::pair<Vertex_ID, Vertex_ID> > &dropped_edges,
    //     Edge_ID e);
};

#endif
