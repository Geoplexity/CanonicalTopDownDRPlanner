#include "DR_Plan.hpp"

#include "Pebbled_Graph.hpp"

#include <iostream>

// DR_Plan::DR_Plan(const Graph &graph) :
//     graph(graph),
//     _root_v(Cluster_v()),
//     _root_e(Cluster_e())
// {
//     std::pair<Vertex_Iterator, Vertex_Iterator> vs = graph.vertices();
//     for (Vertex_Iterator v_it = vs.first; v_it != vs.second; v_it++)
//         _root_v.load.insert(*v_it);
//     _rigid = Pebbled_Graph(&graph).pebble_game_2D() == Pebbled_Graph::l;

//     _DRP_2D_linear_aux(&_root_v);

//     _root_v.sort_children_descending_recursively();
// }





DR_Plan::DR_Plan(const Graph &graph) :
    graph(graph),
    _root_v(Cluster_v()),
    _root_e(Cluster_e())
{
    std::pair<Edge_Iterator, Edge_Iterator> es = graph.edges();

    // Generate the root node (all the edges)
    for (Edge_Iterator e_it = es.first; e_it != es.second; e_it++)
        this->_root_e.load.insert(*e_it);


    // Find the components of every subgraph (graph minus one edge)
    Cluster_e all_edges;
    for (Edge_Iterator e_it = es.first; e_it != es.second; e_it++) {
        all_edges.insert(*e_it);
    }

    std::map<Edge_ID, std::set<Cluster_e*> > M;
    std::map<Edge_ID, std::map<Edge_ID, Cluster_e*> > P;
    for (Edge_Iterator e_it = es.first; e_it != es.second; e_it++) {
        // remove this edge
        all_edges.erase(*e_it);

        // make a copy
        Mapped_Graph_Copy mgc(&graph, all_edges);

        // get the components
        Pebbled_Graph mgc_pebbled(&mgc);
        std::set<Cluster_v*> mgc_components = mgc_pebbled.component_pebble_game_2D();

        // Get components in terms of the input graph edges
        M[*e_it] = std::set<Cluster_e*>();
        for (std::set<Cluster_v*>::iterator c_it = mgc_components.begin(); c_it != mgc_components.end(); c_it++) {
            Cluster_v c_v = mgc.original_vertices(**c_it);
            Cluster_e *c_e = new Cluster_e();
            *c_e = graph.edges_induced(c_v);
            M[*e_it].insert(c_e);
            delete(*c_it);
        }

        // construct P[*e_it]
        // for (Edge_Iterator e_it2 = es.first; e_it2 != es.second; e_it2++) {
        //     if (e_it == e_it2)
        //         continue;

        //     for (std::set<Cluster_e*>::iterator c_it = M[*e_it].begin(); c_it != M[*e_it].end(); c_it++) {
        //         if ((*c_it)->find(*e_it) != (*c_it)->end()) {
        //             P[*e_it][*e_it2] = *c_it;
        //         }
        //     }
        // }
        for (std::set<Cluster_e*>::iterator c_it = M[*e_it].begin(); c_it != M[*e_it].end(); c_it++) {
            for (Cluster_e::iterator e_it2 = (*c_it)->begin(); e_it2 != (*c_it)->end(); e_it2++) {
                P[*e_it][*e_it2] = *c_it;
            }
        }

        // add this edge back in
        all_edges.insert(*e_it);
    }


    // Determine if the input is isostatic
    // this->_rigid = Pebbled_Graph(&graph).pebble_game_2D() == Pebbled_Graph::l;
    Pebbled_Graph graph_pebbled(&graph);
    std::set<Cluster_v*> graph_components = graph_pebbled.component_pebble_game_2D();
    if (graph_components.size() == 1) {
        // the graph is rigid
        this->_rigid = true;

        decompose_isostatic_component(&this->_root_e, M, P);
    } else {
        // the graph is underconstrained, set each component as a child
        this->_rigid = false;

        for (std::set<Cluster_v*>::iterator c_it = graph_components.begin(); c_it != graph_components.end(); c_it++) {
            // DRP_Node_v *child = new DRP_Node_v(**c_it);
            // this->_root_e.add_child(child);
            // decompose_isostatic_component(child, M, P);
            DRP_Node_e *child = new DRP_Node_e(graph.edges_induced(**c_it));
            this->_root_e.add_child(child);
            decompose_isostatic_component(child, M, P);
        }
    }


    // Clear out M (and consequently everything to which P points)
    for (std::map<Edge_ID, std::set<Cluster_e*> >::iterator m_it = M.begin(); m_it != M.end(); m_it++)
        for (std::set<Cluster_e*>::iterator c_it = m_it->second.begin(); c_it != m_it->second.end(); c_it++)
            delete(*c_it);


    // sort the tree (DR-Plan)
    _root_e.sort_children_descending_recursively();


    // find the vertex based tree, using root_e
    _root_v.load = graph.vertices_incident(_root_e.load);
    DRP_Node_e *child_e = _root_e.first_child();
    while (child_e != NULL) {
        _root_v.add_child(get_DRP_in_terms_of_vertices(child_e));
        child_e = child_e->next();
    }

    // TODO: How do I do this without unrolling the first level of recursion
    // here? I'd like it to look like:
        // DRP_Node_v *root_v = get_DRP_in_terms_of_vertices(&_root_e);
        // _root_v = *root_v;
        // delete(root_v);

    // sort the tree (DR-Plan)
    _root_v.sort_children_descending_recursively();
}

// DR_Plan::~DR_Plan() {
// }


DRP_Node_v* DR_Plan::root() {
    return &_root_v;
}

bool DR_Plan::rigid() const {
    return _rigid;
}

void DR_Plan::print_cluster(const Graph &g, Cluster_v &c) {
    for (Cluster_v::iterator v_it = c.begin(); v_it != c.end(); v_it++) {
        std::cout << g[*v_it].name << " ";
    }
    std::cout << std::endl;
}

void DR_Plan::print_cluster(const Graph &g, Cluster_e &c) {
    for (Cluster_e::iterator e_it = c.begin(); e_it != c.end(); e_it++) {
        std::pair<Vertex_ID, Vertex_ID> vs = g.vertices_incident(*e_it);

        std::cout << "(" << g[vs.first].name << ", " << g[vs.second].name << ") ";
    }
    std::cout << std::endl;
}

void DR_Plan::print_depth_first(DRP_Node_v *node, unsigned int tabs) const {
    for (int i = 0; i < tabs; i++) std::cout << "\t";
    std::cout << "Node: ";

    print_cluster(graph, node->load);

    DRP_Node_v *fc = node->first_child();
    DRP_Node_v *n  = node->next();

    if (fc != NULL) print_depth_first(fc, tabs+1);
    if (n  != NULL) print_depth_first(n,  tabs);
}

void DR_Plan::print_depth_first(DRP_Node_e *node, unsigned int tabs) const {
    for (int i = 0; i < tabs; i++) std::cout << "\t";
    std::cout << "Node: ";

    print_cluster(graph, node->load);

    DRP_Node_e *fc = node->first_child();
    DRP_Node_e *n  = node->next();

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

void DR_Plan::decompose_isostatic_component(
    DRP_Node_e* node,
    const std::map<Edge_ID, std::set<Cluster_e*> > &M,
    const std::map<Edge_ID, std::map<Edge_ID, Cluster_e*> > &P)
{
    std::cout << "decompose_isostatic_component: 0" << std::endl;
    std::cout << "Edges: " << node->load.size() << " of " <<  graph.num_edges() << std::endl;
    print_cluster(graph, node->load);

    //////
    // BASE CASE
    //////
    if (node->load.size() == 1)
        return;


    //////
    // Step 1: find all clusters (isostatic vertex-maximal proper subgraphs) of
    // the node's cluster
    //////
    std::cout << "decompose_isostatic_component: 1" << std::endl;
    std::set<Cluster_e*> clusters;
    // for (Cluster_e::iterator e_it = edges.begin(); e_it != edges.end(); e_it++) {
    //     unsigned int max_cardinality = 0;
    //     Cluster_e *c_max;
    //     for (Cluster_e::iterator e_it2 = edges.begin(); e_it2 != edges.end(); e_it2++) {
    //         Cluster_e *c = P.at(*e_it2).at(*e_it);
    //         // Cluster_v *c = P[*e_it2][*e_it];

    //         // if the edge count is higher.... which means vertex count is higher....
    //         if (c->size() > max_cardinality) {
    //             max_cardinality = c->size();
    //             c_max = c;
    //         }
    //     }
    //     clusters.insert(c_max);

    //     ////////
    //     // TODO: Check for duplicates
    //     ////////
    // }
    Cluster_e edges = node->load;
    while (edges.size() > 0) {
        Cluster_e::iterator e_it = edges.begin();
        // Edge_ID e = *e_it;
        // edges.erase(e_it);

        // find the largest cluster containing edge e
        unsigned int max_cardinality = 0;
        Cluster_e *c_max;
        for (Cluster_e::iterator e_it2 = node->load.begin(); e_it2 != node->load.end(); e_it2++) {
            if (*e_it == *e_it2)
                continue;

            Cluster_e *c = P.at(*e_it2).at(*e_it);
            // Cluster_v *c = P[*e_it2][*e_it];

            // if the edge count is higher.... which means vertex count is higher....
            if (c->size() > max_cardinality) {
                max_cardinality = c->size();
                c_max = c;
            }
        }
        clusters.insert(c_max);

        // erase all edges in this cluster from the set
        for (Cluster_e::iterator e_it2 = c_max->begin(); e_it2 != c_max->end(); e_it2++) {
            edges.erase(*e_it2);
        }
    }



    //////
    // Step 2: Determine intersection type
    //////
    std::cout << "decompose_isostatic_component: 2" << std::endl;
    std::cout << "clusters.size() = " << clusters.size() << std::endl;
    assert(clusters.size() > 1);

    bool trivially_intersecting_clusters;

    {
        std::set<Cluster_e*>::iterator c_it = clusters.begin();
        Cluster_e *c1, *c2;
        c1 = *(c_it++);
        c2 = *(c_it);

        trivially_intersecting_clusters = is_trivial_intersection(*c1, *c2);
    }



    //////
    // Step 3a: Handle trivial intersections
    //////
    std::cout << "decompose_isostatic_component: 3" << std::endl;
    if (trivially_intersecting_clusters) {
        std::cout << "decompose_isostatic_component: 3a: trivial" << std::endl;
        assert(clusters.size() > 2);

        for (std::set<Cluster_e*>::iterator c_it = clusters.begin(); c_it != clusters.end(); c_it++) {
            DRP_Node_e *child = new DRP_Node_e(**c_it);
            node->add_child(child);
            decompose_isostatic_component(child, M, P);
        }
    }


    //////
    // Step 3b: Handle non-trivial intersections
    //////
    else {
        std::cout << "decompose_isostatic_component: 3b: nontrivial" << std::endl;
        assert(clusters.size() == 2);

        // Get the two clusters
        Cluster_e *c1, *c2;
        {
            std::set<Cluster_e*>::iterator c_it = clusters.begin();
            c1 = *(c_it++);
            c2 = *(c_it);
        }

        //
        std::cout << "First cluster (" << c1->size() << "): ";
        print_cluster(graph, *c1);
        std::cout << "Second cluster (" << c2->size() << "): ";
        print_cluster(graph, *c2);

        //
        std::vector<Cluster_e> partners;
        std::vector<Cluster_e> appendages;

        // Get the first partner, i.e. c1
        partners.push_back(*c1);

        // Get the first appendage, i.e. r1 = G\c1
        appendages.push_back(Cluster_e());
        std::set_difference(
            node->load.begin(), node->load.end(),
            partners.back().begin(), partners.back().end(),
            std::inserter(appendages.back(), appendages.back().begin()));

        // Get an arbitrary edge in r1 (i.e. not in c1)
        Edge_ID arbitrary_edge_in_appendage = *(appendages.back().begin());

        //
        std::cout << "First partner (" << partners.back().size() << "): ";
        print_cluster(graph, partners.back());
        std::cout << "First appendage (" << appendages.back().size() << "): ";
        print_cluster(graph, appendages.back());
        // assert(false);

        // Find the remaining partners and appendages
        Cluster_e edges = partners.back();
        while (edges.size() > 0) {
            std::cout << "Edges before (" << edges.size() << "): "; print_cluster(graph, edges);

            // find a new rigid component
            Edge_ID edge_in_partner = *(edges.begin());
            Cluster_e *ck = P.at(edge_in_partner).at(arbitrary_edge_in_appendage);

            // determine if it's a cluster
            if (is_trivial_intersection(*c1, *ck)) {
                // the edge is in the core
                edges.erase(edges.begin());
                // edges.erase(edge_in_partner);
            } else {
                std::cout << "Nontrivial - ck (" << ck->size() << "): "; print_cluster(graph, *ck);

                // reduce the edge set
                Cluster_e edges_new;
                std::set_intersection(
                    edges.begin(), edges.end(),
                    ck->begin(), ck->end(),
                    std::inserter(edges_new, edges_new.begin()));
                edges = edges_new;

                // find and add the new partner
                // Cluster_e &current_partner = partners.back();
                partners.push_back(Cluster_e());
                Cluster_e &current_partner = partners[partners.size()-2];
                std::cout << "Nontrivial - cp (" << current_partner.size() << "): "; print_cluster(graph, current_partner);
                std::set_intersection(
                    current_partner.begin(), current_partner.end(),
                    ck->begin(), ck->end(),
                    std::inserter(partners.back(), partners.back().begin()));
                // partners.push_back(edges);

                // add the appendage
                appendages.push_back(Cluster_e());
                std::set_difference(
                    node->load.begin(), node->load.end(),
                    ck->begin(), ck->end(),
                    std::inserter(appendages.back(), appendages.back().begin()));


                // // need to check that c isn't a subgraph of anything else, and that
                // // anything else isn't a subgraph of it. This ensures that the
                // // resulting plan is sequential.


                // // This means it didn't come from the core (assuming there are
                // // none of the strange events at the bottom)

                // ////////
                // // TODO: Check if it's a duplicate
                // //
                // // could compare to each thing already in the set
                // //
                // // or, could simplify P by deleting repeat clusters in the same
                // // column and using the same pointer. That is, if P[i][j] and
                // // P[k][j] are the same, set P[k][j]= P[i][j]
                // ////////
                // clusters.insert(ck);


                // // delete all the edges from the set....

            }

            std::cout << "Edges after (" << edges.size() << "): "; print_cluster(graph, edges);
        }

        for (unsigned int i = 0; i < partners.size(); i++) {
            std::cout << "Partner "<< i << " (" << partners[i].size() << "): ";
            print_cluster(graph, partners[i]);
            std::cout << "Appendage "<< i << " (" << appendages[i].size() << "): ";
            print_cluster(graph, appendages[i]);
        }
        // assert(false);

        // make the rest of the DR_Plan
        DRP_Node_e *current_node = node;
        Edge_ID arbitrary_edge_from_core = *(partners[partners.size()-1].begin());
        for (unsigned int i = 0; i < partners.size(); i++) {
            Cluster_e &current_appendage = appendages[i];

            // add the partner
            DRP_Node_e *next_partner = new DRP_Node_e(partners[i]);
            current_node->add_child(next_partner);

            // Find all the clusters in the appendage. This is done by getting
            // P[i][j] where i is an arbitrary edge in the core, for all j in
            // the appendage, deleting duplicates.
            std::set<Cluster_e*> clusters_of_appendage;
            while (current_appendage.size() != 0) {
                Edge_ID edge_from_appendage = *(current_appendage.begin());
                Cluster_e *cluster_in_appendage = P.at(arbitrary_edge_from_core).at(edge_from_appendage);
                clusters_of_appendage.insert(cluster_in_appendage);

                // remove edges in this cluster from appendage to avoid duplicates
                for (Cluster_e::iterator e_it = cluster_in_appendage->begin(); e_it != cluster_in_appendage->end(); e_it++) {
                    current_appendage.erase(*e_it);
                }
            }

            // Make DRP_Nodes corresponding to these appendage clusters
            for (std::set<Cluster_e*>::iterator c_it = clusters_of_appendage.begin(); c_it != clusters_of_appendage.end(); c_it++) {
                DRP_Node_e *app_child = new DRP_Node_e(**c_it);
                current_node->add_child(app_child);
                decompose_isostatic_component(app_child, M, P);
            }

            // iterate
            current_node = next_partner;
        }

        // Decompose the core
        decompose_isostatic_component(current_node, M, P);
    }
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

DRP_Node_v* DR_Plan::get_DRP_in_terms_of_vertices(const DRP_Node_e *root) {
    DRP_Node_v *root_v = new DRP_Node_v(graph.vertices_incident(root->load));
    DRP_Node_e *child_e = root->first_child();
    while (child_e != NULL) {
        root_v->add_child(get_DRP_in_terms_of_vertices(child_e));
        child_e = child_e->next();
    }
    return root_v;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool DR_Plan::is_trivial_intersection(const Cluster_e &c1, const Cluster_e &c2) {
    if (intersection_size(c1, c2) == 0)
        return true;
    return false;
}

unsigned int DR_Plan::intersection_size(const Cluster_e &c1, const Cluster_e &c2) {
    Cluster_e intersection;
    std::set_intersection(
        c1.begin(), c1.end(),
        c2.begin(), c2.end(),
        std::inserter(intersection, intersection.begin()));

    return intersection.size();
}

bool DR_Plan::is_subseteq(const Cluster_e &c1, const Cluster_e &c2) {
    if (c1.size() > c2.size())
        return false;

    for (Cluster_e::iterator e = c1.begin(); e != c1.end(); e++)
        if (c2.find(*e) == c2.end())
            return false;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////





DRP_Node_v* DR_Plan::add_uncomputed_child(DRP_Node_v *parent, Cluster_v *child) {
    DRP_Node_v *new_node = new DRP_Node_v(*child);
    parent->add_child(new_node);
    return new_node;
}

void DR_Plan::add_and_compute_child(DRP_Node_v *parent, Cluster_v *child) {
    DRP_Node_v *new_node = new DRP_Node_v(*child);
    _DRP_2D_linear_aux(new_node);
    parent->add_child(new_node);
}




// TODO: this is quite inefficient, the paper says there is a better way
void erase_and_delete_all_subseteqs(Cluster_v *super, std::set<Cluster_v*> &sub) {
    std::vector<std::set<Cluster_v*>::iterator> to_erase;

    for (std::set<Cluster_v*>::iterator sub_it = sub.begin(); sub_it != sub.end(); sub_it++) {
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
void erase_and_delete_all_subseteqs(std::set<Cluster_v*> &super, std::set<Cluster_v*> &sub) {
    std::vector<std::set<Cluster_v*>::iterator> to_erase;

    for (std::set<Cluster_v*>::iterator sub_it = sub.begin(); sub_it != sub.end(); sub_it++) {
        for (std::set<Cluster_v*>::iterator super_it = super.begin(); super_it != super.end(); super_it++) {
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
std::set<Cluster_v*> DR_Plan::get_all_wcvmps(Mapped_Graph_Copy *mgc) {
    Pebbled_Graph pg(mgc);

    std::set<Cluster_v*> current_clusters;

    std::pair<Vertex_Iterator, Vertex_Iterator> vs = mgc->vertices();
    for (Vertex_Iterator v_it = vs.first; v_it != vs.second; v_it++)
    {
        Vertex_ID vid = *v_it;
        std::set<Cluster_v*> potential_clusters = pg.component_pebble_game_2D(&vid);


        std::vector<std::set<Cluster_v*>::iterator> to_erase;


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
        for (std::set<Cluster_v*>::iterator pc_it = potential_clusters.begin(); pc_it != potential_clusters.end(); pc_it++) {
            current_clusters.insert(*pc_it);
        }
    }

    return current_clusters;
}



// it will be optimal
void DR_Plan::_DRP_2D_linear_aux(DRP_Node_v *node) {
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

    std::set<Cluster_v*> current_clusters = get_all_wcvmps(&mgc);
    // get original Vertex_ID's
    for (std::set<Cluster_v*>::iterator c_it = current_clusters.begin(); c_it != current_clusters.end(); c_it++) {
        **c_it = mgc.original_vertices(**c_it);
    }


    // the system is underconstrained
    if (dofs > Pebbled_Graph::l) {
        // recursively apply to all clusters
        for (std::set<Cluster_v*>::iterator c_it = current_clusters.begin(); c_it != current_clusters.end(); c_it++) {
            add_and_compute_child(node, *c_it);
        }

        return;
    }


    // if there are more than one clusters we need to check if the intersection is wc
    // although I suppose there always should be at least 2, as it's vertex-maximal
    if (current_clusters.size() > 1) {
        std::set<Cluster_v*>::iterator c_it = current_clusters.begin();
        Cluster_v *c1, *c2;

        c1 = *c_it;
        c_it++;
        c2 = *c_it;

        Cluster_v intersection;
        std::set_intersection(
            c1->begin(), c1->end(),
            c2->begin(), c2->end(),
            std::inserter(intersection, intersection.begin()));


        // for 2D wellconstrained input, if it intersects on more than 1 node, it's wellconstrained
        // if wellconstrained intersection, we need to do the linear decomposition.
        if (intersection.size() > 1) {

            // fill up d with everything first
            Cluster_v d = node->load;

            DRP_Node_v *dummy_original_parent = new DRP_Node_v(Cluster_v()); // just going to delete in the end
            DRP_Node_v *parent = dummy_original_parent;

            // the loop should begin with d, which is C \ R1 \ R2 \ ... up to the current point
            // initially it's just C
            for (std::set<Cluster_v*>::iterator Ci = current_clusters.begin();
                Ci != current_clusters.end();
                Ci++)
            {
                // find Ri = C \ Ci
                Cluster_v Ri;
                std::set_difference(
                    d.begin(), d.end(),
                    (*Ci)->begin(), (*Ci)->end(),
                    std::inserter(Ri, Ri.begin()));

                // get the new d
                Cluster_v new_d;
                std::set_difference(
                    d.begin(), d.end(),
                    Ri.begin(), Ri.end(),
                    std::inserter(new_d, new_d.begin()));
                d = new_d;

                // get the edges between the subgraph and the rest of the graph
                // there won't be any edges between different Ris, so you can use the whole subgraph
                Cluster_e edges_between_d_and_Ri = mgc.edges_between(Ri, d);


                // add d
                DRP_Node_v *next_parent = add_uncomputed_child(parent, &d);

                // add Ri + edges_between_d_and_Ri
                Mapped_Graph_Copy mgc_d_Ri(&graph, Ri);
                mgc_d_Ri.grow_into(d);

                std::set<Cluster_v*> d_Ri_clusters = get_all_wcvmps(&mgc_d_Ri);
                // get original Vertex_ID's
                for (std::set<Cluster_v*>::iterator c_it = d_Ri_clusters.begin(); c_it != d_Ri_clusters.end(); c_it++) {
                    Cluster_v temp = mgc_d_Ri.original_vertices(**c_it);
                    add_and_compute_child(parent, &temp);
                }

                // iterate
                parent = next_parent;
            }

            // recurse on d (the "core")
            _DRP_2D_linear_aux(parent);

            // add the children to the input node and delete the dummy parent
            node->move_children_from(dummy_original_parent);
            delete dummy_original_parent;
            // node->add_children(dummy_original_parent->children());
        }
        // if it's trivial intersection... same as when it's underconstrained
        else {
            // Recursively perform on all children
            int i = 0;
            for (std::set<Cluster_v*>::iterator c_it = current_clusters.begin(); c_it != current_clusters.end(); c_it++) {
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
