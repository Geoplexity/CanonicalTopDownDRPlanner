#include "DR_Plan.hpp"

#include "Pebbled_Graph.hpp"

#include <iostream>



void print_depth_first(Graph &g, DRP_Node_v *node, unsigned int tabs = 0);
void print_depth_first(Graph &g, DRP_Node_e *node, unsigned int tabs = 0);






DR_Plan::DR_Plan(const Graph &graph) :
    graph(graph),
    _root_v(Cluster_v()),
    _root_e(Cluster_e())
{
    // make_DR_plan_OV4();
    make_DR_plan();

    // sort the tree (DR-Plan)
    _root_e.sort_children_descending_recursively();

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


void DR_Plan::make_DR_plan() {
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


    // find the vertex based tree, using root_e
    _root_v.load = graph.vertices_incident(_root_e.load);
    DRP_Node_e *child_e = _root_e.first_child();
    while (child_e != NULL) {
        _root_v.add_child(get_DRP_in_terms_of_vertices(child_e));
        child_e = child_e->next();
    }
    // TODO: How do I do the above without unrolling the first level of recursion
    // here? I'd like it to look like:
        // DRP_Node_v *root_v = get_DRP_in_terms_of_vertices(&_root_e);
        // _root_v = *root_v;
        // delete(root_v);
}


void DR_Plan::decompose_isostatic_component(
    DRP_Node_e* node,
    const std::map<Edge_ID, std::set<Cluster_e*> > &M,
    const std::map<Edge_ID, std::map<Edge_ID, Cluster_e*> > &P)
{
    //////
    // BASE CASE
    //////
    if (node->load.size() == 1)
        return;


    //////
    // Step 1: find all clusters (isostatic vertex-maximal proper subgraphs) of
    // the node's cluster
    //////
    std::vector<Cluster_e> clusters_orig;

    Cluster_e edges = node->load;
    while (edges.size() > 0) {
        // select an arbitrary edge e
        Cluster_e::iterator e_in_cluster_it = edges.begin();

        // Find the largest cluster containing edge e, by checking every edge f
        // in node->load such that |P[f][e]| is maximized
        unsigned int max_cardinality = 0;
        Cluster_e *c_max;
        for (Cluster_e::iterator e_not_in_cluster_it = node->load.begin(); e_not_in_cluster_it != node->load.end(); e_not_in_cluster_it++) {
            if (*e_in_cluster_it == *e_not_in_cluster_it)
                continue;

            Cluster_e *c = P.at(*e_not_in_cluster_it).at(*e_in_cluster_it);

            // If the edge count is higher (i.e. vertex count is higher) then
            // it's the new contender for the cluster containing e
            if (c->size() > max_cardinality) {
                max_cardinality = c->size();
                c_max = c;
            }
        }

        // At lower levels of recursion, the largest cluster containing e may
        // contain some edge not in node->load, so we instead take:
        // c_max n node->load
        // This will still be rigid.
        clusters_orig.push_back(Cluster_e());
        std::set_intersection(
            node->load.begin(), node->load.end(),
            c_max->begin(), c_max->end(),
            std::inserter(clusters_orig.back(), clusters_orig.back().begin()));

        // erase all edges in this cluster from the set "edges"
        for (Cluster_e::iterator e_to_delete_it = c_max->begin(); e_to_delete_it != c_max->end(); e_to_delete_it++) {
            edges.erase(*e_to_delete_it);
        }
    }



    //////
    // Step 2: Determine intersection type
    //////
    assert(clusters_orig.size() > 1);

    bool trivially_intersecting_clusters = is_trivial_intersection(clusters_orig[0], clusters_orig[1]);



    //////
    // Step 3a: Handle trivial intersections
    //////
    if (trivially_intersecting_clusters) {
        assert(clusters_orig.size() > 2);

        // Make a new child node for each cluster
        for (std::vector<Cluster_e>::iterator c_it = clusters_orig.begin(); c_it != clusters_orig.end(); c_it++) {
            DRP_Node_e *child = new DRP_Node_e(*c_it);
            node->add_child(child);
            decompose_isostatic_component(child, M, P);
        }
    }


    //////
    // Step 3b: Handle non-trivial intersections
    //////
    else {
        // if it's non-trivial, you will have found exactly 2 clusters_orig. These
        // will be the largest (most edges) and the largest that contains the
        // appendage corresponding to the first
        assert(clusters_orig.size() == 2);

        // Get the first cluster
        Cluster_e *c1 = &clusters_orig[0];

        // We will fill these vectors in, down to the core
        std::vector<Cluster_e> clusters;
        std::vector<Cluster_e> partners;
        std::vector<Cluster_e> appendages;

        // Get the first cluster and partner, i.e. c1
        clusters.push_back(*c1);
        partners.push_back(*c1);

        // Get the first appendage, i.e. r1 = G\c1
        appendages.push_back(Cluster_e());
        std::set_difference(
            node->load.begin(), node->load.end(),
            partners.back().begin(), partners.back().end(),
            std::inserter(appendages.back(), appendages.back().begin()));

        // Get an arbitrary edge in r1 (i.e. not in c1)
        Edge_ID arbitrary_edge_in_appendage = *(appendages.back().begin());

        // Find the rest of the clusters
        Cluster_e edges = partners[0];
        while (edges.size() > 0) {
            // pick the edge that corresponds to the largest cluster containing
            // the arbitrary edge in the appendage
            Cluster_e::iterator edge_in_partner_it;
            Cluster_e *ck;
            unsigned int ck_size = 0;
            for (Cluster_e::iterator e_it = edges.begin(); e_it != edges.end(); e_it++) {
                Cluster_e *potential_ck = P.at(*e_it).at(arbitrary_edge_in_appendage);
                if (potential_ck->size() > ck_size) {
                    ck_size = potential_ck->size();
                    ck = potential_ck;
                    edge_in_partner_it = e_it;
                }
            }

            // determine if it's a cluster
            if (is_trivial_intersection(clusters[0], *ck)) {
                edges.erase(edge_in_partner_it);
            } else {
                // Since we've taken all the largest clusters, if it's the subset
                // of an already chosen cluster, then we are done, we're now
                // trying to break down the core
                bool subset = false;
                for (std::vector<Cluster_e>::iterator c_it = clusters.begin(); c_it != clusters.end(); c_it++) {
                    if (is_subseteq(*ck, *c_it)) {
                        subset = true;
                        break;
                    }
                }
                if (subset) break;

                // add to list of clusters
                clusters.push_back(*ck);

                // reduce the edge set (edges = edges n ck)
                Cluster_e edges_new;
                std::set_intersection(
                    edges.begin(), edges.end(),
                    ck->begin(), ck->end(),
                    std::inserter(edges_new, edges_new.begin()));
                edges = edges_new;
            }
        }

        // Compute the partners and appendages
        for (unsigned int c_idx = 1; c_idx < clusters.size(); c_idx++) {
            // find and add the new partner (partner[i] = partner[i-1] n cluster[i])
            partners.push_back(Cluster_e());
            std::set_intersection(
                partners[c_idx-1].begin(), partners[c_idx-1].end(),
                clusters[c_idx].begin(), clusters[c_idx].end(),
                std::inserter(partners[c_idx], partners[c_idx].begin()));

            // add the appendage (appendage[i] = partner[i-1] \ cluster[i])
            appendages.push_back(Cluster_e());
            std::set_difference(
                partners[c_idx-1].begin(), partners[c_idx-1].end(),
                // node->load.begin(), node->load.end(),
                clusters[c_idx].begin(), clusters[c_idx].end(),
                std::inserter(appendages[c_idx], appendages[c_idx].begin()));

            // test... should be true
            assert(partners[c_idx].size() + appendages[c_idx].size() == partners[c_idx-1].size());
        }


        // make the rest of the DR_Plan
        DRP_Node_e *current_node = node;
        Cluster_e &core_cluster = partners.back();
        for (unsigned int p_idx = 0; p_idx < partners.size(); p_idx++) {
            // add the partner, but don't decompose. We will be explicitly
            // finding the children for all but the core_cluster (partners.back)
            DRP_Node_e *next_partner = new DRP_Node_e(partners[p_idx]);
            current_node->add_child(next_partner);

            // Find all of the clusters in the appendage. Since the appendages
            // are underconstrained, there are more than one. We can't run
            // the pebble game again because that would increase the complexity.
            Cluster_e &current_appendage = appendages[p_idx];
            std::vector<Cluster_e> clusters_of_appendage;
            while (current_appendage.size() != 0) {
                // get an edge from the appendage, e
                Edge_ID edge_from_appendage = *(current_appendage.begin());

                // Find the cluster in the appendage containing that edge. We do
                // this by finding the edge in the core, f, such that |P[f][e]|
                // if minimized.
                Cluster_e *cluster_in_appendage;
                unsigned int cia_size = graph.num_edges();
                for (Cluster_e::iterator e_it = core_cluster.begin(); e_it != core_cluster.end(); e_it++) {
                    Cluster_e *potential_cia = P.at(*e_it).at(edge_from_appendage);
                    if (potential_cia->size() < cia_size) {
                        cia_size = potential_cia->size();
                        cluster_in_appendage = potential_cia;
                    }
                }

                // Add (cluster_in_appendage n node->load). It will still be rigid.
                clusters_of_appendage.push_back(Cluster_e());
                std::set_intersection(
                    node->load.begin(), node->load.end(),
                    cluster_in_appendage->begin(), cluster_in_appendage->end(),
                    std::inserter(clusters_of_appendage.back(), clusters_of_appendage.back().begin()));

                // remove edges in this cluster from appendage to iterate and
                // avoid duplicates
                for (Cluster_e::iterator e_it = clusters_of_appendage.back().begin(); e_it != clusters_of_appendage.back().end(); e_it++) {
                    current_appendage.erase(*e_it);
                }
            }

            // Make DRP_Nodes corresponding to these appendage clusters
            for (std::vector<Cluster_e>::iterator c_it = clusters_of_appendage.begin(); c_it != clusters_of_appendage.end(); c_it++) {
                DRP_Node_e *app_child = new DRP_Node_e(*c_it);
                current_node->add_child(app_child);
                decompose_isostatic_component(app_child, M, P);
            }
            clusters_of_appendage.clear();

            // iterate
            current_node = next_partner;
        }

        // Recursively decompose the core
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
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



void DR_Plan::make_DR_plan_OV4() {
    std::pair<Vertex_Iterator, Vertex_Iterator> vs = graph.vertices();
    for (Vertex_Iterator v_it = vs.first; v_it != vs.second; v_it++)
        _root_v.load.insert(*v_it);
    _rigid = Pebbled_Graph(&graph).pebble_game_2D() == Pebbled_Graph::l;

    _DRP_2D_linear_aux(&_root_v);
}

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
