#include "Cluster.hpp"

#include <iostream>


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Operators
////////////////////////////////////////////////////////////////////////////////

bool operator<(const Cluster_v &c0, const Cluster_v &c1) {
    return c0.size() < c1.size();
}

bool operator<(const Cluster_e &c0, const Cluster_e &c1) {
    return c0.size() < c1.size();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Output
////////////////////////////////////////////////////////////////////////////////

void print_cluster(const Graph &g, Cluster_v &c) {
    for (Cluster_v::iterator v_it = c.begin(); v_it != c.end(); v_it++) {
        std::cout << g[*v_it].name << " ";
    }
    std::cout << std::endl;
}

void print_cluster(const Graph &g, Cluster_e &c) {
    for (Cluster_e::iterator e_it = c.begin(); e_it != c.end(); e_it++) {
        std::pair<Vertex_ID, Vertex_ID> vs = g.vertices_incident(*e_it);

        std::cout << "(" << g[vs.first].name << ", " << g[vs.second].name << ") ";
    }
    std::cout << std::endl;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Other Functions
////////////////////////////////////////////////////////////////////////////////

bool is_trivial_intersection(const Cluster_e &c1, const Cluster_e &c2) {
    if (intersection_size(c1, c2) == 0)
        return true;
    return false;
}

unsigned int intersection_size(const Cluster_e &c1, const Cluster_e &c2) {
    Cluster_e intersection;
    std::set_intersection(
        c1.begin(), c1.end(),
        c2.begin(), c2.end(),
        std::inserter(intersection, intersection.begin()));

    return intersection.size();
}

bool is_subseteq(const Cluster_e &c1, const Cluster_e &c2) {
    if (c1.size() > c2.size())
        return false;

    for (Cluster_e::iterator e = c1.begin(); e != c1.end(); e++)
        if (c2.find(*e) == c2.end())
            return false;

    return true;
}

