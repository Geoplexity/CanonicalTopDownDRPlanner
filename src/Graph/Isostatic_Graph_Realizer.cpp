#include "Isostatic_Graph_Realizer.hpp"

#include "MinPriorityQueue.hpp"

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

    Edge_Iterator e, e_end;
    for (boost::tie(e, e_end) = orig->edges(); e != e_end; e++) {
        std::pair<Vertex_ID, Vertex_ID> vs = orig->verts_on_edge(*e);
        copy.add_edge(orig_to_copy[vs.first], orig_to_copy[vs.second], (*orig)[*e].length);
    }
}


Isostatic_Graph_Realizer::Isostatic_Graph_Realizer(Graph *g) {
    in_graph = g;
}

void Isostatic_Graph_Realizer::realize() {
    // graph_copy gc(in_graph);
    // bool partial_2_tree = is_partial_2_tree(gc);

    make_partial_2_tree();

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
        std::cout << "(" << (gc.copy)[top.first].name << ", " << top.second << ")" << std::endl;

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


// std::vector<std::pair<Vertex_ID, Vertex_ID> > Isostatic_Graph_Realizer::make_partial_2_tree() {
//     std::vector<std::pair<Vertex_ID, Vertex_ID> > ret;

//     graph_copy gc(in_graph);
//     bool partial_2_tree = is_partial_2_tree(gc);

//     if (partial_2_tree) {
//         //
//     } else {
//         Edge_Iterator e, e_end;
//         for (boost::tie(e, e_end) = gc.copy.edges(); e != e_end; e++) {
//             std::pair<Vertex_ID, Vertex_ID> vs = gc.copy.verts_on_edge(*e);
//             Vertex_ID v0 = gc.copy_to_orig[vs.first];
//             Vertex_ID v1 = gc.copy_to_orig[vs.second];

//             if (in_graph->has_edge(v0, v1)) {
//                 ret.push_back(std::make_pair(v0, v1));
//                 in_graph->remove_edge(v0, v1);
//             }
//         }
//     }


//     return ret;
// }


std::vector<std::pair<Vertex_ID, Vertex_ID> > Isostatic_Graph_Realizer::make_partial_2_tree() {
    std::vector<std::pair<Vertex_ID, Vertex_ID> > ret;

    graph_copy gc(in_graph);
    bool partial_2_tree = is_partial_2_tree(gc);

    while (!partial_2_tree) {
        std::cout << "here1" << std::endl;
        Edge_Iterator e, e_end;
        for (boost::tie(e, e_end) = gc.copy.edges(); e != e_end; e++) {
            std::cout << "here2" << std::endl;
            std::pair<Vertex_ID, Vertex_ID> vs = gc.copy.verts_on_edge(*e);
            Vertex_ID v0 = gc.copy_to_orig[vs.first];
            Vertex_ID v1 = gc.copy_to_orig[vs.second];

            std::cout << "here3: Checking on " << gc.copy[vs.first].name << "--" << gc.copy[vs.second].name << std::endl;
            std::cout << "here3: Checking on " << v0 << "--" << v1 << std::endl;
            std::cout << "here3: Checking on " << (*in_graph)[v0].name << "--" << (*in_graph)[v1].name << std::endl;
            if (in_graph->has_edge(v0, v1)) {
                std::cout << "here3.1" << std::endl;
                ret.push_back(std::make_pair(v0, v1));
                std::cout << "here3.2" << std::endl;
                in_graph->remove_edge(v0, v1);
                std::cout << "here 3.3: Removed: " << (*in_graph)[v0].name << "--" << (*in_graph)[v1].name << std::endl;
                break;
            }
        }

        gc = graph_copy(in_graph);
        std::cout << "here4" << std::endl;
        partial_2_tree = is_partial_2_tree(gc);
        std::cout << "here5: "
            << "Partial 2 tree? "
            << (partial_2_tree? "true": "false")
            << ". Remaining nodes: "
            << gc.copy.num_vertices()
            << std::endl;

        break;
    }

    *in_graph = gc.copy;
    return ret;
}
