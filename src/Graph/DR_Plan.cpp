#include "DR_Plan.hpp"

#include "Pebbled_Graph.hpp"

#include <iostream>


DR_Plan::DR_Plan(const Graph &graph) : graph(graph), _root(Cluster()) {
    std::pair<Vertex_Iterator, Vertex_Iterator> vs = graph.vertices();
    for (Vertex_Iterator v_it = vs.first; v_it != vs.second; v_it++)
        _root.load.insert(*v_it);

    _rigid = Pebbled_Graph(&graph).pebble_game_2D() == Pebbled_Graph::l;

    _DRP_2D_linear_aux(&_root);

    _root.sort_children_descending_recursively();
}


DRP_Node* DR_Plan::root() {
    return &_root;
}

bool DR_Plan::rigid() {
    return _rigid;
}

void DR_Plan::print_cluster(const Graph &g, Cluster &c) {
    for (Cluster::iterator v_it = c.begin(); v_it != c.end(); v_it++) {
        std::cout << g[*v_it].name << " ";
    }
    std::cout << std::endl;
}


void DR_Plan::print_depth_first(DRP_Node *node, unsigned int tabs) {
    for (int i = 0; i < tabs; i++) std::cout << "\t";
    std::cout << "Node: ";

    print_cluster(graph, node->load);

    DRP_Node *fc = node->first_child();
    DRP_Node *n  = node->next();

    if (fc != NULL) print_depth_first(fc, tabs+1);
    if (n  != NULL) print_depth_first(n,  tabs);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////





DRP_Node* DR_Plan::add_uncomputed_child(DRP_Node *parent, Cluster *child) {
    DRP_Node *new_node = new DRP_Node(*child);
    parent->add_child(new_node);
    return new_node;
}

void DR_Plan::add_and_compute_child(DRP_Node *parent, Cluster *child) {
    DRP_Node *new_node = new DRP_Node(*child);
    _DRP_2D_linear_aux(new_node);
    parent->add_child(new_node);
}




// TODO: this is quite inefficient, the paper says there is a better way
void erase_and_delete_all_subseteqs(Cluster *super, std::set<Cluster*> &sub) {
    std::vector<std::set<Cluster*>::iterator> to_erase;

    for (std::set<Cluster*>::iterator sub_it = sub.begin(); sub_it != sub.end(); sub_it++) {
        bool is_subset = std::includes(
            super->begin(), super->end(),
            (*sub_it)->begin(), (*sub_it)->end());
        if (is_subset) {
            to_erase.push_back(sub_it);
        }
    }
    for (int i = to_erase.size()-1; i >= 0; i--) {
        delete *(to_erase[i]);

        sub.erase(to_erase[i]);
    }
}

// TODO: this is quite inefficient, the paper says there is a better way
void erase_and_delete_all_subseteqs(std::set<Cluster*> &super, std::set<Cluster*> &sub) {
    std::vector<std::set<Cluster*>::iterator> to_erase;

    for (std::set<Cluster*>::iterator sub_it = sub.begin(); sub_it != sub.end(); sub_it++) {
        for (std::set<Cluster*>::iterator super_it = super.begin(); super_it != super.end(); super_it++) {
            bool is_subset = std::includes(
                (*super_it)->begin(), (*super_it)->end(),
                (*sub_it)->begin(), (*sub_it)->end());
            if (is_subset) {
                to_erase.push_back(sub_it);
                break; // don't want to add it twice
            }
        }
    }
    for (int i = to_erase.size()-1; i >= 0; i--) {
        delete *(to_erase[i]);

        sub.erase(to_erase[i]);
    }
}


// get all wellconstrained vertex-maximal subgraphs
std::set<Cluster*> DR_Plan::get_all_wcvmps(Mapped_Graph_Copy *mgc) {
    Pebbled_Graph pg(mgc);

    std::set<Cluster*> current_clusters;

    std::pair<Vertex_Iterator, Vertex_Iterator> vs = mgc->vertices();
    for (Vertex_Iterator v_it = vs.first; v_it != vs.second; v_it++)
    {
        Vertex_ID vid = *v_it;
        std::set<Cluster*> potential_clusters = pg.component_pebble_game_2D(&vid);


        std::vector<std::set<Cluster*>::iterator> to_erase;


        // // remove anything from potential that's a subseteq of known
        // if (known_cluster != NULL)
        //     erase_and_delete_all_subseteqs(known_cluster, potential_clusters);


        // remove anything from potential that's a subseteq of something from current
        // std::cout << "Current > Potential" << std::endl;
        erase_and_delete_all_subseteqs(current_clusters, potential_clusters);


        // remove anything from current that's a strict subset of something from potential
        // strict subset because it happens after doing it the other way arouund
        // It MUST come second. Otherwise you can delete current clusters that exist
        // in other DRP nodes, leading to segfault
        // std::cout << "Potential > Current" << std::endl;
        erase_and_delete_all_subseteqs(potential_clusters, current_clusters);


        // add the remaining new clusts to the all clusts
        for (std::set<Cluster*>::iterator pc_it = potential_clusters.begin(); pc_it != potential_clusters.end(); pc_it++) {
            current_clusters.insert(*pc_it);
        }
    }

    return current_clusters;
}



// it will be optimal
void DR_Plan::_DRP_2D_linear_aux(DRP_Node *node) {
    Mapped_Graph_Copy mgc(&graph, node->load);

    unsigned int num_vertices = mgc.num_vertices();
    assert(num_vertices > 1);

    // trivial subgraph case
    if (num_vertices == 2) {
        // should have 1 edge between the vertices
        assert(mgc.num_edges() == 1);
        return;
    }

    Pebbled_Graph pg(&mgc);
    unsigned int dofs = pg.pebble_game_2D();

    // < implies that the graph is overconstrained
    assert(dofs >= Pebbled_Graph::l);

    std::set<Cluster*> current_clusters = get_all_wcvmps(&mgc);
    // get original Vertex_ID's
    for (std::set<Cluster*>::iterator c_it = current_clusters.begin(); c_it != current_clusters.end(); c_it++) {
        **c_it = mgc.original_vertices(**c_it);
    }


    // the system is underconstrained
    if (dofs > Pebbled_Graph::l) {
        // recursively apply to all clusters
        for (std::set<Cluster*>::iterator c_it = current_clusters.begin(); c_it != current_clusters.end(); c_it++) {
            add_and_compute_child(node, *c_it);
        }

        return;
    }


    // if there are more than one clusters we need to check if the intersection is wc
    // although I suppose there always should be at least 2, as it's vertex-maximal
    if (current_clusters.size() > 1) {
        std::set<Cluster*>::iterator c_it = current_clusters.begin();
        Cluster *c1, *c2;

        c1 = *c_it;
        c_it++;
        c2 = *c_it;

        Cluster intersection;
        std::set_intersection(
            c1->begin(), c1->end(),
            c2->begin(), c2->end(),
            std::inserter(intersection, intersection.begin()));


        // for 2D wellconstrained input, if it intersects on more than 1 node, it's wellconstrained
        // if wellconstrained intersection, we need to do the linear decomposition.
        if (intersection.size() > 1) {

            // fill up d with everything first
            Cluster d = node->load;

            DRP_Node *dummy_original_parent = new DRP_Node(Cluster()); // just going to delete in the end
            DRP_Node *parent = dummy_original_parent;

            // the loop should begin with d, which is C \ R1 \ R2 \ ... up to the current point
            // initially it's just C
            for (std::set<Cluster*>::iterator Ci = current_clusters.begin();
                Ci != current_clusters.end();
                Ci++)
            {
                // find Ri = C \ Ci
                Cluster Ri;
                std::set_difference(
                    d.begin(), d.end(),
                    (*Ci)->begin(), (*Ci)->end(),
                    std::inserter(Ri, Ri.begin()));

                // get the new d
                Cluster new_d;
                std::set_difference(
                    d.begin(), d.end(),
                    Ri.begin(), Ri.end(),
                    std::inserter(new_d, new_d.begin()));
                d = new_d;

                // get the edges between the subgraph and the rest of the graph
                // there won't be any edges between different Ris, so you can use the whole subgraph
                std::set<Edge_ID> edges_between_d_and_Ri = mgc.edges_between(Ri, d);


                // add d
                DRP_Node *next_parent = add_uncomputed_child(parent, &d);

                // add Ri + edges_between_d_and_Ri
                Mapped_Graph_Copy mgc_d_Ri(&graph, Ri);
                mgc_d_Ri.grow_into(d);

                std::set<Cluster*> d_Ri_clusters = get_all_wcvmps(&mgc_d_Ri);
                // get original Vertex_ID's
                for (std::set<Cluster*>::iterator c_it = d_Ri_clusters.begin(); c_it != d_Ri_clusters.end(); c_it++) {
                    Cluster temp = mgc_d_Ri.original_vertices(**c_it);
                    add_and_compute_child(parent, &temp);
                }

                // iterate
                parent = next_parent;
            }

            // recurse on d (the "core")
            _DRP_2D_linear_aux(parent);

            // add the children to the input node and delete the dummy parent
            node->add_children(dummy_original_parent->make_children_set());
            delete dummy_original_parent;
        }
        // if it's trivial intersection... same as when it's underconstrained
        else {
            // Recursively perform on all children
            int i = 0;
            for (std::set<Cluster*>::iterator c_it = current_clusters.begin(); c_it != current_clusters.end(); c_it++) {
                add_and_compute_child(node, *c_it);
            }
        }


        // delete all current_clusters
        c_it = current_clusters.begin();
        for (; c_it != current_clusters.end(); c_it++) {
            delete(*c_it);
        }
    } else {
        assert(false);
    }

    return;
}
