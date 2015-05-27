#ifndef DR_PLAN_HPP
#define DR_PLAN_HPP

#include "Graph.hpp"
#include "Tree.hpp"

#include <set>
#include <vector>



typedef std::set<Vertex_ID> Cluster;
typedef Tree<Cluster> DRP_Node;




// class Cluster : public std::set<Vertex_ID> {
//     Cluster &difference(Cluster &other);
// };


// A simple tree, where each node stores a cluster and a flag as to whether or not it is finished
class DR_Plan {
public:
    // TODO: return a forest if input is underconstrained. Currently just returns
    // a DRP with input as root and children as the "forest"

    // input graph must be independent
    DR_Plan(const Graph &graph); // uses all vertices in the graph
    // DR_Plan(const Graph &graph, Cluster vertices); // uses induced subgraph on vertices

    DRP_Node* root();


    void print_depth_first(DRP_Node *node, unsigned int tabs = 0);
    static void print_cluster(const Graph &g, Cluster &c);
private:
    const Graph &graph;
    DRP_Node _root;

    DRP_Node* add_uncomputed_child(DRP_Node *parent, Cluster *child);
    void add_and_compute_child(DRP_Node *parent, Cluster *child);

    // // used when building a linear DR_Plan
    // // does not immediately compute children like public constructors
    // DR_Plan(Cluster vertices);

    // finds all wellconstrained, vertex-maximal proper subgraphs
    std::set<Cluster*> get_all_wcvmps(Mapped_Graph_Copy *mgc);


    // generates a DR_Plan from this node
    void _DRP_2D_linear_aux(DRP_Node *node);

};


#endif
