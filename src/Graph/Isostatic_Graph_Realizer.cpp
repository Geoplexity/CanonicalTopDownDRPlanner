#include "Isostatic_Graph_Realizer.hpp"

#include "MinPriorityQueue.hpp"

#include <cmath>
#include <iostream>



graph_copy::graph_copy(Graph *g) {
    orig = g;

    Vertex_Iterator v, v_end;
    for (boost::tie(v, v_end) = orig->vertices(); v != v_end; v++) {
        // Vertex_ID newv = copy.add_vertex();
        Vertex_ID newv = copy.add_vertex((*orig)[*v]);
        orig_to_copy[*v] = newv;
        copy_to_orig[newv] = *v;
    }

    // std::cout << "========graph_copy:" <<std::endl;
    // print_copy_to_orig(1);
    // copy.print_vertices(2);

    Edge_Iterator e, e_end;
    for (boost::tie(e, e_end) = orig->edges(); e != e_end; e++) {
        std::pair<Vertex_ID, Vertex_ID> vs = orig->verts_on_edge(*e);
        copy.add_edge(orig_to_copy[vs.first], orig_to_copy[vs.second], (*orig)[*e].length);
    }
}

void graph_copy::print_copy_to_orig(unsigned int indent) {
    for (int i = 0; i < indent; i++) std::cout << "\t";
    std::cout << "copy_to_orig mapping (new, old):" << std::endl;

    for (std::map<Vertex_ID, Vertex_ID>::iterator v = copy_to_orig.begin(); v!= copy_to_orig.end(); v++) {
        for (int i = 0; i < indent+1; i++) std::cout << "\t";
        // std::cout << "(" << v->first << ", " << v->second << ")" << std::endl;
        std::cout << "(" << v->first << ": " << copy[v->first].name << ", " << v->second << ": " << (*orig)[v->second].name << ")" << std::endl;
    }
}



Isostatic_Graph_Realizer::Isostatic_Graph_Realizer(Graph *g) {
    in_graph = g;
}

bool Isostatic_Graph_Realizer::realize() {
    // graph_copy gc(in_graph);
    // bool partial_2_tree = is_partial_2_tree(gc);

    make_partial_2_tree();
    make_2_tree();
    return realize_2_tree();

    // std::cout
    //     << "Partial 2 tree? "
    //     << (partial_2_tree? "true": "false")
    //     << std::endl;
}

bool Isostatic_Graph_Realizer::is_partial_2_tree(graph_copy &gc) {
    MinPriorityQueue<Vertex_ID, unsigned int> mpq;
    Vertex_Iterator v, v_end;
    for (boost::tie(v, v_end) = gc.copy.vertices(); v != v_end; v++) {
        mpq.insert(*v, gc.copy.degree_of_vertex(*v));
    }

    // while (!mpq.empty()) {
    //     std::pair<Vertex_ID, unsigned int> top = mpq.pop_top();
    //     std::cout << "(" << (*gc.copy)[top.first].name << ", " << top.second << ") ";
    // }
    // std::cout << std::endl;

    while (!mpq.empty()) {
        std::pair<Vertex_ID, unsigned int> top = mpq.pop_top();
        // std::cout << "(" << (gc.copy)[top.first].name << ", " << top.second << ")" << std::endl;

        if (top.second == 0) {
            // can simply remove gc.copy
            gc.copy.remove_vertex(top.first);
        } else if (top.second == 1) {
            std::pair<Graph_Adj_Iterator, Graph_Adj_Iterator> v_adj
                = boost::adjacent_vertices(top.first, gc.copy);
            Vertex_ID v = *(v_adj.first);

            // Contract it into the graph.
            this->contractions.push_back(std::make_pair(top.first, v));
            gc.copy.contract_edge(top.first, v);

            // No other degrees change besides v and the one popped
            mpq.change_key(v, gc.copy.degree_of_vertex(v));
        } else if (top.second == 2) {
            std::pair<Graph_Adj_Iterator, Graph_Adj_Iterator> v_adj
                = boost::adjacent_vertices(top.first, gc.copy);

            // make a vector of the adj verts, iterators go bad after contraction
            std::vector<Vertex_ID> v_adj_vec;
            for (Graph_Adj_Iterator v_adj_it = v_adj.first; v_adj_it != v_adj.second; ++v_adj_it) {
                v_adj_vec.push_back(*v_adj_it);
            }

            // contract
            this->contractions.push_back(std::make_pair(top.first, v_adj_vec[0]));
            gc.copy.contract_edge(top.first, v_adj_vec[0]);

            // update degrees
            for (int i = 0; i < v_adj_vec.size(); i++) {
                mpq.change_key(v_adj_vec[i], gc.copy.degree_of_vertex(v_adj_vec[i]));
            }
        } else {
            break;
        }
    }

    if (gc.copy.num_vertices() == 0)
        return true;
    else
        return false;
}




// returns a list of dropped edges
std::vector<std::pair<Vertex_ID, Vertex_ID> > Isostatic_Graph_Realizer::make_partial_2_tree() {
    assert(in_graph->num_vertices() >= 3);

    std::vector<std::pair<Vertex_ID, Vertex_ID> > ret;

    // Copy the graph, check if it's a partial 2 tree (does all the contractions, altering the copy)
    // Checks the leftover edges, if they're in the original graph, they are removed from the original graph
    graph_copy *gc = new graph_copy(in_graph);
    while (!is_partial_2_tree(*gc)) {
        Edge_Iterator e, e_end;
        for (boost::tie(e, e_end) = gc->copy.edges(); e != e_end; e++) {
            std::pair<Vertex_ID, Vertex_ID> vs = gc->copy.verts_on_edge(*e);
            Vertex_ID v0 = gc->copy_to_orig[vs.first];
            Vertex_ID v1 = gc->copy_to_orig[vs.second];

            if (in_graph->has_edge(v0, v1)) {
                ret.push_back(std::make_pair(v0, v1));
                in_graph->remove_edge(v0, v1);
                break;
            }
        }

        // delete and make the new one. Needs to be pointers or else copy
        // constructor is invoked and the graph_copy maps stop working.
        delete(gc);
        gc = new graph_copy(in_graph);
    }
    delete(gc);

    return ret;
}





// returns the first element in the set that is not v
Vertex_ID get_not_self(Vertex_ID v, std::set<Vertex_ID> &set) {
    for (std::set<Vertex_ID>::iterator v_it = set.begin(); v_it != set.end(); v_it++) {
        if (*v_it != v) {
            return *v_it;
        }
    }

    assert(false); // you gave me a set that contained nothing or just v
    return (Vertex_ID) NULL;
}




// returns a list of added edges
std::vector<std::pair<Vertex_ID, Vertex_ID> > Isostatic_Graph_Realizer::make_2_tree() {
    const double default_edge_length = 1.0;

    std::vector<std::pair<Vertex_ID, Vertex_ID> > ret;

    graph_copy gc(in_graph);

    MinPriorityQueue<Vertex_ID, unsigned int> mpq;
    Vertex_Iterator v, v_end;
    for (boost::tie(v, v_end) = gc.copy.vertices(); v != v_end; v++) {
        mpq.insert(*v, gc.copy.degree_of_vertex(*v));
    }

    std::cout << "Out of loop: 1" << std::endl;

    while (mpq.size() != 3) {
        std::cout << "In loop: here 1" << std::endl;
        std::pair<Vertex_ID, unsigned int> top = mpq.top();
        std::cout << "In loop: here 2" << std::endl;

        if (top.second == 0) {
            // choose any other vertex
            // I feel like if the previous parts are done correctly there should be nothing with degree 0....
            // Since we are dealing with rigid subsystems on graphs of size >= 3... making them into partial 2 trees should never make a free vertex
            assert(false); // lazy, should make real errors

            Vertex_Iterator v, v_end;
            for (boost::tie(v, v_end) = gc.copy.vertices(); v != v_end; v++) {
                if (top.first != *v) {
                    // add in a single edge to an arbitrary vertex (that isn't itself)
                    Edge_ID e = gc.copy.add_edge(top.first, *v, default_edge_length);
                    ret.push_back(std::make_pair(top.first, *v));

                    // adjust priority queue
                    mpq.change_key(top.first, 1);
                    mpq.change_key(*v, gc.copy.degree_of_vertex(*v));

                    break;
                }
            }
        } else if (top.second == 1) {
            // choose any vertex adjacent to the vertex it's adjacent to
            // except it could be just an edge floating in space...

            // Shouldn't occur for same reason as 0
            // assert(false); // lazy, should make real errors

            std::cout << "Handling " << gc.copy[top.first].name <<", with degree 1" << std::endl;

            Vertex_ID v_adj = *(gc.copy.vertices_adjacent(top.first).begin());
            Vertex_ID v_adj_adj;

            std::set<Vertex_ID> adj_adj_vs = gc.copy.vertices_adjacent(v_adj);
            v_adj_adj = get_not_self(top.first, adj_adj_vs);

            // add in edge to return list
            ret.push_back(std::make_pair(top.first, v_adj_adj));

            // it's now an ear, remove the vertex from the graph and priority queue
            gc.copy.remove_vertex(top.first);
            mpq.pop();

            // adjust degree of adjacent vertex
            mpq.change_key(v_adj, gc.copy.degree_of_vertex(v_adj));
        } else if (top.second == 2) {
            // if it's part of a trianlge
            // if the adjacent vertices lie on the same edge, it's a triangle

            // By the 2-ear theorem, every triangulation will have >=2 degree 2
            // vertices. So we remove it if it's part of a triangle.
            // Eventually, this will uncover (if not already there) some vertices
            // with degree 2 that are not ears. Then we add in edges.

            std::cout << "Handling " << gc.copy[top.first].name <<", with degree 2" << std::endl;

            std::set<Vertex_ID> adj_vs = gc.copy.vertices_adjacent(top.first);
            std::set<Vertex_ID>::iterator v_it = adj_vs.begin();
            std::cout << "degree 2: here 1" << std::endl;
            Vertex_ID v0 = *v_it;
            v_it++;
            Vertex_ID v1 = *v_it;
            std::cout << "degree 2: here 2" << std::endl;

            if (gc.copy.has_edge(v0, v1)) {
                std::cout << "degree 2: here 3" << std::endl;
                // this vertex is an ear

                // remove the ear, from graph and priority queue
                gc.copy.remove_vertex(top.first);
                mpq.pop();

                // adjust the priority queue degrees
                mpq.change_key(v0, gc.copy.degree_of_vertex(v0));
                mpq.change_key(v1, gc.copy.degree_of_vertex(v1));
                std::cout << "degree 2: here 4" << std::endl;
            } else {
                std::cout << "degree 2: here 5" << std::endl;
                // add the edge between its 2 neighbors
                gc.copy.add_edge(v0, v1, default_edge_length);
                ret.push_back(std::make_pair(v0, v1));

                // it's now an ear, remove it from the graph and priority queue
                mpq.pop();
                gc.copy.remove_vertex(top.first);

                // the other vertices did not change degree, you're done!

                std::cout << "degree 2: here 6" << std::endl;
            }
            std::cout << "degree 2: here 7" << std::endl;
        } else {
            std::cout << "Handling " << gc.copy[top.first].name <<", with degree " << top.second << std::endl;
            break;
        }
    }

    std::cout << "Out of loop: 2" << std::endl;
    for (std::vector<std::pair<Vertex_ID, Vertex_ID> >::iterator es = ret.begin(); es != ret.end(); es++) {
        std::cout << "Out of loop: adding edge: (" << (*in_graph)[es->first].name << ", " << (*in_graph)[es->second].name << ")" << std::endl;
        in_graph->add_edge(gc.copy_to_orig[es->first], gc.copy_to_orig[es->second], default_edge_length);
        std::cout << "Added" << std::endl;
    }
    in_graph->set_layout();
    in_graph->get_graph_in_range(-0.87, 0.87, -0.87, 0.87);
    // gc.copy.print_vertices();
    // in_graph->print_vertices();

    // *in_graph = gc.copy;

    std::cout << "Out of loop: 3" << std::endl;
    return ret;
}



// takes a realized edge and recursively realizes a 2-tree
bool Isostatic_Graph_Realizer::_realize_2_tree_aux(Vertex_ID v0, Vertex_ID v1) {
    // want to use the vertex with lesser degree, less looping later
    if (in_graph->degree_of_vertex(v1) < in_graph->degree_of_vertex(v0)) {
        Vertex_ID temp = v0;
        v0 = v1;
        v1 = temp;
    }

    std::set<Vertex_ID> vs_adj = in_graph->vertices_adjacent(v0);
    for (std::set<Vertex_ID>::iterator v_it = vs_adj.begin(); v_it != vs_adj.end(); v_it++) {
        Vertex_ID v2 = *v_it;

        // if there is a triangle and the other vertex isn't already realized
        if (v0 != v2
            && in_graph->has_edge(v1, v2)
            && this->realized.find(v2) == this->realized.end()) {

            // get vertex positions
            double v0x = (*in_graph)[v0].x, v0y = (*in_graph)[v0].y;
            double v1x = (*in_graph)[v1].x, v1y = (*in_graph)[v1].y;

            // get edge lengths
            double e01 = (*in_graph)[in_graph->edge(v0, v1)].length;
            double e12 = (*in_graph)[in_graph->edge(v1, v2)].length;
            double e02 = (*in_graph)[in_graph->edge(v0, v2)].length;

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
            double v2x = midx + h*(v1y-v0y)/d;
            double v2y = midy - h*(v1x-v0x)/d;

            (*in_graph)[v2].set_position(v2x, v2y);

            this->realized[v2] = true;

            if (!_realize_2_tree_aux(v0, v2)) return false;
            if (!_realize_2_tree_aux(v1, v2)) return false;
        }
    }

    return true;
}


bool Isostatic_Graph_Realizer::realize_2_tree() {
    this->realized.clear();

    // get first vertex
    Vertex_ID v0 = *(in_graph->vertices().first);

    // get second (adjacent) vertex
    std::set<Vertex_ID> vs_adj = in_graph->vertices_adjacent(v0);
    Vertex_ID v1 = get_not_self(v0, vs_adj);

    double e01 = (*in_graph)[in_graph->edge(v0, v1)].length;

    (*in_graph)[v0].set_position(0, 0);
    (*in_graph)[v1].set_position(0, e01);

    this->realized[v0] = true;
    this->realized[v1] = true;

    bool ret = _realize_2_tree_aux(v0, v1);

    in_graph->get_graph_in_range(-0.87, 0.87, -0.87, 0.87);
    in_graph->print_edges();

    return ret;
}
