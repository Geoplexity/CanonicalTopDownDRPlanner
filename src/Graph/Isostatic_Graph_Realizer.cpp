#include "Isostatic_Graph_Realizer.hpp"

#include "Min_Priority_Queue.hpp"

#include <cmath>
#include <iostream>




Isostatic_Graph_Realizer::Isostatic_Graph_Realizer(const Graph *g) : in_graph(g) {
    // this->in_graph = g;
    this->working_copy = new Mapped_Graph_Copy(g);
}

Isostatic_Graph_Realizer::~Isostatic_Graph_Realizer() {
    delete(working_copy);
}

std::list<Mapped_Graph_Copy*> Isostatic_Graph_Realizer::realize() {
    std::vector<std::pair<Vertex_ID, Vertex_ID> > dropped = make_partial_2_tree();
    std::list<Edge_ID> nonedges_ordered = make_2_tree();



    // fill up the set of dropped edges in terms of the input graph
    in_graph_dropped.clear();
    for (std::vector<std::pair<Vertex_ID, Vertex_ID> >::iterator v_p_it = dropped.begin(); v_p_it != dropped.end(); v_p_it++) {
        Vertex_ID orig0 = working_copy->original_vertex(v_p_it->first);
        Vertex_ID orig1 = working_copy->original_vertex(v_p_it->second);
        in_graph_dropped.insert(in_graph->edge(orig0, orig1));
    }

    // fill up the vector of added edges in terms of the input graph
    in_graph_added.clear();
    for (std::list<Edge_ID>::iterator e_it = nonedges_ordered.begin(); e_it != nonedges_ordered.end(); e_it++) {
        std::pair<Vertex_ID, Vertex_ID> vs = working_copy->vertices_incident(*e_it);
        in_graph_added.push_back(std::make_pair(working_copy->original_vertex(vs.first), working_copy->original_vertex(vs.second)));
    }




    std::set<Edge_ID> nonedges;
    for (std::list<Edge_ID>::iterator es = nonedges_ordered.begin(); es != nonedges_ordered.end(); es++) {
        nonedges.insert(*es);
    }

    Edge_ID e = nonedges_ordered.front();
    nonedges_ordered.pop_front();
    nonedges.erase(e);

    std::list<Mapped_Graph_Copy*> realized = sample(nonedges, nonedges_ordered, dropped, e);

    bool success = realized.size() != 0;
    std::cout << "realize: " << (success? "success": "failure") << " " << realized.size() << std::endl;

    if (success) {
        Mapped_Graph_Copy &g = *(realized.front());

        g.print_vertices();
        g.print_edges();
        for (Vertex_Iterator v_it = g.vertices().first; v_it != g.vertices().second; v_it++) {
            std::cout << g[*v_it].x << "\t" << g[*v_it].y << std::endl;
        }
    }




    return realize_2_tree();




}

// in terms of the input graph
const std::set<Edge_ID>& Isostatic_Graph_Realizer::list_of_dropped() const {
    return in_graph_dropped;
}

// in terms of the input graph
const std::vector<std::pair<Vertex_ID, Vertex_ID> >& Isostatic_Graph_Realizer::list_of_added() const {
    return in_graph_added;
}

bool Isostatic_Graph_Realizer::is_partial_2_tree(Mapped_Graph_Copy &mgc) {
    Min_Priority_Queue<Vertex_ID, unsigned int> mpq;
    Vertex_Iterator v, v_end;
    for (boost::tie(v, v_end) = mgc.vertices(); v != v_end; v++) {
        mpq.insert(*v, mgc.degree_of_vertex(*v));
    }

    // while (!mpq.empty()) {
    //     std::pair<Vertex_ID, unsigned int> top = mpq.pop_top();
    //     std::cout << "(" << (*mgc)[top.first].name << ", " << top.second << ") ";
    // }
    // std::cout << std::endl;

    while (!mpq.empty()) {
        std::pair<Vertex_ID, unsigned int> top = mpq.pop_top();
        // std::cout << "(" << (mgc)[top.first].name << ", " << top.second << ")" << std::endl;

        if (top.second == 0) {
            // can simply remove mgc
            mgc.remove_vertex(top.first);
        } else if (top.second == 1) {
            std::pair<Graph_Adj_Iterator, Graph_Adj_Iterator> v_adj
                = boost::adjacent_vertices(top.first, mgc);
            Vertex_ID v = *(v_adj.first);

            // Contract it into the graph.
            // this->contractions.push_back(std::make_pair(top.first, v));
            mgc.contract_edge(top.first, v);

            // No other degrees change besides v and the one popped
            mpq.change_key(v, mgc.degree_of_vertex(v));
        } else if (top.second == 2) {
            std::pair<Graph_Adj_Iterator, Graph_Adj_Iterator> v_adj
                = boost::adjacent_vertices(top.first, mgc);

            // make a vector of the adj verts, iterators go bad after contraction
            std::vector<Vertex_ID> v_adj_vec;
            for (Graph_Adj_Iterator v_adj_it = v_adj.first; v_adj_it != v_adj.second; ++v_adj_it) {
                v_adj_vec.push_back(*v_adj_it);
            }

            // contract
            // this->contractions.push_back(std::make_pair(top.first, v_adj_vec[0]));
            mgc.contract_edge(top.first, v_adj_vec[0]);

            // update degrees
            for (int i = 0; i < v_adj_vec.size(); i++) {
                mpq.change_key(v_adj_vec[i], mgc.degree_of_vertex(v_adj_vec[i]));
            }
        } else {
            break;
        }
    }

    if (mgc.num_vertices() == 0)
        return true;
    else
        return false;
}




// returns a list of dropped edges
std::vector<std::pair<Vertex_ID, Vertex_ID> > Isostatic_Graph_Realizer::make_partial_2_tree() {
    assert(working_copy->num_vertices() >= 3);

    std::vector<std::pair<Vertex_ID, Vertex_ID> > ret;

    // Copy the graph, check if it's a partial 2 tree (does all the contractions, altering the copy)
    // Checks the leftover edges, if they're in the original graph, they are removed from the original graph
    Mapped_Graph_Copy *mgc = new Mapped_Graph_Copy(working_copy);
    while (!is_partial_2_tree(*mgc)) {
        Edge_Iterator e, e_end;
        for (boost::tie(e, e_end) = mgc->edges(); e != e_end; e++) {
            std::pair<Vertex_ID, Vertex_ID> vs = mgc->vertices_incident(*e);
            Vertex_ID v0 = mgc->original_vertex(vs.first);
            Vertex_ID v1 = mgc->original_vertex(vs.second);
            // Vertex_ID v0_orig = working_copy->original_vertex(v0);
            // Vertex_ID v1_orig = working_copy->original_vertex(v1);

            if (working_copy->has_edge(v0, v1)) {
                // ret.push_back(in_graph->edge(v0_orig, v1_orig));
                ret.push_back(std::make_pair(v0, v1));
                working_copy->remove_edge(v0, v1);
                break;
            }
        }

        // delete and make the new one. Needs to be pointers or else copy
        // constructor is invoked and the Mapped_Graph_Copy maps stop working.
        delete(mgc);
        mgc = new Mapped_Graph_Copy(working_copy);
    }
    delete(mgc);

    return ret;
}





// returns the first element in the set that is not v
Vertex_ID get_not_self_in_set(Vertex_ID v, std::set<Vertex_ID> &set) {
    for (std::set<Vertex_ID>::iterator v_it = set.begin(); v_it != set.end(); v_it++) {
        if (*v_it != v) {
            return *v_it;
        }
    }

    assert(false); // you gave me a set that contained nothing or just v
    return (Vertex_ID) NULL;
}




// returns a list of added edges
// establishes a partial order on the added edges, which is used in sampling.
// changing the order of things in this algorithm can have unintended consequences.
std::list<Edge_ID> Isostatic_Graph_Realizer::make_2_tree() {
    const double default_edge_length = 1.0;

    std::list<Edge_ID> ret;
    std::vector<std::pair<Vertex_ID, Vertex_ID> > add_to_copy;

    Mapped_Graph_Copy mgc(working_copy);

    Min_Priority_Queue<Vertex_ID, unsigned int> mpq;
    Vertex_Iterator v, v_end;
    for (boost::tie(v, v_end) = mgc.vertices(); v != v_end; v++) {
        mpq.insert(*v, mgc.degree_of_vertex(*v));
    }

    // std::cout << "Out of loop: 1" << std::endl;

    while (mpq.size() != 3) {
        // std::cout << "In loop: here 1" << std::endl;
        std::pair<Vertex_ID, unsigned int> top = mpq.top();
        // std::cout << "In loop: here 2" << std::endl;

        if (top.second == 0) {
            // choose any other vertex
            // I feel like if the previous parts are done correctly there should be nothing with degree 0....
            // Since we are dealing with rigid subsystems on graphs of size >= 3... making them into partial 2 trees should never make a free vertex
            // assert(false); // lazy, should make real errors

            Vertex_Iterator v, v_end;
            for (boost::tie(v, v_end) = mgc.vertices(); v != v_end; v++) {
                if (top.first != *v) {
                    // add in a single edge to an arbitrary vertex (that isn't itself)
                    Edge_ID e = mgc.add_edge(top.first, *v, default_edge_length);
                    add_to_copy.push_back(std::make_pair(top.first, *v));

                    // adjust priority queue
                    mpq.change_key(top.first, 1);
                    mpq.change_key(*v, mgc.degree_of_vertex(*v));

                    break;
                }
            }
        } else if (top.second == 1) {
            // choose any vertex adjacent to the vertex it's adjacent to
            // except it could be just an edge floating in space...

            // Shouldn't occur for same reason as 0
            // assert(false); // lazy, should make real errors

            // std::cout << "Handling " << mgc[top.first].name <<", with degree 1" << std::endl;

            Vertex_ID v_adj = *(mgc.vertices_adjacent(top.first).begin());
            Vertex_ID v_adj_adj;

            std::set<Vertex_ID> adj_adj_vs = mgc.vertices_adjacent(v_adj);
            v_adj_adj = get_not_self_in_set(top.first, adj_adj_vs);

            // add in edge to return list
            add_to_copy.push_back(std::make_pair(top.first, v_adj_adj));

            // it's now an ear, remove the vertex from the graph and priority queue
            mgc.remove_vertex(top.first);
            mpq.pop();

            // adjust degree of adjacent vertex
            mpq.change_key(v_adj, mgc.degree_of_vertex(v_adj));
        } else if (top.second == 2) {
            // if it's part of a trianlge
            // if the adjacent vertices lie on the same edge, it's a triangle

            // By the 2-ear theorem, every triangulation will have >=2 degree 2
            // vertices. So we remove it if it's part of a triangle.
            // Eventually, this will uncover (if not already there) some vertices
            // with degree 2 that are not ears. Then we add in edges.

            // std::cout << "Handling " << mgc[top.first].name <<", with degree 2" << std::endl;

            std::set<Vertex_ID> adj_vs = mgc.vertices_adjacent(top.first);
            std::set<Vertex_ID>::iterator v_it = adj_vs.begin();
            // std::cout << "degree 2: here 1" << std::endl;
            Vertex_ID v0 = *v_it;
            v_it++;
            Vertex_ID v1 = *v_it;
            // std::cout << "degree 2: here 2" << std::endl;

            if (mgc.has_edge(v0, v1)) {
                // std::cout << "degree 2: here 3" << std::endl;
                // this vertex is an ear

                // remove the ear, from graph and priority queue
                mgc.remove_vertex(top.first);
                mpq.pop();

                // adjust the priority queue degrees
                mpq.change_key(v0, mgc.degree_of_vertex(v0));
                mpq.change_key(v1, mgc.degree_of_vertex(v1));
                // std::cout << "degree 2: here 4" << std::endl;
            } else {
                // std::cout << "degree 2: here 5" << std::endl;
                // add the edge between its 2 neighbors
                mgc.add_edge(v0, v1, default_edge_length);
                add_to_copy.push_back(std::make_pair(v0, v1));

                // it's now an ear, remove it from the graph and priority queue
                mpq.pop();
                mgc.remove_vertex(top.first);

                // the other vertices did not change degree, you're done!

                // std::cout << "degree 2: here 6" << std::endl;
            }
            // std::cout << "degree 2: here 7" << std::endl;
        } else {
            // std::cout << "Handling " << mgc[top.first].name <<", with degree " << top.second << std::endl;
            break;
        }
    }

    // std::cout << "Out of loop: 2" << std::endl;
    // this->nonedge_interval.clear();
    for (std::vector<std::pair<Vertex_ID, Vertex_ID> >::iterator es = add_to_copy.begin(); es != add_to_copy.end(); es++) {
        // std::cout << "Out of loop: adding edge: (" << (*working_copy)[es->first].name << ", " << (*working_copy)[es->second].name << ")" << std::endl;
        Edge_ID e = working_copy->add_edge(mgc.original_vertex(es->first), mgc.original_vertex(es->second), default_edge_length);
        ret.push_back(e);
        // *es = std::make_pair(mgc.original_vertex(es->first), mgc.original_vertex(es->second));
        // this->nonedge_interval[e] = std::make_pair(0,0);
        // std::cout << "Added" << std::endl;

    }
    // working_copy->set_layout();
    // working_copy->get_graph_in_range(-0.87, 0.87, -0.87, 0.87);


    // std::cout << "Out of loop: 3" << std::endl;
    return ret;
}



// // takes a realized edge and recursively realizes a 2-tree
// bool Isostatic_Graph_Realizer::_realize_2_tree_aux(
//     Vertex_ID v0,
//     Vertex_ID v1,
//     std::map<Vertex_ID, bool> &already_realized)
// {
//     // want to use the vertex with lesser degree, less looping later
//     bool swapped = false;
//     if (working_copy->degree_of_vertex(v1) < working_copy->degree_of_vertex(v0)) {
//         Vertex_ID temp = v0;
//         v0 = v1;
//         v1 = temp;
//         swapped = true;
//     }

//     std::set<Vertex_ID> vs_adj = working_copy->vertices_adjacent(v0);
//     for (std::set<Vertex_ID>::iterator v_it = vs_adj.begin(); v_it != vs_adj.end(); v_it++) {
//         Vertex_ID v2 = *v_it;

//         // if there is a triangle and the other vertex isn't already realized
//         if (v1 != v2
//             && working_copy->has_edge(v1, v2)
//             && already_realized.find(v2) == already_realized.end()) {

//             // get vertex positions
//             double v0x = (*working_copy)[v0].x, v0y = (*working_copy)[v0].y;
//             double v1x = (*working_copy)[v1].x, v1y = (*working_copy)[v1].y;

//             // get edge lengths
//             double e01 = (*working_copy)[working_copy->edge(v0, v1)].length;
//             double e12 = (*working_copy)[working_copy->edge(v1, v2)].length;
//             double e02 = (*working_copy)[working_copy->edge(v0, v2)].length;

//             // calculate position of third vertex, think intersecting circles
//             double d  = e01;
//             double r0 = e02;
//             double r1 = e12;
//             double a = (r0*r0 - r1*r1 + d*d) / (2*d);
//             double h_sqr = r0*r0 - a*a;

//             // if <0 it can't be realized
//             if (h_sqr < 0) return false;

//             double h = sqrt(h_sqr);
//             double midx = v0x + a*(v1x-v0x)/d;
//             double midy = v0y + a*(v1y-v0y)/d;

//             double v2x = midx + h*(v1y-v0y)/d;
//             double v2y = midy - h*(v1x-v0x)/d;

//             (*working_copy)[v2].set_position(v2x, v2y);

//             already_realized[v2] = true;

//             if (!_realize_2_tree_aux(already_realized, v0, v2)) return false;
//             if (!_realize_2_tree_aux(already_realized, v1, v2)) return false;
//         }
//     }

//     return ret;
// }


// std::list<Mapped_Graph_Copy*> Isostatic_Graph_Realizer::realize_2_tree() {
//     std::map<Vertex_ID, bool> already_realized;

//     // get first vertex
//     Vertex_ID v0 = *(working_copy->vertices().first);

//     // get second (adjacent) vertex
//     std::set<Vertex_ID> vs_adj = working_copy->vertices_adjacent(v0);
//     Vertex_ID v1 = get_not_self_in_set(v0, vs_adj);

//     double e01 = (*working_copy)[working_copy->edge(v0, v1)].length;

//     (*working_copy)[v0].set_position(0, 0);
//     (*working_copy)[v1].set_position(0, e01);

//     already_realized[v0] = true;
//     already_realized[v1] = true;

//     bool ret = _realize_2_tree_aux(v0, v1, already_realized);

//     return ret;
// }



bool triangulate(
    double v0x, double v0y,
    double v1x, double v1y,
    double e01, double e02, double e12,
    double *v2x0, double *v2y0,
    double *v2x1, double *v2y1)
{
    // calculate position of third vertex, think intersecting circles
    double d  = e01;
    double r0 = e02;
    double r1 = e12;
    double a = (r0*r0 - r1*r1 + d*d) / (2*d);
    double h_sqr = r0*r0 - a*a;

    // if <0 it can't be realized
    if (h_sqr < 0) return false;

    double h = sqrt(h_sqr);
    double midx = v0x + a*(v1x-v0x)/d;
    double midy = v0y + a*(v1y-v0y)/d;

    *v2x0 = midx + h*(v1y-v0y)/d;
    *v2y0 = midy - h*(v1x-v0x)/d;

    *v2x1 = midx - h*(v1y-v0y)/d;
    *v2y1 = midy + h*(v1x-v0x)/d;

    return true;
}


// // takes a realized edge and recursively realizes a 2-tree
// std::list<Mapped_Graph_Copy*> Isostatic_Graph_Realizer::_realize_2_tree_aux(
//     Mapped_Graph_Copy *g,
//     Vertex_ID v0,
//     Vertex_ID v1,
//     std::map<Vertex_ID, bool> &already_realized)
// {
//     std::list<Mapped_Graph_Copy*> ret;

//     // want to use the vertex with lesser degree, less looping later
//     bool swapped = false;
//     if (working_copy->degree_of_vertex(v1) < working_copy->degree_of_vertex(v0)) {
//         Vertex_ID temp = v0;
//         v0 = v1;
//         v1 = temp;
//         swapped = true;
//     }

//     std::set<Vertex_ID> vs_adj = working_copy->vertices_adjacent(v0);
//     for (std::set<Vertex_ID>::iterator v_it = vs_adj.begin(); v_it != vs_adj.end(); v_it++) {
//         Vertex_ID v2 = *v_it;

//         // if there is a triangle and the other vertex isn't already realized
//         if (v1 != v2
//             && working_copy->has_edge(v1, v2)
//             && already_realized.find(v2) == already_realized.end())
//         {
//             double v2x0, v2y0, v2x1, v2y1;

//             bool success = triangulate(
//                 (*working_copy)[v0].x, (*working_copy)[v0].y,
//                 (*working_copy)[v1].x, (*working_copy)[v1].y,
//                 (*working_copy)[working_copy->edge(v0, v1)].length,
//                 (*working_copy)[working_copy->edge(v1, v2)].length,
//                 (*working_copy)[working_copy->edge(v0, v2)].length,
//                 &v2x0, &v2y0, &v2x1, &v2y1);

//             if (!success) return ret;

//             Mapped_Graph_Copy *g2 = new Mapped_Graph_Copy(g);
//             ret.push_back(g2);

//             (*g) [v2].set_position(v2x0, v2y0);
//             (*g2)[v2].set_position(v2x1, v2y1);

//             already_realized[v2] = true;

//             ret.splice(ret.end(), _realize_2_tree_aux(g,  v0, v2, already_realized));
//             ret.splice(ret.end(), _realize_2_tree_aux(g,  v1, v2, already_realized));

//             ret.splice(ret.end(), _realize_2_tree_aux(g2, v0, v2, already_realized));
//             ret.splice(ret.end(), _realize_2_tree_aux(g2, v1, v2, already_realized));
//         }
//     }

//     return ret;
// }


// std::list<Mapped_Graph_Copy*> Isostatic_Graph_Realizer::realize_2_tree() {
//     std::map<Vertex_ID, bool> already_realized;

//     // get first vertex
//     Vertex_ID v0 = *(working_copy->vertices().first);

//     // get second (adjacent) vertex
//     std::set<Vertex_ID> vs_adj = working_copy->vertices_adjacent(v0);
//     Vertex_ID v1 = get_not_self_in_set(v0, vs_adj);

//     double e01 = (*working_copy)[working_copy->edge(v0, v1)].length;

//     (*working_copy)[v0].set_position(0, 0);
//     (*working_copy)[v1].set_position(0, e01);

//     already_realized[v0] = true;
//     already_realized[v1] = true;

//     Mapped_Graph_Copy *g = new Mapped_Graph_Copy(*working_copy);
//     std::list<Mapped_Graph_Copy*> ret = _realize_2_tree_aux(
//         g, v0, v1, already_realized);

//     ret.push_front(g);

//     return ret;
// }




std::vector<Vertex_ID> all_triangles_with_edge(const Graph &g, Vertex_ID v0, Vertex_ID v1) {
    std::vector<Vertex_ID> ret;

    // want to use the vertex with lesser degree, less looping later
    if (g.degree_of_vertex(v1) < g.degree_of_vertex(v0)) {
        Vertex_ID temp = v0; v0 = v1; v1 = temp; // swap
    }

    std::set<Vertex_ID> vs_adj = g.vertices_adjacent(v0);
    for (std::set<Vertex_ID>::iterator v_it = vs_adj.begin(); v_it != vs_adj.end(); v_it++)
        if (v1 != *v_it && g.has_edge(v1, *v_it))
            ret.push_back(*v_it);

    return ret;
}

void Isostatic_Graph_Realizer::_realize_2_tree_aux(
    std::list<Mapped_Graph_Copy*> &already_made,
    Vertex_ID orig_v0, Vertex_ID orig_v1, Vertex_ID orig_v_ignore)
{
    Vertex_ID wc_v0 = working_copy->copy_vertex(orig_v0);
    Vertex_ID wc_v1 = working_copy->copy_vertex(orig_v1);

    std::vector<Vertex_ID> triangles = all_triangles_with_edge(
        *working_copy, wc_v0, wc_v1);

    for (std::vector<Vertex_ID>::iterator v2_it = triangles.begin(); v2_it != triangles.end(); v2_it++) {
        Vertex_ID wc_v2 = *v2_it;
        Vertex_ID orig_v2 = working_copy->original_vertex(wc_v2);

        // don't want to go back up the tree
        if (orig_v2 == orig_v_ignore) continue;

        std::vector<std::list<Mapped_Graph_Copy*>::iterator> to_delete;
        std::list<Mapped_Graph_Copy*> to_add;

        // bool at_least_one_realization = false;
        for (std::list<Mapped_Graph_Copy*>::iterator g_it = already_made.begin(); g_it != already_made.end(); g_it++) {
            Mapped_Graph_Copy &g = **g_it;

            Vertex_ID v0 = g.copy_vertex(orig_v0);
            Vertex_ID v1 = g.copy_vertex(orig_v1);
            Vertex_ID v2 = g.copy_vertex(orig_v2);

            double v2x0, v2y0, v2x1, v2y1;

            bool success = triangulate(
                g[v0].x, g[v0].y,
                g[v1].x, g[v1].y,
                g[g.edge(v0, v1)].length,
                g[g.edge(v0, v2)].length,
                g[g.edge(v1, v2)].length,
                &v2x0, &v2y0, &v2x1, &v2y1);

            if (!success) {
                to_delete.push_back(g_it);
                continue;
            }

            Mapped_Graph_Copy *g_new = new Mapped_Graph_Copy(g);
            to_add.push_back(g_new);

            Vertex_ID new_v2 = g_new->copy_vertex(orig_v2);

            g[v2].x            = v2x0; g[v2].y            = v2y0;
            (*g_new)[new_v2].x = v2x1; (*g_new)[new_v2].y = v2y1;
            // at_least_one_realization = true;
        }

        for (std::vector<std::list<Mapped_Graph_Copy*>::iterator>::iterator d = to_delete.begin(); d != to_delete.end(); d++) {
            already_made.erase(*d);
        }

        already_made.splice(already_made.end(), to_add);



        _realize_2_tree_aux(already_made, orig_v0, orig_v2, orig_v1);
        _realize_2_tree_aux(already_made, orig_v1, orig_v2, orig_v0);
    }
}

std::list<Mapped_Graph_Copy*> Isostatic_Graph_Realizer::realize_2_tree() {
    std::list<Mapped_Graph_Copy*> ret;

    Mapped_Graph_Copy *g = new Mapped_Graph_Copy(*working_copy);

    // get first vertex
    Vertex_ID v0 = *(g->vertices().first);

    // get second (adjacent) vertex
    std::set<Vertex_ID> vs_adj = g->vertices_adjacent(v0);
    Vertex_ID v1 = get_not_self_in_set(v0, vs_adj);

    // get third (triangle, adjacent to both) vertex
    // maybe do this more efficiently
    Vertex_ID v2 = all_triangles_with_edge(*g, v0, v1)[0];

    // set position of v0 and v1
    double e01_length = (*g)[g->edge(v0, v1)].length;
    (*g)[v0].set_position(0, 0);
    (*g)[v1].set_position(0, e01_length); // if you reorder here, reorder below

    // get and set position of v2
    double v2x0, v2y0, v2x1, v2y1;
    bool success = triangulate(
        0, 0,
        0, e01_length,
        e01_length,
        (*g)[g->edge(v0, v2)].length,
        (*g)[g->edge(v1, v2)].length,
        &v2x0, &v2y0, &v2x1, &v2y1);
    if (!success) return ret;
    // choice is arbitrary for this first triangle
    (*g)[v2].set_position(v2x0, v2y0);

    // std::cout << "realize_2_tree: 1" << std::endl;
    Vertex_ID
        orig_v0 = g->original_vertex(v0),
        orig_v1 = g->original_vertex(v1),
        orig_v2 = g->original_vertex(v2);
    // std::cout << "realize_2_tree: 2" << std::endl;

    ret.push_back(g);
    _realize_2_tree_aux(ret, orig_v0, orig_v1, orig_v2);
    _realize_2_tree_aux(ret, orig_v0, orig_v2, orig_v1);
    _realize_2_tree_aux(ret, orig_v1, orig_v2, orig_v0);
    // std::cout << "realize_2_tree: 3" << std::endl;

    return ret;
}




// std::vector<Vertex_ID> all_triangles_with_edge(const Graph &g, Vertex_ID v0, Vertex_ID v1) {
//     std::vector<Vertex_ID> ret;

//     // want to use the vertex with lesser degree, less looping later
//     if (g.degree_of_vertex(v1) < g.degree_of_vertex(v0)) {
//         Vertex_ID temp = v0; v0 = v1; v1 = temp; // swap
//     }

//     std::set<Vertex_ID> vs_adj = g.vertices_adjacent(v0);
//     for (std::set<Vertex_ID>::iterator v_it = vs_adj.begin(); v_it != vs_adj.end(); v_it++)
//         if (v1 != *v_it && g.has_edge(v1, *v_it))
//             ret.push_back(*v_it);

//     return ret;
// }



// std::pair<double, double> Isostatic_Graph_Realizer::interval_of_nonedge(Vertex_ID v0, Vertex_ID v1) {
//     assert(working_copy->has_edge(v0, v1));

//     Edge_ID e01 = working_copy->edge(v0, v1);
//     assert(nonedge_interval.find(e01) != nonedge_interval.end());

//     std::pair<double, double> ret;
//     std::vector<Vertex_ID> triangles = all_triangles_with_edge(*working_copy, v0, v1);

//     for (std::vector<Vertex_ID>::iterator v2 = triangles.begin(); v2 != triangles.end(); v2++) {
//         Edge_ID e02 = working_copy->edge(v0, *v2);
//         Edge_ID e12 = working_copy->edge(v1, *v2);

//         double mn, mx;

//         // check if the other edges are nonedges (added edges)
//         if (nonedge_interval.find(e02) == nonedge_interval.end() && nonedge_interval.find(e12) == nonedge_interval.end()) {
//             // both are normal edges
//             mn = (*working_copy)[e02].length - (*working_copy)[e12].length;
//             mx = (*working_copy)[e02].length + (*working_copy)[e12].length;
//             if (mn < 0) mn = -mn;
//         } else if (nonedge_interval.find(e02) != nonedge_interval.end() && nonedge_interval.find(e12) != nonedge_interval.end()) {
//             // both are nonedges
//             double
//                 e02_min = nonedge_interval[e02].first,
//                 e02_max = nonedge_interval[e02].second,
//                 e12_min = nonedge_interval[e12].first,
//                 e12_max = nonedge_interval[e12].second;

//             mx = e02_max + e12_max;
//             if (e02_max >= e12_min || e12_max >= e02_min) {
//                 mn = 0;
//             } else {}

//             // mn = nonedge_interval[e02].first  - nonedge_interval[e12].first;
//             // mx = nonedge_interval[e02].second + nonedge_interval[e12].second;
//             // if (mn < 0) mn = -mn;
//         } else {
//             // one is nonedge, the other is not. Swap to make consistent
//             if (nonedge_interval.find(e12) == nonedge_interval.end()) {
//                 Vertex_ID tempv = v0; v0 = v1; v1 = tempv;
//                 Edge_ID tempe = e02; e02 = e12; e12 = tempe;
//             }

//             // e02 is nonedge, e12 is edge
//             double
//                 e02_min = nonedge_interval[e02].first,
//                 e02_max = nonedge_interval[e02].second,
//                 e12_len = (*working_copy)[e12].length;

//             mx = e02_max + e12_len;
//             if (e12_len < e02_min) {
//                 mn = e02_min - e12_len;
//             } else if (e12_len > e02_max) {
//                 mn = e12_len - e02_max;
//             } else {
//                 mn = 0;
//             }
//         }


//     }

//     return ret;
// }






std::vector<Vertex_ID> all_triangles_with_edge(
    const Graph &g,
    std::set<Edge_ID> &nonedges,
    Vertex_ID v0,
    Vertex_ID v1)
    // Edge_ID e)
{
    std::vector<Vertex_ID> ret;

    // want to use the vertex with lesser degree, less looping later
    if (g.degree_of_vertex(v1) < g.degree_of_vertex(v0)) {
        Vertex_ID temp = v0; v0 = v1; v1 = temp; // swap
    }

    std::set<Vertex_ID> vs_adj = g.vertices_adjacent(v0);
    for (std::set<Vertex_ID>::iterator v_it = vs_adj.begin(); v_it != vs_adj.end(); v_it++)
        if (v1 != *v_it
          && g.has_edge(v1, *v_it)
          && nonedges.find(g.edge(v0, *v_it)) == nonedges.end()
          && nonedges.find(g.edge(v1, *v_it)) == nonedges.end())
            ret.push_back(*v_it);

    return ret;
}


std::pair<double, double> Isostatic_Graph_Realizer::interval_of_nonedge(
    std::set<Edge_ID> &nonedges,
    Edge_ID e)
{
    Vertex_ID v0, v1;
    boost::tie(v0, v1) = working_copy->vertices_incident(e);
    // Vertex_ID v0 = .first;
    // Vertex_ID v1 = working_copy->vertices_incident(e).second;
    Edge_ID e01 = e;

    std::pair<double, double> ret;
    std::vector<Vertex_ID> triangles = all_triangles_with_edge(*working_copy, nonedges, v0, v1);

    bool unassigned = true;
    for (std::vector<Vertex_ID>::iterator v2 = triangles.begin(); v2 != triangles.end(); v2++) {
        Edge_ID e02 = working_copy->edge(v0, *v2);
        Edge_ID e12 = working_copy->edge(v1, *v2);

        double mn, mx;

        // both are normal edges
        mn = (*working_copy)[e02].length - (*working_copy)[e12].length;
        mx = (*working_copy)[e02].length + (*working_copy)[e12].length;
        if (mn < 0) mn = -mn;

        if (unassigned) {
            ret.first  = mn;
            ret.second = mx;
            unassigned = false;
        } else {
            if (mn > ret.first)  ret.first  = mn;
            if (mx < ret.second) ret.second = mx;
        }
    }

    assert(ret.second > ret.first);

    return ret;
}

std::list<Mapped_Graph_Copy*> Isostatic_Graph_Realizer::sample(
    std::set<Edge_ID> &nonedges,
    std::list<Edge_ID> &nonedges_ordered,
    std::vector<std::pair<Vertex_ID, Vertex_ID> > &dropped_edges,
    Edge_ID e)
{
    assert(nonedges.size() == nonedges_ordered.size());

    std::list<Mapped_Graph_Copy*> ret;

    // Vertex_ID v0, v1;
    // boost::tie(v0, v1) = working_copy->vertices_incident(e);
    // std::cout << "sample: Edge (" << (*working_copy)[v0].name << ", " << (*working_copy)[v1].name << ")" << std::endl;

    // int steps = 500;
    // double eps = 1e-1;
    int steps = 2;
    double eps = 1e-2;
    // int steps = 100;
    // double eps = 6;

    std::pair<double, double> interval = interval_of_nonedge(nonedges, e);
    // std::cout << "sample: Interval (" << interval.first << ", " << interval.second << ")" << std::endl;

    Edge_ID next_e;
    bool has_next_e = !nonedges.empty();
    if (has_next_e) {
        next_e = nonedges_ordered.front();
        nonedges_ordered.pop_front();
        nonedges.erase(next_e);
    }

    for (unsigned int i = 1; i <= steps; i++) {
        // one dimension of the Cayley point
        double cayley_point = interval.first + (interval.second - interval.first) * ((double) i / steps);
        (*working_copy)[e].length = cayley_point;

        // std::cout << "sample: value = " << cayley_point << std::endl;


        if (has_next_e) {
            ret.splice(ret.begin(), sample(nonedges, nonedges_ordered, dropped_edges, next_e));
        } else {
            std::list<Mapped_Graph_Copy*> realized = realize_2_tree();
            // std::cout << "sample: size = " << realized.size() << std::endl;
            if (realized.size() == 0) continue;

            // now check the dropped edge lengths...
            // bool satisfied = true;
            // for (std::vector<std::pair<Vertex_ID, Vertex_ID> >::iterator dropped_e = dropped_edges.begin();
            //     satisfied && (dropped_e != dropped_edges.end());
            //     dropped_e++)
            // {
            //     Vertex_ID v0 = dropped_e->first;
            //     Vertex_ID v1 = dropped_e->second;

            //     double length = sqrt(
            //         pow(((*working_copy)[v0].x - (*working_copy)[v1].x), 2) +
            //         pow(((*working_copy)[v0].y - (*working_copy)[v1].y), 2));

            //     double desired_length = (*in_graph)[in_graph->edge(working_copy->original_vertex(v0), working_copy->original_vertex(v1))].length;

            //     if (fabs(length - desired_length) > eps) {
            //         satisfied = false;
            //     }
            //     // else {
            //     //     std::cout
            //     //         << "Sample: satisfied "
            //     //         << length << " - " << desired_length
            //     //         << " = " << fabs(length - desired_length)
            //     //         << std::endl;
            //     // }
            // }

            for (std::list<Mapped_Graph_Copy*>::iterator g_it = realized.begin(); g_it != realized.end(); g_it++) {

                Mapped_Graph_Copy &g = **g_it;

                bool satisfied = true;
                for (std::vector<std::pair<Vertex_ID, Vertex_ID> >::iterator dropped_e = dropped_edges.begin();
                    satisfied && (dropped_e != dropped_edges.end());
                    dropped_e++)
                {
                    Vertex_ID wc_v0   = dropped_e->first;
                    Vertex_ID wc_v1   = dropped_e->second;
                    Vertex_ID orig_v0 = working_copy->original_vertex(wc_v0);
                    Vertex_ID orig_v1 = working_copy->original_vertex(wc_v1);
                    Vertex_ID copy_v0 = g.copy_vertex(orig_v0);
                    Vertex_ID copy_v1 = g.copy_vertex(orig_v1);

                    double length = sqrt(
                        pow((g[copy_v0].x - g[copy_v1].x), 2) +
                        pow((g[copy_v0].y - g[copy_v1].y), 2));

                    double desired_length = (*in_graph)[in_graph->edge(orig_v0, orig_v1)].length;

                    if (fabs(length - desired_length) > eps) {
                        satisfied = false;
                        break;
                    }
                }

                if (satisfied)
                    ret.push_back(&g);
                else {
                    // delete it...
                }
            }
        }
    }

    return ret;
}
