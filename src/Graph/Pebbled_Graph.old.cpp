#include "Pebbled_Graph.hpp"

#include <vector>



Pebbled_Graph::Pebbled_Graph(Graph *g) {
    this->g = g;
}

// if there are exactly 2 pebbles free in the end, the graph is rigid
unsigned int Pebbled_Graph::pebble_game_2D() {
    reset_pebbles();

    std::pair<Edge_Iterator, Edge_Iterator> es = this->g->edges();
    for (Edge_Iterator it = es.first; it != es.second; it++)
    {
        this->enlarge_cover(it);
    }

    return _total_pebbles_available;
}

Clusters Pebbled_Graph::DRP_2D() {
    std::map<Vertex_ID, Clusters> clusters_for_excluded_vert;

    std::pair<Vertex_Iterator, Vertex_Iterator> vs = this->g->vertices();
    for (Vertex_Iterator it = vs.first; it != vs.second; it++)
    {
        clusters_for_excluded_vert[*it] = this->pebble_game_2D_exclude(*it);
    }

    // this->pebble_game_2D();

    return clusters_for_excluded_vert[0];
}


Clusters Pebbled_Graph::pebble_game_2D_exclude(Vertex_ID excluded_vert) {
    // init pebbles
    reset_pebbles();
    _total_pebbles_available -= this->k;

    // init cluster map
    std::map<Vertex_ID, unsigned int> cluster_id_for_vert;
    unsigned int current_cluster_id = 0;
    Clusters cs;


    //
    std::pair<Edge_Iterator, Edge_Iterator> es = this->g->edges();
    for (Edge_Iterator it = es.first; it != es.second; it++)
    {
        std::pair<Vertex_ID, Vertex_ID> vs_on_e = g->verts_on_edge(it);
        Vertex_ID v1 = vs_on_e.first, v2 = vs_on_e.second;

        // check if this edge lies on the excluded vertex
        if (v1 == excluded_vert || v2 == excluded_vert)
            continue;

        // get the cluster_ids
        std::map<Vertex_ID, unsigned int>::iterator v1_cid
            = cluster_id_for_vert.find(v1);
        std::map<Vertex_ID, unsigned int>::iterator v2_cid
            = cluster_id_for_vert.find(v2);

        std::map<Vertex_ID, unsigned int>::iterator cid_end
            = cluster_id_for_vert.end();

        // check if they're in the same cluster
        if (v1_cid != cid_end && v2_cid != cid_end && v1_cid == v2_cid)
            continue;

        // if everything is fine, enlarge cover
        this->enlarge_cover(it);

    }

    return cs;
}


void Pebbled_Graph::reset_pebbles() {
    // put all of the pebbles back on their vertex
    pebbles_at_vertex.clear();
    std::pair<Vertex_Iterator, Vertex_Iterator> vs = this->g->vertices();
    for (Vertex_Iterator it = vs.first; it != vs.second; it++)
    {
        for (unsigned int i = 0; i < k; i++) {
            pebbles_at_vertex[*it][i] = *it;
        }
    }

    // reset the pebble count
    _total_pebbles_available = this->k * g->num_vertices();
}


// returns the index of a free pebble or k if there is none
unsigned int Pebbled_Graph::identify_free_pebble(Vertex_ID v) {
    for (unsigned int i = 0; i < k; i++)
        if (this->pebbles_at_vertex[v][i] == v)
            return i;

    return k;
}



// Finds a free pebble if there is one and sets up the path to it.
bool Pebbled_Graph::find_pebble(Vertex_ID v){
    // Initialize
    this->path.erase(v);
    this->seen[v] = true;

    // Is there already is an available pebble? If so, return
    if (identify_free_pebble(v) < this->k) return true;

    // Go through the edges the pebbles are on
    for (unsigned int i = 0; i < this->k; i++) {
        Vertex_ID other_v = this->pebbles_at_vertex[v][i];
        if (!seen[other_v]) {
            this->path[v] = vert_peb(other_v, i);
            if (find_pebble(other_v))
                return true;
        }
    }

    // All verts were seen previously or there were no pebbles
    return false;
}


// takes one of origin's available pebbles and places it on the edge to
// the destination vertex_ID. Returns false if there was no available pebble
bool Pebbled_Graph::place_available_pebble(Vertex_ID origin, Vertex_ID destination) {
    // which pebble can be taken?
    unsigned int free_pebble = identify_free_pebble(origin);

    // No free pebble found
    if (free_pebble == this->k)
        return false;

    // place the pebble
    pebbles_at_vertex[origin][free_pebble] = destination;
    _total_pebbles_available--;

    return true;
}


// Frees a pebble at vertex_ID v, if there is one. Returns true if successful
bool Pebbled_Graph::make_pebble_available(Vertex_ID v) {
    // finds a pebble if there is one and initializes the path. If not,
    // terminate
    path.clear();
    if (!find_pebble(v)) return false;

    // was the free pebble already on v? If so terminate
    if (path.find(v) == path.end()) return true;



    // free the pebble at v
    pebbles_at_vertex[v][path[v].pebble] = v;
    _total_pebbles_available++;

    // move the pebble from the next edge in the path onto the edge
    // that was just cleared to free a pebble. Repeat iteratively
    // until the end of the path is reached, at which point the free
    // pebble found by find_pebble is used.
    while (true) {
        Vertex_ID next_v = path[v].vertex;

        // you've reached the end of the path, next_v is guaranteed to
        // have an extra pebble. So place on edge and exit loop
        if (path.find(next_v) == path.end()) {
            place_available_pebble(next_v, v);
            break;
        }


        // there is more to the path, therefore there are no free
        // pebbles on this vert. So take it from the next vert in
        // the path
        pebbles_at_vertex[next_v][path[next_v].pebble] = v;


        // for next iteration
        v = next_v;
    }

    return true;
}




// true means the edge was added to the cover
bool Pebbled_Graph::enlarge_cover(Edge_Iterator e) {
    // initialize
    std::pair<Vertex_Iterator, Vertex_Iterator> vs = g->vertices();
    for (Vertex_Iterator it = vs.first; it != vs.second; it++)
    {
        seen[*it] = false;
    }

    std::pair<Vertex_ID, Vertex_ID> vs_on_e = g->verts_on_edge(e);
    Vertex_ID v1 = vs_on_e.first, v2 = vs_on_e.second;

    // check the first vert in the edge
    if (make_pebble_available(v1)) {
        place_available_pebble(v1, v2);
        return true;
    }

    // check the second vert in the edge
    // note that the search for the first may have included the second
    if (!seen[v2]) {
        if (make_pebble_available(v2)) {
            place_available_pebble(v2, v1);
            return true;
        }
    }

    return false;
}
