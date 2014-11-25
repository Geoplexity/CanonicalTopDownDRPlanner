#include "Pebbled_Graph.hpp"

#include <vector>



Pebbled_Graph::Pebbled_Graph(Graph *g) {
    this->g = g;

    cluster_id = 0;

    _total_pebbles_available = 2*g->num_vertices();

    // **pebbles = new (Vertex**)[g->num_vertices()];
    // for (uint i = 0; i < g->num_vertices(); i++)
    //     *pebbles[i] = new Vertex[2];
}

// if there are exactly 2 pebbles free in the end, the graph is rigid
unsigned int Pebbled_Graph::DRP_2D() {
    std::pair<Edge_iter, Edge_iter> es = g->edges();
    for (Edge_iter it = es.first; it != es.second; it++)
    {
        this->enlarge_cover(it);
    }

    return _total_pebbles_available;
}




// Finds a free pebble if there is one and sets up the path to it.
bool Pebbled_Graph::find_pebble(Vertex v){
    path.erase(v);
    seen[v] = true;

    // get the edges
    std::map<vert_peb, Vertex>::iterator p1v
        = pebbles_on_verts.find(vert_peb(v, p1));

    std::map<vert_peb, Vertex>::iterator p2v
        = pebbles_on_verts.find(vert_peb(v, p2));

    // there is an available pebble
    if (p1v == pebbles_on_verts.end() or p2v == pebbles_on_verts.end())
        return true;

    // check the edge p1 is on
    Vertex p1vid = p1v->second;
    if (!seen[p1vid]) {
        path[v] = vert_peb(p1vid, p1);
        if (find_pebble(p1vid)) return true;
    }

    // check the edge p2 is on
    Vertex p2vid = p2v->second;
    if (!seen[p2vid]) {
        path[v] = vert_peb(p2vid, p2);
        if (find_pebble(p2vid)) return true;
    }

    // both of them were seen previously or there were no pebbles
    return false;
}



// Frees a pebble at vertex v, if there is one. Returns true if successful
bool Pebbled_Graph::make_pebble_available(Vertex v) {
    // finds a pebble if there is one and initializes the path. If not,
    // terminate
    if (!find_pebble(v)) return false;

    // was the free pebble already on v? If so terminate
    if (path.find(v) == path.end()) return true;



    // free the pebble at v
    pebbles_on_verts.erase(vert_peb(v, path[v].second));
    _total_pebbles_available++;

    // move the pebble from the next edge in the path onto the edge
    // that was just cleared to free a pebble. Repeat iteratively
    // until the end of the path is reached, at which point the free
    // pebble found by find_pebble is used.
    while (true) {
        Vertex next_v  = path[v].first;

        // you've reached the end of the path, next_v is guaranteed to
        // have an extra pebble. So place on edge and exit loop
        if (path.find(next_v) == path.end()) {
            place_available_pebble(next_v, v);
            break;
        }


        // there is more to the path, therefore there are no free
        // pebbles on this vert. So take it from the next vert in
        // the path
        pebbles_on_verts[vert_peb(next_v, path[next_v].second)] = v;


        // for next iteration
        v = next_v;
    }

    return true;
}


// takes one of origin's available pebbles and places it on the edge to
// the destination vertex. Returns false if there was no available pebble
bool Pebbled_Graph::place_available_pebble(Vertex origin, Vertex destination) {
    pebble pb;

    if (pebbles_on_verts.find(vert_peb(origin, p1))
        == pebbles_on_verts.end())
    {
        pb = p1;
    }
    else if (pebbles_on_verts.find(vert_peb(origin, p2))
        == pebbles_on_verts.end())
    {
        pb = p2;
    }
    else return false;

    pebbles_on_verts[vert_peb(origin, pb)] = destination;

    _total_pebbles_available--;
    return true;
}

// true means the edge was added to the cover
bool Pebbled_Graph::enlarge_cover(Edge_iter e) {
    // initialize
    std::pair<Vertex_iter, Vertex_iter> vs = g->vertices();
    for (Vertex_iter it = vs.first; it != vs.second; it++)
    {
        seen[*it] = false;
        path.clear();
    }

    std::pair<Vertex, Vertex> vs_on_e = g->verts_on_edge(e);

    // check the first vert in the edge
    if (make_pebble_available(vs_on_e.first)) {
        place_available_pebble(vs_on_e.first, vs_on_e.second);
        return true;
    }

    // check the second edge
    // note that the search for the first may have included the second
    if (!seen[vs_on_e.second]) {
        if (make_pebble_available(vs_on_e.second)) {
            place_available_pebble(vs_on_e.second, vs_on_e.first);
            return true;
        }
    }

    return false;
}
