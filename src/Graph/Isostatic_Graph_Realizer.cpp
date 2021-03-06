#include "Isostatic_Graph_Realizer.hpp"

#include "Min_Priority_Queue.hpp"

#include <cmath>
#include <iostream>




Isostatic_Graph_Realizer::Isostatic_Graph_Realizer(Graph *g) :
    in_graph(g),
    working_copy(new Mapped_Graph_Copy(g))
{
    // // make a copy to work with
    // this->working_copy = new Mapped_Graph_Copy(g);

    // drop original edges to get partial 2-tree
    wc_graph_dropped = make_partial_2_tree(working_copy);
    // fill up the set of dropped edges in terms of the input graph
    in_graph_dropped.clear();
    for (std::vector<std::pair<Vertex_ID, Vertex_ID> >::iterator v_p_it = wc_graph_dropped.begin(); v_p_it != wc_graph_dropped.end(); v_p_it++) {
        Vertex_ID orig0 = working_copy->original_vertex(v_p_it->first);
        Vertex_ID orig1 = working_copy->original_vertex(v_p_it->second);
        in_graph_dropped.insert(in_graph->edge(orig0, orig1));
    }

    // add back in new edges to get 2-tree
    wc_graph_added_ordered = make_2_tree(working_copy);
    // fill up the vector of added edges in terms of the input graph
    // fill up the set of added edges in terms of copy graph
    in_graph_added.clear();
    wc_graph_added.clear();
    for (std::list<Edge_ID>::iterator e_it = wc_graph_added_ordered.begin(); e_it != wc_graph_added_ordered.end(); e_it++) {
        std::pair<Vertex_ID, Vertex_ID> vs = working_copy->vertices_incident(*e_it);
        in_graph_added.push_back(std::make_pair(working_copy->original_vertex(vs.first), working_copy->original_vertex(vs.second)));
        wc_graph_added.insert(*e_it);
    }


    realization_order = realize_2_tree_order(*working_copy);


    init_sampling_uniform();
}

Isostatic_Graph_Realizer::~Isostatic_Graph_Realizer() {
    delete(working_copy);
    for (std::list<Mapped_Graph_Copy*>::iterator r_it = _realizations.begin(); r_it != _realizations.end(); r_it++) {
        delete(*r_it);
    }
}

std::list<Mapped_Graph_Copy*>& Isostatic_Graph_Realizer::realizations() {
    return _realizations;
}


// in terms of the input graph
const std::set<Edge_ID>& Isostatic_Graph_Realizer::list_of_dropped() const {
    return in_graph_dropped;
}

// in terms of the input graph
const std::vector<std::pair<Vertex_ID, Vertex_ID> >& Isostatic_Graph_Realizer::list_of_added() const {
    return in_graph_added;
}

bool Isostatic_Graph_Realizer::is_partial_2_tree(Graph* graph) {
    Min_Priority_Queue<Vertex_ID, unsigned int> mpq;
    Vertex_Iterator v, v_end;
    for (boost::tie(v, v_end) = graph->vertices(); v != v_end; v++) {
        mpq.insert(*v, graph->degree_of_vertex(*v));
    }

    // while (!mpq.empty()) {
    //     std::pair<Vertex_ID, unsigned int> top = mpq.pop_top();
    //     std::cout << "(" << (*graph)[top.first].name << ", " << top.second << ") ";
    // }
    // std::cout << std::endl;

    while (!mpq.empty()) {
        std::pair<Vertex_ID, unsigned int> top = mpq.pop_top();
        // std::cout << "(" << (graph)[top.first].name << ", " << top.second << ")" << std::endl;

        if (top.second == 0) {
            // can simply remove graph
            graph->remove_vertex(top.first);
        } else if (top.second == 1) {
            std::pair<Graph_Adj_Iterator, Graph_Adj_Iterator> v_adj
                = boost::adjacent_vertices(top.first, *graph);
            Vertex_ID v = *(v_adj.first);

            // Contract it into the graph.
            // this->contractions.push_back(std::make_pair(top.first, v));
            graph->contract_edge(top.first, v);

            // No other degrees change besides v and the one popped
            mpq.change_key(v, graph->degree_of_vertex(v));
        } else if (top.second == 2) {
            std::pair<Graph_Adj_Iterator, Graph_Adj_Iterator> v_adj
                = boost::adjacent_vertices(top.first, *graph);

            // make a vector of the adj verts, iterators go bad after contraction
            std::vector<Vertex_ID> v_adj_vec;
            for (Graph_Adj_Iterator v_adj_it = v_adj.first; v_adj_it != v_adj.second; ++v_adj_it) {
                v_adj_vec.push_back(*v_adj_it);
            }

            // contract
            // this->contractions.push_back(std::make_pair(top.first, v_adj_vec[0]));
            graph->contract_edge(top.first, v_adj_vec[0]);

            // update degrees
            for (int i = 0; i < v_adj_vec.size(); i++) {
                mpq.change_key(v_adj_vec[i], graph->degree_of_vertex(v_adj_vec[i]));
            }
        } else {
            break;
        }
    }

    if (graph->num_vertices() == 0)
        return true;
    else
        return false;
}




// returns a list of dropped edges
// std::vector<std::pair<Vertex_ID, Vertex_ID> > Isostatic_Graph_Realizer::make_partial_2_tree() {
std::vector<std::pair<Vertex_ID, Vertex_ID> > Isostatic_Graph_Realizer::make_partial_2_tree(
    Graph* graph)
{
    assert(graph->num_vertices() >= 3);

    std::vector<std::pair<Vertex_ID, Vertex_ID> > ret;

    // Copy the graph, check if it's a partial 2 tree (does all the contractions, altering the copy)
    // Checks the leftover edges, if they're in the original graph, they are removed from the original graph
    Mapped_Graph_Copy *mgc = new Mapped_Graph_Copy(graph);
    while (!is_partial_2_tree(mgc)) {
        Edge_Iterator e, e_end;
        for (boost::tie(e, e_end) = mgc->edges(); e != e_end; e++) {
            std::pair<Vertex_ID, Vertex_ID> vs = mgc->vertices_incident(*e);
            Vertex_ID v0 = mgc->original_vertex(vs.first);
            Vertex_ID v1 = mgc->original_vertex(vs.second);
            // Vertex_ID v0_orig = graph->original_vertex(v0);
            // Vertex_ID v1_orig = graph->original_vertex(v1);

            if (graph->has_edge(v0, v1)) {
                // ret.push_back(in_graph->edge(v0_orig, v1_orig));
                ret.push_back(std::make_pair(v0, v1));
                graph->remove_edge(v0, v1);
                break;
            }
        }

        // delete and make the new one. Needs to be pointers or else copy
        // constructor is invoked and the Mapped_Graph_Copy maps stop working.
        delete(mgc);
        mgc = new Mapped_Graph_Copy(graph);
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
std::list<Edge_ID> Isostatic_Graph_Realizer::make_2_tree(Graph* graph) {
    const double default_edge_length = 1.0;

    std::list<Edge_ID> ret;
    std::vector<std::pair<Vertex_ID, Vertex_ID> > add_to_copy;

    Mapped_Graph_Copy mgc = Mapped_Graph_Copy(graph);

    Min_Priority_Queue<Vertex_ID, unsigned int> mpq;
    Vertex_Iterator v, v_end;
    for (boost::tie(v, v_end) = mgc.vertices(); v != v_end; v++) {
        mpq.insert(*v, mgc.degree_of_vertex(*v));
    }


    while (mpq.size() != 3) {
        std::pair<Vertex_ID, unsigned int> top = mpq.top();

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


            std::set<Vertex_ID> adj_vs = mgc.vertices_adjacent(top.first);
            std::set<Vertex_ID>::iterator v_it = adj_vs.begin();

            Vertex_ID v0 = *v_it;
            v_it++;
            Vertex_ID v1 = *v_it;

            if (mgc.has_edge(v0, v1)) {
                // this vertex is an ear

                // remove the ear, from graph and priority queue
                mgc.remove_vertex(top.first);
                mpq.pop();

                // adjust the priority queue degrees
                mpq.change_key(v0, mgc.degree_of_vertex(v0));
                mpq.change_key(v1, mgc.degree_of_vertex(v1));
            } else {
                // add the edge between its 2 neighbors
                mgc.add_edge(v0, v1, default_edge_length);
                add_to_copy.push_back(std::make_pair(v0, v1));

                // it's now an ear, remove it from the graph and priority queue
                mpq.pop();
                mgc.remove_vertex(top.first);

                // the other vertices did not change degree, you're done!
            }
        } else {
            break;
        }
    }

    for (std::vector<std::pair<Vertex_ID, Vertex_ID> >::iterator es = add_to_copy.begin(); es != add_to_copy.end(); es++) {
        Edge_ID e = graph->add_edge(mgc.original_vertex(es->first), mgc.original_vertex(es->second), default_edge_length);
        ret.push_back(e);
    }

    return ret;
}


std::vector<Vertex_ID> Isostatic_Graph_Realizer::all_triangles_with_edge(
    const Graph &g,
    Vertex_ID v0,
    Vertex_ID v1)
{
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


std::vector<Vertex_ID> Isostatic_Graph_Realizer::all_triangles_with_edge(
    const Graph &g,
    Vertex_ID v0,
    Vertex_ID v1,
    const std::set<Edge_ID>& edges_to_ignore)
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
          && edges_to_ignore.find(g.edge(v0, *v_it)) == edges_to_ignore.end()
          && edges_to_ignore.find(g.edge(v1, *v_it)) == edges_to_ignore.end())
            ret.push_back(*v_it);

    return ret;
}

void Isostatic_Graph_Realizer::_realize_2_tree_order_aux(
    const Graph& graph,
    std::vector<Isostatic_Graph_Realizer::realization_triplet> &already_ordered,
    Vertex_ID v0, Vertex_ID v1, Vertex_ID v_ignore)
{
    std::vector<Vertex_ID> ts = all_triangles_with_edge(graph, v0, v1);

    for (std::vector<Vertex_ID>::iterator v_it = ts.begin(); v_it != ts.end(); v_it++) {
        if (*v_it != v_ignore) {
            already_ordered.emplace_back(*v_it, v0, v1);
            _realize_2_tree_order_aux(graph, already_ordered, v0, *v_it, v1);
            _realize_2_tree_order_aux(graph, already_ordered, v1, *v_it, v0);
        }
    }
}

std::vector<Isostatic_Graph_Realizer::realization_triplet> Isostatic_Graph_Realizer::realize_2_tree_order(
    const Graph& graph)
{
    std::vector<realization_triplet> ret;

    // get first vertex
    Vertex_ID v0 = *(graph.vertices().first);

    // get second (adjacent) vertex
    std::set<Vertex_ID> vs_adj = graph.vertices_adjacent(v0);
    Vertex_ID v1 = *(vs_adj.begin());
    // Vertex_ID v1 = get_not_self_in_set(v0, vs_adj);

    // add to ret
    ret.emplace_back(v0, v0, v0); // dummy values for parents in triplet, they'll be ignored
    ret.emplace_back(v1, v1, v1); // dummy values for parents in triplet, they'll be ignored

    // recurse... we use v0 to "ignore" as just a dummy input. It won't actually
    // try to use v0 and we actually want everything it gives us
    _realize_2_tree_order_aux(graph, ret, v0, v1, v0);


    // std::cout << "Realize order:" << std::endl;
    // for (std::vector<Vertex_ID>::iterator v_it = ret.begin(); v_it != ret.end(); v_it++) {
    //     std::cout << "\t" << (*mgc)[*v_it].name << std::endl;
    // }

    return ret;
}


bool Isostatic_Graph_Realizer::triangulate(
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


// result is a list of realized 2-tree, which are mapped copies of in_graph
// TODO: technically, if I only change one vertex position, I only need to update the positions
//// of the vertices occuring after it in the realization_order (and not even all of them)
std::list<Mapped_Graph_Copy*> Isostatic_Graph_Realizer::realize_2_tree(
    const Mapped_Graph_Copy& graph,
    std::vector<realization_triplet>& realization_order)
{
    std::list<Mapped_Graph_Copy*> ret;

    Mapped_Graph_Copy *g = new Mapped_Graph_Copy(graph);

    // v0
    Vertex_ID v0_wkcp = realization_order[0].v;
    Vertex_ID v0_orig = graph.original_vertex(v0_wkcp);
    Vertex_ID v0_newg = g->copy_vertex(v0_orig);

    // v1
    Vertex_ID v1_wkcp = realization_order[1].v;
    Vertex_ID v1_orig = graph.original_vertex(v1_wkcp);
    Vertex_ID v1_newg = g->copy_vertex(v1_orig);

    // set position of v0 and v1
    double e01_length = (*g)[g->edge(v0_newg, v1_newg)].length;
    (*g)[v0_newg].set_position(0, 0);
    (*g)[v1_newg].set_position(0, e01_length); // if you reorder here, reorder below

    // add to return
    ret.push_back(g);

    // iterate through the rest of the vertices
    for (unsigned int i = 2; i < realization_order.size(); i++) {

        // iterate through the realizations (going to have to double the size of the list)
        std::list<Mapped_Graph_Copy*>::iterator
            mgc_it = ret.begin(),
            mgc_end = ret.end();
        mgc_end--;
        while (true) {
            // assert(*mgc_it != NULL);

            // vi
            Vertex_ID vi_wkcp = realization_order[i].v;
            Vertex_ID vi_orig = graph.original_vertex(vi_wkcp);
            Vertex_ID vi_newg = (*mgc_it)->copy_vertex(vi_orig);

            // vi_parent_0
            Vertex_ID vip0_wkcp = realization_order[i].v_parent_0;
            Vertex_ID vip0_orig = graph.original_vertex(vip0_wkcp);
            Vertex_ID vip0_newg = (*mgc_it)->copy_vertex(vip0_orig);

            // vi_parent_1
            Vertex_ID vip1_wkcp = realization_order[i].v_parent_1;
            Vertex_ID vip1_orig = graph.original_vertex(vip1_wkcp);
            Vertex_ID vip1_newg = (*mgc_it)->copy_vertex(vip1_orig);

            // edges
            Edge_ID e_p0_p1 = (*mgc_it)->edge(vip0_newg, vip1_newg);
            Edge_ID e_vi_p0 = (*mgc_it)->edge(vi_newg,   vip0_newg);
            Edge_ID e_vi_p1 = (*mgc_it)->edge(vi_newg,   vip1_newg);

            // get lengths and stuff
            double vip0_x = (**mgc_it)[vip0_newg].x;
            double vip0_y = (**mgc_it)[vip0_newg].y;
            double vip1_x = (**mgc_it)[vip1_newg].x;
            double vip1_y = (**mgc_it)[vip1_newg].y;
            double e_p0_p1_length = (**mgc_it)[e_p0_p1].length;
            double e_vi_p0_length = (**mgc_it)[e_vi_p0].length;
            double e_vi_p1_length = (**mgc_it)[e_vi_p1].length;

            // get and set position of v2
            double v2x0, v2y0, v2x1, v2y1;
            bool success = triangulate(
                vip0_x, vip0_y,
                vip1_x, vip1_y,
                e_p0_p1_length,
                e_vi_p0_length,
                e_vi_p1_length,
                &v2x0, &v2y0, &v2x1, &v2y1);

            if (success) {
                // give first value, duplicate, and add to list
                (**mgc_it)[vi_newg].set_position(v2x0, v2y0);
                Mapped_Graph_Copy *duplicate = new Mapped_Graph_Copy(**mgc_it);
                ret.push_back(duplicate);

                // reassign with second value
                (**mgc_it)[vi_newg].set_position(v2x1, v2y1);

                // exit condition
                if (mgc_it == mgc_end) {
                    break;
                } else {
                    mgc_it++;
                }
            } else {
                // delete this one and exit if size of list is 0
                if (mgc_it == mgc_end) {
                    delete(*mgc_it);
                    ret.erase(mgc_it);
                    break;
                } else {
                    std::list<Mapped_Graph_Copy*>::iterator mgc_current = mgc_it;
                    mgc_it++;
                    delete(*mgc_current);
                    ret.erase(mgc_current);
                }
            }
        }

        if (ret.size() == 0)
            break;
    }

    return ret;
}


// std::list<Mapped_Graph_Copy*> Isostatic_Graph_Realizer::realize_2_tree(
//     const Mapped_Graph_Copy& graph,
//     std::vector<realization_triplet>& realization_order)
// {
//     std::list<Mapped_Graph_Copy*> ret;

//     std::list<Isostatic_Graph_Realizer::Oriented_Framework> r = realize_2_tree_solutions(graph, realization_order);
//     for (auto r_it = r.begin(); r_it != r.end(); r_it++) {
//         ret.push_back(r_it->graph);
//     }

//     return ret;
// }

// same as above, only it uses Solution class
std::list<Isostatic_Graph_Realizer::Oriented_Framework> Isostatic_Graph_Realizer::realize_2_tree_solutions(
    const Mapped_Graph_Copy& graph,
    std::vector<realization_triplet>& realization_order)
{
    std::list<Oriented_Framework> ret;

    Oriented_Framework s;
    s.graph = new Mapped_Graph_Copy(graph);
    Mapped_Graph_Copy* g = s.graph;

    // v0
    Vertex_ID v0_wkcp = realization_order[0].v;
    Vertex_ID v0_orig = graph.original_vertex(v0_wkcp);
    Vertex_ID v0_newg = g->copy_vertex(v0_orig);

    // v1
    Vertex_ID v1_wkcp = realization_order[1].v;
    Vertex_ID v1_orig = graph.original_vertex(v1_wkcp);
    Vertex_ID v1_newg = g->copy_vertex(v1_orig);

    // set position of v0 and v1
    double e01_length = (*g)[g->edge(v0_newg, v1_newg)].length;
    (*g)[v0_newg].set_position(0, 0);
    (*g)[v1_newg].set_position(0, e01_length); // if you reorder here, reorder below

    // add to return
    ret.push_back(s);

    // iterate through the rest of the vertices
    for (unsigned int i = 2; i < realization_order.size(); i++) {

        // iterate through the realizations (going to have to double the size of the list)
        std::list<Oriented_Framework>::iterator
            orf_it = ret.begin(),
            orf_end = ret.end();
        orf_end--;
        while (true) {
            Mapped_Graph_Copy* g_copy = orf_it->graph;

            // vi
            Vertex_ID vi_wkcp = realization_order[i].v;
            Vertex_ID vi_orig = graph.original_vertex(vi_wkcp);
            Vertex_ID vi_newg = g_copy->copy_vertex(vi_orig);

            // vi_parent_0
            Vertex_ID vip0_wkcp = realization_order[i].v_parent_0;
            Vertex_ID vip0_orig = graph.original_vertex(vip0_wkcp);
            Vertex_ID vip0_newg = g_copy->copy_vertex(vip0_orig);

            // vi_parent_1
            Vertex_ID vip1_wkcp = realization_order[i].v_parent_1;
            Vertex_ID vip1_orig = graph.original_vertex(vip1_wkcp);
            Vertex_ID vip1_newg = g_copy->copy_vertex(vip1_orig);

            // edges
            Edge_ID e_p0_p1 = g_copy->edge(vip0_newg, vip1_newg);
            Edge_ID e_vi_p0 = g_copy->edge(vi_newg,   vip0_newg);
            Edge_ID e_vi_p1 = g_copy->edge(vi_newg,   vip1_newg);

            // get lengths and stuff
            double vip0_x = (*g_copy)[vip0_newg].x;
            double vip0_y = (*g_copy)[vip0_newg].y;
            double vip1_x = (*g_copy)[vip1_newg].x;
            double vip1_y = (*g_copy)[vip1_newg].y;
            double e_p0_p1_length = (*g_copy)[e_p0_p1].length;
            double e_vi_p0_length = (*g_copy)[e_vi_p0].length;
            double e_vi_p1_length = (*g_copy)[e_vi_p1].length;

            // get and set position of v2
            double v2x0, v2y0, v2x1, v2y1;
            bool success = triangulate(
                vip0_x, vip0_y,
                vip1_x, vip1_y,
                e_p0_p1_length,
                e_vi_p0_length,
                e_vi_p1_length,
                &v2x0, &v2y0, &v2x1, &v2y1);

            if (success) {
                // give first value, duplicate, and add to list
                (*g_copy)[vi_newg].set_position(v2x0, v2y0);
                ret.emplace_back();
                ret.back().graph = new Mapped_Graph_Copy(*g_copy);
                ret.back().orientation = orf_it->orientation;
                ret.back().orientation.push_back(Oriented_Framework::ot_0);

                // reassign with second value
                (*g_copy)[vi_newg].set_position(v2x1, v2y1);
                ret.back().orientation.push_back(Oriented_Framework::ot_1);

                // exit condition
                if (orf_it == orf_end) {
                    break;
                } else {
                    orf_it++;
                }
            } else {
                // delete this one and exit if size of list is 0
                if (orf_it == orf_end) {
                    delete(orf_it->graph);
                    ret.erase(orf_it);
                    break;
                } else {
                    std::list<Oriented_Framework>::iterator orf_current = orf_it;
                    orf_it++;
                    delete(orf_current->graph);
                    ret.erase(orf_current);
                }
            }
        }

        if (ret.size() == 0)
            break;
    }

    return ret;
}

Mapped_Graph_Copy* Isostatic_Graph_Realizer::realize_2_tree_with_orientation(
    const Mapped_Graph_Copy& graph,
    std::vector<realization_triplet>& realization_order,
    std::vector<Oriented_Framework::orientation_types>& orientation)
{
    assert(realization_order.size() == orientation.size() + 2);

    Mapped_Graph_Copy *g = new Mapped_Graph_Copy(graph);

    // v0
    Vertex_ID v0_wkcp = realization_order[0].v;
    Vertex_ID v0_orig = graph.original_vertex(v0_wkcp);
    Vertex_ID v0_newg = g->copy_vertex(v0_orig);

    // v1
    Vertex_ID v1_wkcp = realization_order[1].v;
    Vertex_ID v1_orig = graph.original_vertex(v1_wkcp);
    Vertex_ID v1_newg = g->copy_vertex(v1_orig);

    // set position of v0 and v1
    double e01_length = (*g)[g->edge(v0_newg, v1_newg)].length;
    (*g)[v0_newg].set_position(0, 0);
    (*g)[v1_newg].set_position(0, e01_length); // if you reorder here, reorder below

    // iterate through the rest of the vertices
    for (unsigned int i = 2; i < realization_order.size(); i++) {
        // vi
        Vertex_ID vi_wkcp = realization_order[i].v;
        Vertex_ID vi_orig = graph.original_vertex(vi_wkcp);
        Vertex_ID vi_newg = g->copy_vertex(vi_orig);

        // vi_parent_0
        Vertex_ID vip0_wkcp = realization_order[i].v_parent_0;
        Vertex_ID vip0_orig = graph.original_vertex(vip0_wkcp);
        Vertex_ID vip0_newg = g->copy_vertex(vip0_orig);

        // vi_parent_1
        Vertex_ID vip1_wkcp = realization_order[i].v_parent_1;
        Vertex_ID vip1_orig = graph.original_vertex(vip1_wkcp);
        Vertex_ID vip1_newg = g->copy_vertex(vip1_orig);

        // edges
        Edge_ID e_p0_p1 = g->edge(vip0_newg, vip1_newg);
        Edge_ID e_vi_p0 = g->edge(vi_newg,   vip0_newg);
        Edge_ID e_vi_p1 = g->edge(vi_newg,   vip1_newg);

        // get lengths and stuff
        double vip0_x = (*g)[vip0_newg].x;
        double vip0_y = (*g)[vip0_newg].y;
        double vip1_x = (*g)[vip1_newg].x;
        double vip1_y = (*g)[vip1_newg].y;
        double e_p0_p1_length = (*g)[e_p0_p1].length;
        double e_vi_p0_length = (*g)[e_vi_p0].length;
        double e_vi_p1_length = (*g)[e_vi_p1].length;

        // get and set position of v2
        double v2x0, v2y0, v2x1, v2y1;
        bool success = triangulate(
            vip0_x, vip0_y,
            vip1_x, vip1_y,
            e_p0_p1_length,
            e_vi_p0_length,
            e_vi_p1_length,
            &v2x0, &v2y0, &v2x1, &v2y1);

        if (success) {
            if (orientation[i-2] == Oriented_Framework::ot_0) {
                (*g)[vi_newg].set_position(v2x0, v2y0);
            } else { // orientation[i-2] == Oriented_Framework::ot_1
                (*g)[vi_newg].set_position(v2x1, v2y1);
            }
        } else {
            delete(g);
            g = NULL;
        }
    }

    return g;

}

std::pair<double, double> Isostatic_Graph_Realizer::interval_of_edge(
    const Graph& graph,
    Edge_ID e,
    const std::set<Edge_ID>& edges_to_ignore)
{
    Vertex_ID v0, v1;
    boost::tie(v0, v1) = graph.vertices_incident(e);
    // Vertex_ID v0 = .first;
    // Vertex_ID v1 = graph.vertices_incident(e).second;
    Edge_ID e01 = e;

    std::pair<double, double> ret;
    std::vector<Vertex_ID> triangles = all_triangles_with_edge(graph, v0, v1, edges_to_ignore);

    bool unassigned = true;
    for (std::vector<Vertex_ID>::iterator v2 = triangles.begin(); v2 != triangles.end(); v2++) {
        Edge_ID e02 = graph.edge(v0, *v2);
        Edge_ID e12 = graph.edge(v1, *v2);

        double mn, mx;

        // both are normal edges
        mn = graph[e02].length - graph[e12].length;
        mx = graph[e02].length + graph[e12].length;
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

double Isostatic_Graph_Realizer::length(
    double v0x, double v0y, double v1x, double v1y)
{
    return sqrt(
        pow((v0x - v1x), 2) +
        pow((v0y - v1y), 2));
}

void Isostatic_Graph_Realizer::copy_vertex_positions(
    const Mapped_Graph_Copy& from,
    Graph* to)
{
    // copy over vertex positions
    for (std::pair<Vertex_Iterator, Vertex_Iterator> vs = to->vertices(); vs.first != vs.second; vs.first++) {
        Vertex_ID v_orig = *(vs.first);
        Vertex_ID v_disp = from.copy_vertex(v_orig);

        (*to)[v_orig].x = from[v_disp].x;
        (*to)[v_orig].y = from[v_disp].y;
    }

    to->get_graph_in_range(-0.87, 0.87, -0.87, 0.87);
}








////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////











Isostatic_Graph_Realizer::IGR_Context_Uniform::IGR_Context_Uniform(
    Edge_ID copy_e,
    std::pair<double, double> interval,
    unsigned int steps) :
        copy_e(copy_e),
        _interval(interval),
        _step(0), steps(steps)
{
    assert(steps > 1);
}

void Isostatic_Graph_Realizer::IGR_Context_Uniform::reset(const double begin, const double end) {
    _step = 0;
    _interval.first = begin; _interval.second = end;
}

double Isostatic_Graph_Realizer::IGR_Context_Uniform::point() {
    return _interval.first + (_interval.second - _interval.first) * ((double) _step / (steps-1));
}




bool Isostatic_Graph_Realizer::check_realization_lengths(
    const Mapped_Graph_Copy& g,
    const double epsilon)
{
    bool satisfied = true;
    for (std::vector<std::pair<Vertex_ID, Vertex_ID> >::iterator dropped_e = wc_graph_dropped.begin();
        satisfied && (dropped_e != wc_graph_dropped.end());
        dropped_e++)
    {
        Vertex_ID wc_v0   = dropped_e->first;
        Vertex_ID wc_v1   = dropped_e->second;
        Vertex_ID orig_v0 = working_copy->original_vertex(wc_v0);
        Vertex_ID orig_v1 = working_copy->original_vertex(wc_v1);
        Vertex_ID copy_v0 = g.copy_vertex(orig_v0);
        Vertex_ID copy_v1 = g.copy_vertex(orig_v1);

        // double length = sqrt(
        //     pow((g[copy_v0].x - g[copy_v1].x), 2) +
        //     pow((g[copy_v0].y - g[copy_v1].y), 2));
        double length = this->length(g[copy_v0].x, g[copy_v0].y, g[copy_v1].x, g[copy_v1].y);

        double desired_length = (*in_graph)[in_graph->edge(orig_v0, orig_v1)].length;

        if (fabs(length - desired_length) > epsilon) {
            satisfied = false;
            break;
        }
    }

    return satisfied;
}

void Isostatic_Graph_Realizer::init_sampling_uniform() {
    std::set<Edge_ID> wc_graph_added_copy = wc_graph_added;

    std::list<Edge_ID>::iterator e_it = wc_graph_added_ordered.begin();
    std::vector<std::pair<Vertex_ID, Vertex_ID> >::iterator vp_it = wc_graph_dropped.begin();
    std::set<Edge_ID>::iterator e_orig_it = in_graph_dropped.begin();
    for (;
        e_it != wc_graph_added_ordered.end();
        e_it++, vp_it++, e_orig_it++)
    {
        wc_graph_added_copy.erase(*e_it);
        igr_contexts_uniform.emplace_back(
            *e_it,
            interval_of_edge(*working_copy, *e_it, wc_graph_added_copy),
            SAMPLES);
        (*working_copy)[*e_it].length = igr_contexts_uniform.back().point();
    }

    igr_context_current_uniform = igr_contexts_uniform.end();
    igr_context_current_uniform--;

    // TODO: Doesn't sample the very first point
}


void Isostatic_Graph_Realizer::sample_uniform(bool change_input_vertex_positions) {
    // check the realizations
    std::list<Mapped_Graph_Copy*> realized = realize_2_tree(*working_copy, realization_order);
    // std::cout << "realizations: " << realized.size() << std::endl;

    // copy positions if wanted
    if (change_input_vertex_positions && realized.size() != 0) {
        copy_vertex_positions(*(realized.back()), in_graph);
    }

    // check realizations
    for (std::list<Mapped_Graph_Copy*>::iterator g_it = realized.begin(); g_it != realized.end(); g_it++) {
        if (check_realization_lengths(**g_it, EPS)) {
            _realizations.push_back(*g_it);
        } else {
            delete(*g_it);
        }
    }
}

bool Isostatic_Graph_Realizer::step_uniform() {
    // go back up until not done
    while (igr_context_current_uniform->done()) {
        if (igr_context_current_uniform == igr_contexts_uniform.begin()) {
            return false;
        }
        igr_context_current_uniform--;
    }

    igr_context_current_uniform->step();
    (*working_copy)[igr_context_current_uniform->copy_e].length = igr_context_current_uniform->point();

    // get missing edges (edges after this one) for next part
    std::set<Edge_ID> missing_edges;
    std::list<IGR_Context_Uniform>::iterator c_it = igr_context_current_uniform;
    c_it++;
    for (; c_it != igr_contexts_uniform.end(); c_it++)
        missing_edges.insert(c_it->copy_e);

    // go until you reach end
    igr_context_current_uniform++;
    while (igr_context_current_uniform != igr_contexts_uniform.end()) {
        missing_edges.erase(igr_context_current_uniform->copy_e);
        std::pair<double, double> p = interval_of_edge(*working_copy, igr_context_current_uniform->copy_e, missing_edges);
        igr_context_current_uniform->reset(p.first, p.second);

        (*working_copy)[igr_context_current_uniform->copy_e].length = igr_context_current_uniform->point();

        igr_context_current_uniform++;
    }
    igr_context_current_uniform--;

    return true;
}









////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////











Isostatic_Graph_Realizer::IGR_Context::IGR_Context(
    Edge_ID copy_e,
    Edge_ID orig_e_paired,
    std::pair<Vertex_ID, Vertex_ID> orig_e_paired_as_copy_vs,
    double desired_length,
    std::pair<double, double> interval,
    unsigned int steps) :
        copy_e(copy_e),
        orig_e_paired(orig_e_paired),
        orig_e_paired_as_copy_vs(orig_e_paired_as_copy_vs),
        _desired_length(desired_length),
        _interval(interval),
        steps(steps),
        _done_binary(false),
        _done_solutions(false),
        current_range_index(0),
        current_solution_index(0),
        has_input_nonedge_length_for_current_point(false)
{
    assert(steps > 1);

    for (unsigned int i = 0; i < steps; i++) {
        ranges_to_explore.push_back(
            _interval.first + (_interval.second - _interval.first) * ((double) i / (steps-1)));
    }

}

void Isostatic_Graph_Realizer::IGR_Context::reset(const double begin, const double end) {
    _interval.first = begin; _interval.second = end;
}

// TODO: hmm
void Isostatic_Graph_Realizer::IGR_Context::step() {
    if (!_done_binary) {
        length_of_paired_edge_previous = 0;

        //

        if (ranges_to_explore.size() == 1)
            _done_binary = true;
    } else if (!_done_solutions) {
        current_solution_index++;

        if (current_solution_index == solutions.size())
            _done_solutions = true;
    }
}

double Isostatic_Graph_Realizer::IGR_Context::point() {
    if (!_done_binary)
        return ranges_to_explore[current_range_index];
    else
        return solutions[current_solution_index];
}

void Isostatic_Graph_Realizer::IGR_Context::input_nonedge_length_for_current_point(
    double length)
{
    assert(!_done_binary);
    has_input_nonedge_length_for_current_point = true;
}

void Isostatic_Graph_Realizer::IGR_Context::add_solution(double solution) {
    solutions.push_back(solution);
}


// TODO: untested, i think it uses the wrong Vertex_IDs
bool Isostatic_Graph_Realizer::check_realization_length(
    const IGR_Context& context_to_check,
    const Mapped_Graph_Copy& graph_copy,
    const Graph& graph_orig,
    const double epsilon)
{
    Vertex_ID v0 = context_to_check.orig_e_paired_as_copy_vs.first;
    Vertex_ID v1 = context_to_check.orig_e_paired_as_copy_vs.second;
    double
        v0x = graph_copy[v0].x,
        v0y = graph_copy[v0].y,
        v1x = graph_copy[v1].x,
        v1y = graph_copy[v1].y;
    double length = this->length(v0x, v0y, v1x, v1y);
    // double desired_length = graph_orig[context_to_check.orig_e_paired].length;
    return fabs(length - context_to_check._desired_length) < epsilon;
}


// void Isostatic_Graph_Realizer::init_sampling() {
//     // set up contexts
//     std::set<Edge_ID> wc_graph_added_copy = wc_graph_added;

//     std::list<Edge_ID>::iterator e_it = wc_graph_added_ordered.begin();
//     std::vector<std::pair<Vertex_ID, Vertex_ID> >::iterator vp_it = wc_graph_dropped.begin();
//     std::set<Edge_ID>::iterator e_orig_it = in_graph_dropped.begin();
//     for (;
//         e_it != wc_graph_added_ordered.end();
//         e_it++, vp_it++, e_orig_it++)
//     {
//         wc_graph_added_copy.erase(*e_it);
//         igr_contexts.emplace_back(
//             *e_it,
//             *e_orig_it,
//             *vp_it,
//             (*in_graph)[*e_orig_it].length,
//             interval_of_edge(*working_copy, *e_it, wc_graph_added_copy),
//             SAMPLES);
//         (*working_copy)[*e_it].length = igr_contexts.back().point();
//     }

//     igr_context_current = igr_contexts.end();
//     igr_context_current--;

//     // TODO: Doesn't sample the very first point
// }


// // does a binary search on the current context to find satisfying realizations
// // TODO: realize_2_tree should return NULL in places where the orientation type cannot be realized...
// //// not actually a big deal here, since we're using 2-trees... if one works, the other will too
// void Isostatic_Graph_Realizer::sample(bool change_input_vertex_positions) {
//     double
//         l_min = igr_context_current->_interval.first  - EPS,
//         l_max = igr_context_current->_interval.second + EPS,
//         l_mid;

//     std::list<Mapped_Graph_Copy*> realized_min, realized_max, realized_mid;

//     // min
//     do {
//         l_min += EPS;
//         (*working_copy)[igr_context_current->copy_e].length = l_min;
//         realized_min = realize_2_tree(*working_copy, realization_order);
//     } while (realized_min.size() == 0);

//     // max
//     do {
//         l_max -= EPS;
//         (*working_copy)[igr_context_current->copy_e].length = l_max;
//         realized_max = realize_2_tree(*working_copy, realization_order);
//     } while (realized_max.size() == 0);

//     // mid
//     l_mid = (l_max - l_min) / 2;
//     (*working_copy)[igr_context_current->copy_e].length = l_mid;
//     realized_mid = realize_2_tree(*working_copy, realization_order);

//     // should be true for 2-trees....
//     assert(realized_min.size() == realized_mid.size());
//     assert(realized_mid.size() == realized_max.size());



//     // original length
//     double desired_length = (*in_graph)[igr_context_current->orig_e_paired].length;

//     // Vertex_ID
//     Vertex_ID v0_wkcp = igr_context_current->orig_e_paired_as_copy_vs.first;
//     Vertex_ID v1_wkcp = igr_context_current->orig_e_paired_as_copy_vs.second;
//     Vertex_ID v0_orig = working_copy->original_vertex(v0_wkcp);
//     Vertex_ID v1_orig = working_copy->original_vertex(v1_wkcp);

//     std::list<double> length_of_e_min, length_of_e_max, length_of_e_mid;
//     std::list<Mapped_Graph_Copy*>::iterator
//         mgc_min = realized_min.begin(),
//         mgc_max = realized_max.begin(),
//         mgc_mid = realized_mid.begin();
//     for (unsigned int i = 0; i < realized_min.size(); i++) {

//         double
//             v0x = (**mgc_min)[(*mgc_min)->copy_vertex(v0_orig)].x,
//             v0y = (**mgc_min)[(*mgc_min)->copy_vertex(v0_orig)].y,
//             v1x = (**mgc_min)[(*mgc_min)->copy_vertex(v1_orig)].x,
//             v1y = (**mgc_min)[(*mgc_min)->copy_vertex(v1_orig)].y;
//         length_of_e_min.push_back(length(v0x, v0y, v1x, v1y));


//             v0x = (**mgc_max)[(*mgc_max)->copy_vertex(v0_orig)].x;
//             v0y = (**mgc_max)[(*mgc_max)->copy_vertex(v0_orig)].y;
//             v1x = (**mgc_max)[(*mgc_max)->copy_vertex(v1_orig)].x;
//             v1y = (**mgc_max)[(*mgc_max)->copy_vertex(v1_orig)].y;
//         length_of_e_max.push_back(length(v0x, v0y, v1x, v1y));


//             v0x = (**mgc_mid)[(*mgc_mid)->copy_vertex(v0_orig)].x;
//             v0y = (**mgc_mid)[(*mgc_mid)->copy_vertex(v0_orig)].y;
//             v1x = (**mgc_mid)[(*mgc_mid)->copy_vertex(v1_orig)].x;
//             v1y = (**mgc_mid)[(*mgc_mid)->copy_vertex(v1_orig)].y;
//         length_of_e_mid.push_back(length(v0x, v0y, v1x, v1y));

//         mgc_min++;
//         mgc_max++;
//         mgc_mid++;
//     }


//     // // check realizations
//     // for (std::list<Mapped_Graph_Copy*>::iterator g_it = realized.begin(); g_it != realized.end(); g_it++) {
//     //     if (check_realization_length(*igr_context_current, **g_it, *in_graph, EPS)) {
//     //         igr_context_current->add_solution();
//     //     }

//     //     if (igr_context_current == igr_contexts.begin()) {
//     //         _realizations.push_back(*g_it);
//     //     } else {
//     //         delete(*g_it);
//     //     }
//     // }
// }

// bool Isostatic_Graph_Realizer::step() {
//     (*working_copy)[igr_context_current->copy_e].length = igr_context_current->_interval.first;
//     (*working_copy)[igr_context_current->copy_e].length = igr_context_current->_interval.second;




//     // go back up until not done
//     while (igr_context_current->done()) {
//         if (igr_context_current == igr_contexts.begin()) {
//             return false;
//         }
//         igr_context_current--;
//     }

//     igr_context_current->step();
//     (*working_copy)[igr_context_current->copy_e].length = igr_context_current->point();

//     // get missing edges (edges after this one) for next part
//     std::set<Edge_ID> missing_edges;
//     std::list<IGR_Context>::iterator c_it = igr_context_current;
//     c_it++;
//     for (; c_it != igr_contexts.end(); c_it++)
//         missing_edges.insert(c_it->copy_e);

//     // go until you reach end
//     igr_context_current++;
//     while (igr_context_current != igr_contexts.end()) {
//         missing_edges.erase(igr_context_current->copy_e);
//         std::pair<double, double> p = interval_of_edge(*working_copy, igr_context_current->copy_e, missing_edges);
//         igr_context_current->reset(p.first, p.second);

//         (*working_copy)[igr_context_current->copy_e].length = igr_context_current->point();

//         igr_context_current++;
//     }
//     igr_context_current--;

//     return true;
// }























void Isostatic_Graph_Realizer::init_sampling() {
    // set up contexts
    std::set<Edge_ID> wc_graph_added_copy = wc_graph_added;

    std::list<Edge_ID>::iterator e_it = wc_graph_added_ordered.begin();
    std::vector<std::pair<Vertex_ID, Vertex_ID> >::iterator vp_it = wc_graph_dropped.begin();
    std::set<Edge_ID>::iterator e_orig_it = in_graph_dropped.begin();
    for (;
        e_it != wc_graph_added_ordered.end();
        e_it++, vp_it++, e_orig_it++)
    {
        wc_graph_added_copy.erase(*e_it);
        igr_contexts.emplace_back(
            *e_it,
            *e_orig_it,
            *vp_it,
            (*in_graph)[*e_orig_it].length,
            interval_of_edge(*working_copy, *e_it, wc_graph_added_copy),
            SAMPLES);
        (*working_copy)[*e_it].length = igr_contexts.back().point();
    }

    igr_context_current = igr_contexts.end();
    igr_context_current--;

    // set up oriented framework
    working_copy_orf.graph = working_copy;
    for (unsigned int i = 0; i < realization_order.size()-2; i++) {
        working_copy_orf.orientation.push_back(Oriented_Framework::ot_0);
    }

    // TODO: Doesn't sample the very first point
}


// does a binary search on the current context to find satisfying realizations
// TODO: realize_2_tree should return NULL in places where the orientation type cannot be realized...
//// not actually a big deal here, since we're using 2-trees... if one works, the other will too
void Isostatic_Graph_Realizer::sample(bool change_input_vertex_positions) {
    double
        l_min = igr_context_current->_interval.first  - EPS,
        l_max = igr_context_current->_interval.second + EPS,
        l_mid;

    std::list<Mapped_Graph_Copy*> realized_min, realized_max, realized_mid;

    // min
    do {
        l_min += EPS;
        (*working_copy)[igr_context_current->copy_e].length = l_min;
        realized_min = realize_2_tree(*working_copy, realization_order);
    } while (realized_min.size() == 0);

    // max
    do {
        l_max -= EPS;
        (*working_copy)[igr_context_current->copy_e].length = l_max;
        realized_max = realize_2_tree(*working_copy, realization_order);
    } while (realized_max.size() == 0);

    // mid
    l_mid = (l_max - l_min) / 2;
    (*working_copy)[igr_context_current->copy_e].length = l_mid;
    realized_mid = realize_2_tree(*working_copy, realization_order);

    // should be true for 2-trees....
    assert(realized_min.size() == realized_mid.size());
    assert(realized_mid.size() == realized_max.size());



    // original length
    double desired_length = (*in_graph)[igr_context_current->orig_e_paired].length;

    // Vertex_ID
    Vertex_ID v0_wkcp = igr_context_current->orig_e_paired_as_copy_vs.first;
    Vertex_ID v1_wkcp = igr_context_current->orig_e_paired_as_copy_vs.second;
    Vertex_ID v0_orig = working_copy->original_vertex(v0_wkcp);
    Vertex_ID v1_orig = working_copy->original_vertex(v1_wkcp);

    std::list<double> length_of_e_min, length_of_e_max, length_of_e_mid;
    std::list<Mapped_Graph_Copy*>::iterator
        mgc_min = realized_min.begin(),
        mgc_max = realized_max.begin(),
        mgc_mid = realized_mid.begin();
    for (unsigned int i = 0; i < realized_min.size(); i++) {

        double
            v0x = (**mgc_min)[(*mgc_min)->copy_vertex(v0_orig)].x,
            v0y = (**mgc_min)[(*mgc_min)->copy_vertex(v0_orig)].y,
            v1x = (**mgc_min)[(*mgc_min)->copy_vertex(v1_orig)].x,
            v1y = (**mgc_min)[(*mgc_min)->copy_vertex(v1_orig)].y;
        length_of_e_min.push_back(length(v0x, v0y, v1x, v1y));


            v0x = (**mgc_max)[(*mgc_max)->copy_vertex(v0_orig)].x;
            v0y = (**mgc_max)[(*mgc_max)->copy_vertex(v0_orig)].y;
            v1x = (**mgc_max)[(*mgc_max)->copy_vertex(v1_orig)].x;
            v1y = (**mgc_max)[(*mgc_max)->copy_vertex(v1_orig)].y;
        length_of_e_max.push_back(length(v0x, v0y, v1x, v1y));


            v0x = (**mgc_mid)[(*mgc_mid)->copy_vertex(v0_orig)].x;
            v0y = (**mgc_mid)[(*mgc_mid)->copy_vertex(v0_orig)].y;
            v1x = (**mgc_mid)[(*mgc_mid)->copy_vertex(v1_orig)].x;
            v1y = (**mgc_mid)[(*mgc_mid)->copy_vertex(v1_orig)].y;
        length_of_e_mid.push_back(length(v0x, v0y, v1x, v1y));

        mgc_min++;
        mgc_max++;
        mgc_mid++;
    }


    // // check realizations
    // for (std::list<Mapped_Graph_Copy*>::iterator g_it = realized.begin(); g_it != realized.end(); g_it++) {
    //     if (check_realization_length(*igr_context_current, **g_it, *in_graph, EPS)) {
    //         igr_context_current->add_solution();
    //     }

    //     if (igr_context_current == igr_contexts.begin()) {
    //         _realizations.push_back(*g_it);
    //     } else {
    //         delete(*g_it);
    //     }
    // }
}

bool Isostatic_Graph_Realizer::step() {
    (*working_copy)[igr_context_current->copy_e].length = igr_context_current->_interval.first;
    (*working_copy)[igr_context_current->copy_e].length = igr_context_current->_interval.second;




    // go back up until not done
    while (igr_context_current->done_binary()) {
        if (igr_context_current == igr_contexts.begin()) {
            return false;
        }
        igr_context_current--;
    }

    igr_context_current->step();
    (*working_copy)[igr_context_current->copy_e].length = igr_context_current->point();

    // get missing edges (edges after this one) for next part
    std::set<Edge_ID> missing_edges;
    std::list<IGR_Context>::iterator c_it = igr_context_current;
    c_it++;
    for (; c_it != igr_contexts.end(); c_it++)
        missing_edges.insert(c_it->copy_e);

    // go until you reach end
    igr_context_current++;
    while (igr_context_current != igr_contexts.end()) {
        missing_edges.erase(igr_context_current->copy_e);
        std::pair<double, double> p = interval_of_edge(*working_copy, igr_context_current->copy_e, missing_edges);
        igr_context_current->reset(p.first, p.second);

        (*working_copy)[igr_context_current->copy_e].length = igr_context_current->point();

        igr_context_current++;
    }
    igr_context_current--;

    return true;
}
