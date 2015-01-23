#ifndef PEBBLED_GRAPH_HPP
#define PEBBLED_GRAPH_HPP

#include "Graph.hpp"

#include <map>
#include <vector>
#include <array>
#include <set>


struct Cluster {
    std::set<Vertex_ID> vertices;
    std::set<Cluster*> children;
    // std::set<Edge_ID> edges;

    bool finished;


    Cluster(bool f=false)
        {finished = f;}
    Cluster(std::set<Vertex_ID> vs, bool f=false)
        {vertices = vs; finished = f;}
    Cluster(std::set<Vertex_ID> vs, std::set<Cluster*> cs, bool f=false)
        {vertices = vs; children = cs; finished = f;}


    void add_vertex(Vertex_ID v) {vertices.insert(v);}

    // void finished(bool f) {_finished = f}
    // bool finished() {return finished;}
    // void add_edge(Edge_ID e) {edges.insert(e);}

    void print_tree(Graph &g, unsigned int tabs = 0);
};


class Pebbled_Graph {
public:
    Pebbled_Graph(Graph *g);
    Pebbled_Graph(Subgraph *g);

    // if there are exactly 2 pebbles free in the end, the graph is rigid
    unsigned int pebble_game_2D();
    // if there is exactly 1 cluster, the graph is rigid
    std::set<Cluster*> component_pebble_game_2D(Vertex_ID *exclude = NULL);

    // If optimal is false, you find completeDRP
    Cluster DRP_2D(bool optimalDRP = true);


private:
    // pebbles per node = k = 2
    // pebbles for sparsity/tightness = l = 3
    // solving for (k, l)-sparse graphs
    // ONLY GUARANTEED TO WORK ON (2,3)
    static const unsigned int k = 2;
    static const unsigned int l = 3;





    // the graph we're playing the game on
    Subgraph *in_subgraph;



    // An array to store the Vertex_ID that a pebble is associated with
    typedef std::array<Vertex_ID, k> pebbles;

    // Used to keep track of each vertex's pebbles. Say pebbles_at_vertex[v][i] = n
    // if v == n, then the pebble is available (on the vertex v). Else if v != n
    // then it's on the edge between v and n
    std::map<Vertex_ID, pebbles> pebbles_at_vertex;

    // starts at 2 * the number of vertices
    // set in reset_pebbles, decremented in place_available_pebble
    unsigned int _total_pebbles_available;



    // A vertex and an important pebble (index)
    struct vert_peb {
        Vertex_ID vertex;
        unsigned int pebble;
        vert_peb() {}
        vert_peb(Vertex_ID v, unsigned int p) {vertex = v; pebble = p;}
    };


    std::set<Cluster*> _DRP_2D_aux(
        Cluster* known_cluster,
        bool optimalDRP);

    std::set<Cluster*> _DRP_2D_linear_aux();


    // get all wellconstrained vertex-maximal subgraphs
    std::set<Cluster*> get_all_wcvmps(Cluster* known_cluster = NULL);



    // returns the index of a free pebble or k if there is none
    unsigned int identify_free_pebble(Vertex_ID v);

    // returns the number of pebbles free on given vertex
    unsigned int num_pebbles_on_vert(Vertex_ID v);


    void print_verts(std::set<Vertex_ID> &v);
    void print_all_verts_with_pebbles();



    // Finds a free pebble if there is one and sets up the path to it.
    bool _find_pebble_aux(
        Vertex_ID v,
        std::map<Vertex_ID, bool> *seen,
        std::map<Vertex_ID, vert_peb> *path,
        Vertex_ID *dont_take_pebble_from = NULL,
        Vertex_ID *exclude = NULL);
    bool find_pebble(
        Vertex_ID v,
        std::map<Vertex_ID, vert_peb> *path,
        Vertex_ID *dont_take_pebble_from = NULL,
        Vertex_ID *exclude = NULL);

    // Frees a pebble at vertex_ID v, if there is one. Returns true if successful
    bool make_pebble_available(
        Vertex_ID v,
        Vertex_ID *dont_take_pebble_from = NULL,
        Vertex_ID *exclude = NULL);

    // true means the edge was added to the cover
    bool enlarge_cover(
        Edge_ID e,
        Vertex_ID *exclude = NULL);
    // bool component_enlarge_cover(Edge_Iterator e);

    // takes one of origin's available pebbles and places it on the edge to
    // the destination vertex_ID. Returns false if there was no available pebble
    bool place_available_pebble(Vertex_ID origin, Vertex_ID destination);




    // gives the inverted directed pebble game graph
    std::map<Vertex_ID, std::set<Vertex_ID> > inverse_graph();

    // all the vertices that can be reached by following pebbles (DFS/BFS on directed graph)
    std::set<Vertex_ID> determine_reach(Vertex_ID v);
    std::set<Vertex_ID> determine_reach(Vertex_ID v, std::map<Vertex_ID, std::set<Vertex_ID> > &digraph);





    void reset_pebbles();

    std::set<Cluster*> pebble_game_2D_exclude(Vertex_ID excluded_vert);
};



#endif
