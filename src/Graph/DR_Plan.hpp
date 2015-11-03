#ifndef DR_PLAN_HPP
#define DR_PLAN_HPP

#include "Graph.hpp"
#include "Tree.hpp"

#include <set>
#include <vector>
#include <map>


// TODO: Make cluster class, take out static functions of DR_Plan
// could be template class, with two classes that inherit

// class Cluster_e : public std::set<Edge_ID> {

// };

// inline bool operator=(const std::set<Edge_ID> &set, const Cluster_e &clust) {
//     return c0.size() < c1.size();
// }

typedef std::set<Vertex_ID> Cluster_v;
typedef std::set<Edge_ID>   Cluster_e;

typedef Tree<Cluster_v> DRP_Node_v;
typedef Tree<Cluster_e> DRP_Node_e;

// std::ostream& operator<<(std::ostream& os, const Cluster_v &obj);
// bool operator<(const Cluster_v &c0, const Cluster_v &c1);

inline bool operator<(const Cluster_v &c0, const Cluster_v &c1) {
    return c0.size() < c1.size();
}
inline bool operator<(const Cluster_e &c0, const Cluster_e &c1) {
    return c0.size() < c1.size();
}






// Takes an independent input graph and computes a pseudo-sequential
// DR-plan. While technically an underconstrained graph's DRP should be
// a forest, for convenience it's stored as a tree where the root is
// the input graph and the trees rooted at the second level are the true
// DR-plan.

//Runs in O(V^3).

class DR_Plan {
public:
    // TODO: return a forest if input is underconstrained. Currently just
    // returns a DRP with input as root and children as the "forest"

    // input graph must be independent
    DR_Plan(const Graph &graph); // uses all vertices in the graph

    // ~DR_Plan();

    DRP_Node_v* root();

    // returns whether or not the input graph was rigid
    bool rigid() const;

    static void print_cluster(const Graph &g, Cluster_v &c);
    static void print_cluster(const Graph &g, Cluster_e &c);
    void print_depth_first(DRP_Node_v *node, unsigned int tabs = 0) const;
    void print_depth_first(DRP_Node_e *node, unsigned int tabs = 0) const;
private:
    const Graph &graph;
    DRP_Node_v _root_v;
    DRP_Node_e _root_e;
    bool _rigid;

    // 'node' is expected to contain an isostatic cluster_v, C
    // 'M' maps each edge, e, in 'node' to the set of components in C\e
    // 'P' is a 2D array where element P[i][j] is the component in M[i] that contains edge j
    void decompose_isostatic_component(
        DRP_Node_e* node,
        const std::map<Edge_ID, std::set<Cluster_e*> > &M,
        const std::map<Edge_ID, std::map<Edge_ID, Cluster_e*> > &P);


    // The UI wants everything in terms of vertices, but edges are easier to work with
    DRP_Node_v* get_DRP_in_terms_of_vertices(const DRP_Node_e *root);
    // void get_DRP_in_terms_of_vertices(DRP_Node_e *root_e, DRP_Node_e *root_e);


    // Functions for clusters
    static bool is_trivial_intersection(const Cluster_e &c1, const Cluster_e &c2);
    static unsigned int intersection_size(const Cluster_e &c1, const Cluster_e &c2);
    static bool is_subseteq(const Cluster_e &c1, const Cluster_e &c2); // is c1 subset of c2





    ////////////////////////////////////////////
    ////////////////////////////////////////////

    DRP_Node_v* add_uncomputed_child(DRP_Node_v *parent, Cluster_v *child);
    void add_and_compute_child(DRP_Node_v *parent, Cluster_v *child);

    // // used when building a linear DR_Plan
    // // does not immediately compute children like public constructors
    // DR_Plan(Cluster_v vertices);

    // finds all wellconstrained, vertex-maximal proper subgraphs
    std::set<Cluster_v*> get_all_wcvmps(Mapped_Graph_Copy *mgc);


    // generates a DR_Plan from this node
    void _DRP_2D_linear_aux(DRP_Node_v *node);

};


#endif
