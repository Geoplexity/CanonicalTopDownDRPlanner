#include "Pebbled_Graph.hpp"

#include <vector>
#include <queue>
#include <algorithm>

#include <iostream>

// #define FOR_EACH_IN(TYPE, X, Y) \
//     for (TYPE::iterator Y = X.begin(); Y != X.end(); Y++)

// #define FOR_EACH_EDGE_IN_SUBGRAPH(X, Y) \
//     std::pair<Sg_Edge_Iterator, Sg_Edge_Iterator> _FOR_EACH_EDGE_IN_SUBGRAPH_TEMP_PAIR = X->edges(); \
//     for (Sg_Edge_Iterator Y = _FOR_EACH_EDGE_IN_SUBGRAPH_TEMP_PAIR.first; Y != _FOR_EACH_EDGE_IN_SUBGRAPH_TEMP_PAIR.second; Y++)
// #define FOR_EACH_EDGE_IN_SUBGRAPH(X, Y) \
//     for (Sg_Edge_Iterator Y = X->edges().first; Y != X->edges().second; Y++)




Pebbled_Graph::Pebbled_Graph(const Graph *g) {
    this->in_subgraph = new Subgraph(g);
    std::pair<Vertex_Iterator, Vertex_Iterator> vs = g->vertices();
    in_subgraph->induce(vs.first, vs.second);
}

Pebbled_Graph::Pebbled_Graph(Subgraph *sg) {
    this->in_subgraph = sg;
}

// if there are exactly 2 pebbles free in the end, the graph is rigid
unsigned int Pebbled_Graph::pebble_game_2D() {
    reset_pebbles();

    std::pair<Sg_Edge_Iterator, Sg_Edge_Iterator> es = this->in_subgraph->edges();
    for (Sg_Edge_Iterator it = es.first; it != es.second; it++)
    {
        this->enlarge_cover(*it);
    }

    return _total_pebbles_available;
}



std::set<Cluster*> Pebbled_Graph::component_pebble_game_2D(Vertex_ID *exclude) {
    // init pebbles
    reset_pebbles();

    // std::cout << "Pebbled_Graph::component_pebble_game_2D: here0" << std::endl;
    // for differencing
    Cluster all_vertices;
    std::pair<Sg_Vertex_Iterator, Sg_Vertex_Iterator> vs = this->in_subgraph->vertices();
    for (Sg_Vertex_Iterator v_it = vs.first; v_it != vs.second; v_it++) {
        all_vertices.insert(*v_it);
    }


    // the clusters found
    std::set<Cluster*> clusters_found;

    // true if they're in the same cluster, not in map if not
    std::map<std::pair<Vertex_ID, Vertex_ID>, bool> in_same_clust;

    std::pair<Sg_Edge_Iterator, Sg_Edge_Iterator> es = this->in_subgraph->edges();
    for (Sg_Edge_Iterator it = es.first; it != es.second; it++)
    {
        std::pair<Vertex_ID, Vertex_ID> vs_on_e = in_subgraph->vertices_incident(*it);
        Vertex_ID v1 = vs_on_e.first, v2 = vs_on_e.second;

        // std::cout << "Edge ("<< (*g)[v1].name << "--" << (*g)[v2].name << "): " << std::endl;;

        if (exclude != NULL && (v1 == *exclude || v2 == *exclude))
            continue;

        // if both verts are already in the cluter, no need to add the edge
        // wait, is this even possible?! in wellconstrained? I don't think so
        if (in_same_clust.find(vs_on_e) != in_same_clust.end())
            continue;

        // std::cout << "  Total pebbles remaining: " << _total_pebbles_available << std::endl;
        // std::cout << "    Pebbles before: "; print_all_verts_with_pebbles();

        // std::cout << "Pebbled_Graph::component_pebble_game_2D: here1" << std::endl;
        // if edge can't be added, move on
        if (!enlarge_cover(*it, exclude)) {
            // std::cout << "    Pebbles after : "; print_all_verts_with_pebbles();
            continue;
        }

        // std::cout << "  Successfully added." << std::endl;;

        // std::cout << "Pebbled_Graph::component_pebble_game_2D: here2" << std::endl;



        // work on v1
        while (num_pebbles_on_vert(v1) != this->k &&
            num_pebbles_on_vert(v1) + num_pebbles_on_vert(v2) < this->l)
        {
            // leave if you can't get any more pebbles
            if (!make_pebble_available(v1, &v2, exclude))
                break;
        }


        // work on v2
        while (num_pebbles_on_vert(v2) != this->k &&
            num_pebbles_on_vert(v1) + num_pebbles_on_vert(v2) < this->l)
        {
            // leave if you can't get any more pebbles
            if (!make_pebble_available(v2, &v1, exclude))
                break;
        }



        // std::cout << "Pebbled_Graph::component_pebble_game_2D: here3" << std::endl;



        //cluster maintenance


        // just for generality...
        if (num_pebbles_on_vert(v1) + num_pebbles_on_vert(v2) != this->l)
            continue;

        std::set<Vertex_ID>
            reach_v1 = determine_reach(v1),
            reach_v2 = determine_reach(v2),
            reach;
        std::set_union(
            reach_v1.begin(), reach_v1.end(),
            reach_v2.begin(), reach_v2.end(),
            std::inserter(reach, reach.begin()));

        // std::cout << "  Reach of " << (*g)[v1].name << ": "; print_verts(reach_v1, *g);
        // std::cout << "  Reach of " << (*g)[v2].name << ": "; print_verts(reach_v2, *g);
        // std::cout << "  Reach of union: "; print_verts(reach);



        // check if there are any pebbles in the reach, if so, continue
        bool pebbles_in_reach = false;
        for (std::set<Vertex_ID>::iterator v = reach.begin(); v != reach.end(); v++) {
            if (*v != v1 && *v != v2
                && num_pebbles_on_vert(*v) != 0)
            {
                pebbles_in_reach = true;
                break;
            }
        }
        if (pebbles_in_reach) continue;
        // std::cout << "Pebbled_Graph::component_pebble_game_2D: here4" << std::endl;


        // get -- V \ Reach(v1, v2)
        std::set<Vertex_ID> diff;
        std::set_difference(
            all_vertices.begin(), all_vertices.end(),
            reach.begin(), reach.end(),
            std::inserter(diff, diff.begin()));

        // std::cout << "  Diff          : "; print_verts(diff);

        // get inverse directed graph
        std::map<Vertex_ID, std::set<Vertex_ID> > rev = inverse_graph();

        // for each vertex in the difference, determine its reach
        // union these all up
        std::set<Vertex_ID> visited;
        for (std::set<Vertex_ID>::iterator v = diff.begin(); v != diff.end(); v++) {
            if (num_pebbles_on_vert(*v) != 0) {
                // Search rev, starting at v. Return any node left unvisited
                std::set<Vertex_ID> visited_here = determine_reach(*v, rev);

                std::set_union(
                    visited.begin(), visited.end(),
                    visited_here.begin(), visited_here.end(),
                    std::inserter(visited, visited.begin()));
            }
        }

        // std::cout << "Pebbled_Graph::component_pebble_game_2D: here5" << std::endl;

        // std::cout << "  Visited       : "; print_verts(visited);

        Cluster *c = new Cluster();

        // get -- V \ visited
        // std::set<Vertex_ID> unvisited;
        std::set_difference(
            all_vertices.begin(), all_vertices.end(),
            visited.begin(), visited.end(),
            std::inserter(*c, c->begin()));

        // std::cout << "  Unvisited     : "; print_verts(c->vertices);

        // unvisited is the new cluster!

        // TODO: this is quite inefficient, the paper says there is a better way
        std::vector<std::set<Cluster*>::iterator> to_erase;
        for (std::set<Cluster*>::iterator c_it = clusters_found.begin(); c_it != clusters_found.end(); c_it++) {
            std::set<Vertex_ID> old_diff_new;
            std::set_difference(
                (*c_it)->begin(), (*c_it)->end(),
                c->begin(), c->end(),
                std::inserter(old_diff_new, old_diff_new.begin()));
            if (old_diff_new.empty()) {
                to_erase.push_back(c_it);
            }
        }
        for (int i = to_erase.size()-1; i >= 0; i--) {
            delete *(to_erase[i]);
            clusters_found.erase(to_erase[i]);
        }

        clusters_found.insert(c);
        // for (v)


        // std::cout << "  Cluster: "; print_verts(c->vertices);
    }

    // std::cout << "\n" << std::endl;
    // for (std::set<Cluster*>::iterator c_it = clusters_found.begin(); c_it != clusters_found.end(); c_it++) {
    //     std::cout << "Cluster: ";
    //     for (std::set<Vertex_ID>::iterator v_it = (*c_it)->vertices.begin(); v_it != (*c_it)->vertices.end(); v_it++) {
    //         std::cout << (*g)[*v_it].name << " ";
    //     }
    //     std::cout << std::endl;
    // }
        // std::cout << "Pebbled_Graph::component_pebble_game_2D: here6" << std::endl;

    return clusters_found;

}



void Pebbled_Graph::print_all_verts_with_pebbles() {
    std::pair<Sg_Vertex_Iterator, Sg_Vertex_Iterator> vs = this->in_subgraph->vertices();
    for (Sg_Vertex_Iterator v_it = vs.first; v_it != vs.second; v_it++) {
        std::cout << (*in_subgraph)[*v_it].name << "("
            << (*in_subgraph)[pebbles_at_vertex[*v_it][0]].name
            << (*in_subgraph)[pebbles_at_vertex[*v_it][1]].name << ") ";
    }
    std::cout << std::endl;
}





unsigned int Pebbled_Graph::num_pebbles_on_vert(Vertex_ID v) {
    unsigned int count = 0;
    for (unsigned int i = 0; i < k; i++)
        if (this->pebbles_at_vertex[v][i] == v)
            count++;

    return count;
}

void Pebbled_Graph::reset_pebbles() {
    // put all of the pebbles back on their vertex
    pebbles_at_vertex.clear();
    std::pair<Sg_Vertex_Iterator, Sg_Vertex_Iterator> vs = this->in_subgraph->vertices();
    for (Sg_Vertex_Iterator it = vs.first; it != vs.second; it++)
    {
        for (unsigned int i = 0; i < k; i++) {
            pebbles_at_vertex[*it][i] = *it;
        }
    }

    // reset the pebble count
    _total_pebbles_available = this->k * in_subgraph->num_vertices();
}


// returns the index of a free pebble or k if there is none
unsigned int Pebbled_Graph::identify_free_pebble(Vertex_ID v) {
    for (unsigned int i = 0; i < k; i++)
        if (this->pebbles_at_vertex[v][i] == v)
            return i;

    return k;
}



// Helper function for find_pebble
bool Pebbled_Graph::_find_pebble_aux(
    Vertex_ID v,
    std::map<Vertex_ID, bool> *seen,
    std::map<Vertex_ID, vert_peb> *path,
    Vertex_ID *dont_take_pebble_from,
    Vertex_ID *exclude)
{
    // now you've seen this edge
    (*seen)[v] = true;

    // Is there already an available pebble? If so, return
    if (identify_free_pebble(v) < this->k
        && (dont_take_pebble_from == NULL
            || v != *dont_take_pebble_from))
    {
        return true;
    }

    // Go through the edges the pebbles are on
    for (unsigned int i = 0; i < this->k; i++) {
        Vertex_ID neighbor = this->pebbles_at_vertex[v][i];

        if (exclude != NULL && neighbor == *exclude)
            continue;

        // if you haven't seen the neighbor before
        if (seen->find(neighbor) == seen->end()) {
            (*path)[v] = vert_peb(neighbor, i);
            if (_find_pebble_aux(neighbor, seen, path, dont_take_pebble_from))
                return true;
        }
    }

    // All verts were seen previously or there were no pebbles
    return false;
}

// Finds a free pebble if there is one and sets up the path to it.
// Puts the path in path
bool Pebbled_Graph::find_pebble(
    Vertex_ID v,
    std::map<Vertex_ID, vert_peb> *path,
    Vertex_ID *dont_take_pebble_from,
    Vertex_ID *exclude)
{
    if (exclude != NULL && v == *exclude)
        return false;

    // a flag to prevent repeat work and infinite recursion
    std::map<Vertex_ID, bool> seen;

    unsigned int i;
    for (i = 0; i < this->k; i++) {
        Vertex_ID neighbor = pebbles_at_vertex[v][i];

        if (neighbor == v)
            continue;

        // initialize the seen map
        seen.clear();
        seen[v] = true;

        if (_find_pebble_aux(neighbor, &seen, path, dont_take_pebble_from, exclude)) {
            (*path)[v] = vert_peb(neighbor, i);
            break;
        }
    }

    // if true, no pebbles were found or they were already all free
    if (i == this->k) return false;

    return true;
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


// Frees a pebble at vertex v, if there is one. Returns false if all the pebbles
// are already free, or if it can't free anything up there.
bool Pebbled_Graph::make_pebble_available(
    Vertex_ID v,
    Vertex_ID *dont_take_pebble_from,
    Vertex_ID *exclude)
{
    if (exclude != NULL && v == *exclude)
        return false;

    // std::cout << "make_pebble_available: 0" <<std::endl;

    // used to reconstruct the path for rearranging. the key is the vertex_ID
    // the value is the pair of the next vertex_ID in the path and the pebble
    // used from the original to cover the edge.
    std::map<Vertex_ID, vert_peb> path;


    // try to find pebble. This will fill in path if found
    if (!find_pebble(v, &path, dont_take_pebble_from, exclude))
        return false;



    // free the pebble at v
    pebbles_at_vertex[v][path[v].pebble] = v;
    _total_pebbles_available++;

    // move the pebble from the next edge in the path onto the edge
    // that was just cleared to free a pebble. Repeat iteratively
    // until the end of the path is reached, at which point the free
    // pebble found by find_pebble is used.
    while (true) {
        Vertex_ID next_v = path[v].vertex;

        // std::cout << "make_pebble_available: 1: "
            // << (*g)[v].name << (*g)[next_v].name << std::endl;

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

    // std::cout << "make_pebble_available: "; print_all_verts_with_pebbles();

    return true;
}




// true means the edge was added to the cover
bool Pebbled_Graph::enlarge_cover(
    Edge_ID e,
    Vertex_ID *exclude)
{
    // initialize
    std::pair<Vertex_ID, Vertex_ID> vs_on_e = in_subgraph->vertices_incident(e);
    Vertex_ID v1 = vs_on_e.first, v2 = vs_on_e.second;

    if (exclude != NULL && (v1 == *exclude || v2 == *exclude))
        return false;

    unsigned int free_pebble;

    // check the first vert in the edge
    free_pebble = identify_free_pebble(v1);
    if (free_pebble < this->k) {
        place_available_pebble(v1, v2);
        return true;
    } else {
        if (make_pebble_available(v1)) {
            place_available_pebble(v1, v2);
            return true;
        }
    }

    // check the second vert in the edge
    // note that the search for the first may have included the second
    // so this could be wasted effort
    free_pebble = identify_free_pebble(v1);
    if (free_pebble < this->k) {
        place_available_pebble(v2, v1);
        return true;
    } else {
        if (make_pebble_available(v2)) {
            place_available_pebble(v2, v1);
            return true;
        }
    }

    return false;
}








std::map<Vertex_ID, std::set<Vertex_ID> > Pebbled_Graph::inverse_graph() {
    std::map<Vertex_ID, std::set<Vertex_ID> > ret;

    // put all of the pebbles on their vertex
    std::pair<Sg_Vertex_Iterator, Sg_Vertex_Iterator> vs = this->in_subgraph->vertices();
    for (Sg_Vertex_Iterator vert = vs.first; vert != vs.second; vert++)
    {
        for (unsigned int i = 0; i < k; i++) {
            Vertex_ID other_vert = pebbles_at_vertex[*vert][i];
            if (other_vert != *vert)
                ret[other_vert].insert(*vert);
        }
    }

    return ret;
}


// all the vertices that can be reached by following pebbles
// Uses breadth first search
std::set<Vertex_ID> Pebbled_Graph::determine_reach(Vertex_ID v) {
    std::set<Vertex_ID> ret;

    std::queue<Vertex_ID> q;
    q.push(v);

    std::map<Vertex_ID, bool> seen;
    seen[v] = true;

    while (!q.empty()) {
        Vertex_ID vert = q.front();
        q.pop();

        ret.insert(vert);

        for (int i = 0; i < this->k; i++) {
            Vertex_ID other_vert = pebbles_at_vertex[v][i];

            //if it hasn't been seen... If the pebble is on the vertex it will have been seen
            if (seen.find(other_vert) == seen.end()) {
                q.push(other_vert);
                seen[other_vert] = true;
            }
        }
    }

    return ret;
}



std::set<Vertex_ID> Pebbled_Graph::determine_reach(
    Vertex_ID v,
    std::map<Vertex_ID, std::set<Vertex_ID> > &digraph)
{
    std::set<Vertex_ID> ret;

    std::queue<Vertex_ID> q;
    q.push(v);

    std::map<Vertex_ID, bool> seen;
    seen[v] = true;

    while (!q.empty()) {
        Vertex_ID vert = q.front();
        q.pop();

        ret.insert(vert);

        for (
            std::set<Vertex_ID>::iterator v_it = digraph[vert].begin();
            v_it != digraph[vert].end();
            v_it++)
        {
            Vertex_ID other_vert = (*v_it);

            //if it hasn't been seen... If the pebble is on the vertex it will have been seen
            if (seen.find(other_vert) == seen.end()) {
                q.push(other_vert);
                seen[other_vert] = true;
            }
        }
    }

    return ret;
}
