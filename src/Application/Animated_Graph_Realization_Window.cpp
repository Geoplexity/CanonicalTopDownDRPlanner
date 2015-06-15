#include "Animated_Graph_Realization_Window.hpp"

#include <iostream>

Animated_Graph_Realization_Window::Animated_Graph_Realization_Window(Main_GUI_Manager *mgm, Graph *graph) :
    Movable_App_Window_2D(mgm),
    // original_graph(graph),
    graph(graph),
    display_graph(NULL),
    igr(new Isostatic_Graph_Realizer(graph))
{
    update_graph_positions();
}

Animated_Graph_Realization_Window::~Animated_Graph_Realization_Window() {
    if (igr) delete(igr);
}

bool Animated_Graph_Realization_Window::step() {
    assert(igr);
    if (igr->step()) {
        // std::cout << "Animated_Graph_Realization_Window::step: Here 0" << std::endl;
        igr->sample();
        // std::cout << "Animated_Graph_Realization_Window::step: Here 1" << std::endl;
        update_graph_positions();
        // std::cout << "Animated_Graph_Realization_Window::step: Here 2" << std::endl;
        update_display();
        return true;
    }
    return false;
}

void Animated_Graph_Realization_Window::_update_graph_positions_none() {
    // uses graph, not display_graph

    // vertices
    for (std::pair<Vertex_Iterator, Vertex_Iterator> vs = graph->vertices();
        vs.first != vs.second;
        vs.first++)
    {
        gl_obj::pos_vec pos((*graph)[*vs.first].x, (*graph)[*vs.first].y);
        vertices.push_back(pos);
        vertices_names.push_back((*graph)[*vs.first].name);
    }

    // edges
    for (std::pair<Edge_Iterator, Edge_Iterator> es = graph->edges();
        es.first != es.second;
        es.first++)
    {
        std::pair<Vertex_ID, Vertex_ID> uv = graph->vertices_incident(*es.first);
        edges.push_back(gl_obj::pos_vec((*graph)[uv.first].x, (*graph)[uv.first].y));
        edges.push_back(gl_obj::pos_vec((*graph)[uv.second].x, (*graph)[uv.second].y));
    }
}

// void Animated_Graph_Realization_Window::_update_graph_positions_omd() {
//     assert(igr);

//     std::cout << "_update_graph_positions_omd: Here 0" << std::endl;

//     // display_graph = igr->get_display_graph();

//     //vertices
//     for (std::pair<Vertex_Iterator, Vertex_Iterator> vs = graph->vertices();
//         vs.first != vs.second;
//         vs.first++)
//     {
//     std::cout << "_update_graph_positions_omd: Here 0.1" << std::endl;
//         Vertex_ID v = display_graph->copy_vertex(*(vs.first));
//     std::cout << "_update_graph_positions_omd: Here 0.2" << std::endl;

//         gl_obj::pos_vec pos((*display_graph)[v].x, (*display_graph)[v].y);
//         vertices.push_back(pos);
//     std::cout << "_update_graph_positions_omd: Here 0.3" << std::endl;
//         vertices_names.push_back((*display_graph)[v].name);
//     }
//     std::cout << "_update_graph_positions_omd: Here 1" << std::endl;


//     // regular and dropped edges
//     std::set<Edge_ID> dropped = igr->list_of_dropped();

//     for (std::pair<Edge_Iterator, Edge_Iterator> es = graph->edges();
//         es.first != es.second;
//         es.first++)
//     {
//         std::pair<Vertex_ID, Vertex_ID> v0v1 = graph->vertices_incident(*(es.first));
//         Vertex_ID v0 = display_graph->copy_vertex(v0v1.first);
//         Vertex_ID v1 = display_graph->copy_vertex(v0v1.second);

//         if (dropped.find(*(es.first)) != dropped.end()) {
//             edges_dashed.push_back(gl_obj::pos_vec((*display_graph)[v0].x, (*display_graph)[v0].y));
//             edges_dashed.push_back(gl_obj::pos_vec((*display_graph)[v1].x, (*display_graph)[v1].y));
//         } else {
//             edges.push_back(gl_obj::pos_vec((*display_graph)[v0].x, (*display_graph)[v0].y));
//             edges.push_back(gl_obj::pos_vec((*display_graph)[v1].x, (*display_graph)[v1].y));
//         }
//     }
//     std::cout << "_update_graph_positions_omd: Here 2" << std::endl;


//     // added edges
//     std::vector<std::pair<Vertex_ID, Vertex_ID> > added = igr->list_of_added();

//     for (std::vector<std::pair<Vertex_ID, Vertex_ID> >::iterator es = added.begin();
//         es != added.end();
//         es++)
//     {
//         Vertex_ID v0 = display_graph->copy_vertex(es->first);
//         Vertex_ID v1 = display_graph->copy_vertex(es->second);

//         edges_highlight.push_back(gl_obj::pos_vec((*display_graph)[v0].x,  (*display_graph)[v0].y));
//         edges_highlight.push_back(gl_obj::pos_vec((*display_graph)[v1].x, (*display_graph)[v1].y));
//     }
//     std::cout << "_update_graph_positions_omd: Here 3" << std::endl;
// }


void Animated_Graph_Realization_Window::_update_graph_positions_omd() {
    assert(igr);

    //vertices
    for (std::pair<Vertex_Iterator, Vertex_Iterator> vs = graph->vertices();
        vs.first != vs.second;
        vs.first++)
    {
        Vertex_ID v = *(vs.first);

        gl_obj::pos_vec pos((*graph)[v].x, (*graph)[v].y);
        vertices.push_back(pos);
        vertices_names.push_back((*graph)[v].name);
    }

    // regular and dropped edges
    std::set<Edge_ID> dropped = igr->list_of_dropped();

    for (std::pair<Edge_Iterator, Edge_Iterator> es = graph->edges();
        es.first != es.second;
        es.first++)
    {
        std::pair<Vertex_ID, Vertex_ID> v0v1 = graph->vertices_incident(*(es.first));
        Vertex_ID v0 = v0v1.first;
        Vertex_ID v1 = v0v1.second;

        if (dropped.find(*(es.first)) != dropped.end()) {
            edges_dashed.push_back(gl_obj::pos_vec((*graph)[v0].x, (*graph)[v0].y));
            edges_dashed.push_back(gl_obj::pos_vec((*graph)[v1].x, (*graph)[v1].y));
        } else {
            edges.push_back(gl_obj::pos_vec((*graph)[v0].x, (*graph)[v0].y));
            edges.push_back(gl_obj::pos_vec((*graph)[v1].x, (*graph)[v1].y));
        }
    }

    // added edges
    std::vector<std::pair<Vertex_ID, Vertex_ID> > added = igr->list_of_added();

    for (std::vector<std::pair<Vertex_ID, Vertex_ID> >::iterator es = added.begin();
        es != added.end();
        es++)
    {
        Vertex_ID v0 = es->first;
        Vertex_ID v1 = es->second;

        edges_highlight.push_back(gl_obj::pos_vec((*graph)[v0].x,  (*graph)[v0].y));
        edges_highlight.push_back(gl_obj::pos_vec((*graph)[v1].x, (*graph)[v1].y));
    }
}


void Animated_Graph_Realization_Window::update_graph_positions() {
    vertices.clear();
    vertices_highlight.clear();
    vertices_names.clear();
    vertices_highlight_names.clear();
    edges.clear();
    edges_highlight.clear();
    edges_dashed.clear();
    // non_edges.clear();

    // display_graph = igr->get_display_graph();

    if (!igr)
        _update_graph_positions_none();
    else
        _update_graph_positions_omd();
}

// void Animated_Graph_Realization_Window::key_callback(int key, int scancode, int action, int mods) {
//     // std::cout << "Animated_Graph_Realization_Window::key_callback: begin" << std::endl;
//     if (action == GLFW_PRESS) {
//         // cout << "\tPressed" << endl;
//         if (key == GLFW_KEY_ESCAPE) {
//             this->close_window();
//         } else if (key == GLFW_KEY_UP) {
//             if (current_drp_node && current_drp_node->parent())
//                 highlight_drp_node(current_drp_node->parent());
//         } else if (key == GLFW_KEY_DOWN) {
//             if (current_drp_node && current_drp_node->first_child())
//                 highlight_drp_node(current_drp_node->first_child());
//         } else if (key == GLFW_KEY_RIGHT) {
//             if (this->current_drp_node != NULL) {
//                 DRP_Node *next = current_drp_node->next();
//                 if (!next) next = current_drp_node->first_sibling();
//                 highlight_drp_node(next);
//             }
//         } else if (key == GLFW_KEY_LEFT) {
//             if (this->current_drp_node != NULL) {
//                 DRP_Node *prev = current_drp_node->prev();
//                 if (!prev) prev = current_drp_node->last_sibling();
//                 highlight_drp_node(prev);
//             }
//         } else if (key == GLFW_KEY_SPACE) {
//             if (drp_display_window == NULL) {
//                 // std::cout << "key_callback: Here 0" << std::endl;
//                 drp_display_window = new DRP_Display_Window(mgm, drp);
//                 // std::cout << "key_callback: Here 1" << std::endl;

//                 float scale = 3;
//                 int width_screen_coords, height_screen_coords;
//                 get_window_size_in_screen_coords(&width_screen_coords, &height_screen_coords);
//                 // std::cout << "key_callback: Here 2" << std::endl;

//                 mgm->create_window(
//                     drp_display_window,
//                     width_screen_coords/scale,
//                     height_screen_coords/scale,
//                     "DR-Plan",
//                     3,
//                     2);
//                 drp_display_window->init_program();
//                 // std::cout << "key_callback: Here 3" << std::endl;

//                 if (current_drp_node)
//                     drp_display_window->highlight_drp_node(current_drp_node, current_drp_node->ancestors());
//                 // std::cout << "key_callback: Here 4" << std::endl;

//                 drp_display_window->update_graph_positions();
//                 // std::cout << "key_callback: Here 5" << std::endl;
//                 drp_display_window->update_display();
//                 // std::cout << "key_callback: Here 6" << std::endl;
//             }
//         }
//     }
//     // std::cout << "Animated_Graph_Realization_Window::key_callback: end" << std::endl;
// };


void Animated_Graph_Realization_Window::update_display() {
    set_as_context();

    // Clip the view port to match our ratio
    int width, height;
    get_window_size_in_pixels(&width, &height);
    glViewport(0, 0, width, height);

    program->clearViewport();

    program->draw_graph_edges(edges, edges_highlight, edges_dashed);
    program->draw_graph_vertices(vertices, vertices_highlight);
    program->draw_graph_vertices_names(vertices_names, vertices, vertices_highlight_names, vertices_highlight);

    program->flush();

    swap_buffers();
}
