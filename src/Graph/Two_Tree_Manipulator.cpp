#include "Two_Tree_Manipulator.hpp"

#include "Min_Priority_Queue.hpp"

#include <cmath>
#include <iostream>




Two_Tree_Manipulator::Two_Tree_Manipulator(const Graph& graph) :
    in_graph(graph),
    working_copy(new Mapped_Graph_Copy(&graph))
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
        in_graph_dropped.insert(in_graph.edge(orig0, orig1));
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
}

Two_Tree_Manipulator::~Two_Tree_Manipulator() {
    delete(working_copy);
    for (std::list<Mapped_Graph_Copy*>::iterator r_it = _realizations.begin(); r_it != _realizations.end(); r_it++) {
        delete(*r_it);
    }
}

// in terms of the input graph
const std::set<Edge_ID>& Two_Tree_Manipulator::list_of_dropped() const {
    return in_graph_dropped;
}

// in terms of the input graph
const std::vector<std::pair<Vertex_ID, Vertex_ID> >& Two_Tree_Manipulator::list_of_added() const {
    return in_graph_added;
}

bool Two_Tree_Manipulator::is_partial_2_tree(Graph* graph) {
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
// std::vector<std::pair<Vertex_ID, Vertex_ID> > Two_Tree_Manipulator::make_partial_2_tree() {
std::vector<std::pair<Vertex_ID, Vertex_ID> > Two_Tree_Manipulator::make_partial_2_tree(
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
                // ret.push_back(in_graph.edge(v0_orig, v1_orig));
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
std::list<Edge_ID> Two_Tree_Manipulator::make_2_tree(Graph* graph) {
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


std::vector<Vertex_ID> Two_Tree_Manipulator::all_triangles_with_edge(
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


std::vector<Vertex_ID> Two_Tree_Manipulator::all_triangles_with_edge(
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

void Two_Tree_Manipulator::_realize_2_tree_order_aux(
    const Graph& graph,
    std::vector<Two_Tree_Manipulator::realization_triplet> &already_ordered,
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

std::vector<Two_Tree_Manipulator::realization_triplet> Two_Tree_Manipulator::realize_2_tree_order(
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


bool Two_Tree_Manipulator::triangulate(
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
std::list<Mapped_Graph_Copy*> Two_Tree_Manipulator::realize_2_tree(
    const Mapped_Graph_Copy& graph,
    std::vector<Two_Tree_Manipulator::realization_triplet> realization_order)
{
    std::list<Mapped_Graph_Copy*> ret;

    // std::cout << "Two_Tree_Manipulator::realize_2_tree: Here 0" << std::endl;
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
    // std::cout << "Two_Tree_Manipulator::realize_2_tree: Here 1" << std::endl;

    // iterate through the rest of the vertices
    for (unsigned int i = 2; i < realization_order.size(); i++) {

        // bool found_a_realization = false;

        // std::cout << "Two_Tree_Manipulator::realize_2_tree: loop1: start, i = " << i << std::endl;
        // iterate through the realizations (going to have to double the size of the list)
        std::list<Mapped_Graph_Copy*>::iterator
            mgc_it = ret.begin(),
            mgc_end = ret.end();
        mgc_end--;
        while (true) {
            // assert(*mgc_it != NULL);

            // std::cout << "Two_Tree_Manipulator::realize_2_tree: loop2: start " << std::endl;
            // vi
            Vertex_ID vi_wkcp = realization_order[i].v;
            Vertex_ID vi_orig = graph.original_vertex(vi_wkcp);
            Vertex_ID vi_newg = (*mgc_it)->copy_vertex(vi_orig);
            // std::cout << "Two_Tree_Manipulator::realize_2_tree: loop2: here 0 " << std::endl;

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
            // std::cout << "Two_Tree_Manipulator::realize_2_tree: loop2: here 1 " << std::endl;

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
                    ret.erase(mgc_it);
                    break;
                } else {
                    std::list<Mapped_Graph_Copy*>::iterator mgc_current = mgc_it;
                    mgc_it++;
                    ret.erase(mgc_current);
                }
            }

            // std::cout << "Two_Tree_Manipulator::realize_2_tree: loop2: end " << std::endl;
        }

        if (ret.size() == 0)
            break;
        // std::cout << "Two_Tree_Manipulator::realize_2_tree: loop1: end" << std::endl;
    }

    return ret;
}
