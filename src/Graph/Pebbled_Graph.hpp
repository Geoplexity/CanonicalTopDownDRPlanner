#ifndef PEBBLED_GRAPH_HPP
#define PEBBLED_GRAPH_HPP

#include "Graph.hpp"

#include <map>


class Pebbled_Graph {
public:
    Pebbled_Graph(Graph *g);

    // if there are exactly 2 pebbles free in the end, the graph is rigid
    unsigned int DRP_2D();


private:

    unsigned int cluster_id;


    enum pebble {
        p1, p2
    };

    // the edge a pebble from a vertex is on, represented as the vertex it's
    // pointing to. If it maps to nothing, then the pebble is available
    typedef std::pair<Vertex, pebble> vert_peb;
    std::map<vert_peb, Vertex> pebbles_on_verts;

    // set in the constructor, decremented in place_available_pebble
    unsigned int _total_pebbles_available;



    // // the pair represents and edge and it maps to the vertex it got its
    // // pebble from
    // typedef std::pair<Vertex, Vertex> V_ID_pair;
    // std::map<V_ID_pair, Vertex> pebbled;

    // std::map<Vertex, uint> free_pebbles;

    // Vertex **pebbles;




    // used to reconstruct the path for rearranging. the key is the vertex
    // the value is the pair of the next vertex in the path and the pebble
    // used from the original to cover the edge.
    std::map<Vertex, vert_peb> path;

    // a flag to prevent repeat work and infinite recursion
    std::map<Vertex, bool> seen;

    // the graph we're playing the game on
    Graph *g;


    // Vertex *is_pebbled(Vertex vid1, Vertex vid2) {
    //     V_ID_pair p = V_ID_pair(vid1, vid2);
    //     if (pebbled.find(p) == pebbled.end())
    //         return NULL;
    //     else return &pebbled[p];
    // }

    // Vertex *is_pebbled(Vertex vid1, Vertex vid2) {
    // }


    // Finds a free pebble if there is one and sets up the path to it.
    bool find_pebble(Vertex v);


    // Frees a pebble at vertex v, if there is one. Returns true if successful
    bool make_pebble_available(Vertex v);


    // takes one of origin's available pebbles and places it on the edge to
    // the destination vertex. Returns false if there was no available pebble
    bool place_available_pebble(Vertex origin, Vertex destination);

    // true means the edge was added to the cover
    bool enlarge_cover(Edge_iter e);
};



#endif
