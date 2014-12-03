#ifndef PEBBLED_GRAPH_HPP
#define PEBBLED_GRAPH_HPP

#include "Graph.hpp"

#include <map>
#include <vector>
#include <array>


typedef std::vector<std::vector<Vertex_ID> > Clusters;


class Pebbled_Graph {
public:
    Pebbled_Graph(Graph *g);

    // if there are exactly 2 pebbles free in the end, the graph is rigid
    unsigned int pebble_game_2D();

    Clusters DRP_2D();


private:
    // pebbles per node = k = 2
    // pebbles for sparsity/tightness = l = 3
    // solving for (k, l)-sparse graphs
    static const unsigned int k = 2;
    static const unsigned int l = 3;





    // the graph we're playing the game on
    Graph *g;



    // An array to store the Vertex_ID that a pebble is associated with
    typedef std::array<Vertex_ID, k> pebbles;

    // Used to keep track of each vertex's pebbles. Say pebbles_at_vertex[v][i] = n
    // if v == n, then the pebble is available (on the vertex v). Else if v != n
    // then it's on the edge between v and n
    std::map<Vertex_ID, pebbles> pebbles_at_vertex;

    // starts at 2 * the number of vertices
    unsigned int _total_pebbles_available;




    // a flag to prevent repeat work and infinite recursion
    std::map<Vertex_ID, bool> seen;

    // A vertex and an important pebble (index)
    struct vert_peb {
        Vertex_ID vertex;
        unsigned int pebble;
        vert_peb() {}
        vert_peb(Vertex_ID v, unsigned int p) {vertex = v; pebble = p;}
    };
    // typedef std::pair<Vertex_ID, unsigned int> vert_peb;
    // used to reconstruct the path for rearranging.
    std::map<Vertex_ID, vert_peb> path;


    // enum pebble {
    //     p1, p2
    // };

    // // the edge a pebble from a vertex_ID is on, represented as the vertex_ID it's
    // // pointing to. If it maps to nothing, then the pebble is available
    // typedef std::pair<Vertex_ID, pebble> vert_peb;
    // std::map<vert_peb, Vertex_ID> pebbles_on_verts;

    // // set in the constructor, decremented in place_available_pebble
    // unsigned int _total_pebbles_available;


    // // used to reconstruct the path for rearranging. the key is the vertex_ID
    // // the value is the pair of the next vertex_ID in the path and the pebble
    // // used from the original to cover the edge.
    // std::map<Vertex_ID, vert_peb> path;




    // returns the index of a free pebble or k if there is none
    unsigned int identify_free_pebble(Vertex_ID v);

    // Finds a free pebble if there is one and sets up the path to it.
    bool find_pebble(Vertex_ID v);

    // Frees a pebble at vertex_ID v, if there is one. Returns true if successful
    bool make_pebble_available(Vertex_ID v);

    // takes one of origin's available pebbles and places it on the edge to
    // the destination vertex_ID. Returns false if there was no available pebble
    bool place_available_pebble(Vertex_ID origin, Vertex_ID destination);

    // true means the edge was added to the cover
    bool enlarge_cover(Edge_Iterator e);




    void reset_pebbles();

    Clusters pebble_game_2D_exclude(Vertex_ID excluded_vert);
};



#endif
