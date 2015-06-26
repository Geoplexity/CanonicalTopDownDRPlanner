#include "Graph_Display_Window.hpp"

#include "../Graph/Pebbled_Graph.hpp"



Graph_Display_Window::Graph_Display_Window(Main_GUI_Manager *mgm, Graph *graph) :
    Movable_App_Window_2D(mgm),
    graph(graph),
    vertex_clicked_index(-1),
    drp_display_window(NULL),
    drp(NULL),
    current_drp_node(NULL)
{
    update_graph_positions();
}

Graph_Display_Window::~Graph_Display_Window() {
    if (drp) delete(drp); // also deletes current_drp_node
}


void Graph_Display_Window::_update_graph_positions_none() {
    //vertices
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

void Graph_Display_Window::_update_graph_positions_drp() {
    assert(current_drp_node);

    for (std::pair<Vertex_Iterator, Vertex_Iterator> vs = graph->vertices();
        vs.first != vs.second;
        vs.first++)
    {
        gl_obj::pos_vec pos((*graph)[*vs.first].x, (*graph)[*vs.first].y);
        if (current_drp_node->load.find(*vs.first) != current_drp_node->load.end()) {
            vertices_highlight.push_back(pos);
            vertices_highlight_names.push_back((*graph)[*vs.first].name);
        } else {
            vertices.push_back(pos);
            vertices_names.push_back((*graph)[*vs.first].name);
        }
    }

    std::set<Edge_ID> highlight_es = this->graph->edges_in_induced_subgraph(current_drp_node->load);

    for (std::pair<Edge_Iterator, Edge_Iterator> es = graph->edges();
        es.first != es.second;
        es.first++)
    {
        std::pair<Vertex_ID, Vertex_ID> uv = graph->vertices_incident(*es.first);
        if (highlight_es.find(*(es.first)) != highlight_es.end()) {
            edges_highlight.push_back(gl_obj::pos_vec((*graph)[uv.first].x, (*graph)[uv.first].y));
            edges_highlight.push_back(gl_obj::pos_vec((*graph)[uv.second].x, (*graph)[uv.second].y));
        } else {
            edges.push_back(gl_obj::pos_vec((*graph)[uv.first].x, (*graph)[uv.first].y));
            edges.push_back(gl_obj::pos_vec((*graph)[uv.second].x, (*graph)[uv.second].y));
        }
    }

}

void Graph_Display_Window::update_graph_positions() {
    vertices.clear();
    vertices_highlight.clear();
    vertices_names.clear();
    vertices_highlight_names.clear();
    edges.clear();
    edges_highlight.clear();
    edges_dashed.clear();
    // non_edges.clear();

    if (drp)
        _update_graph_positions_drp();
    else
        _update_graph_positions_none();
}

void Graph_Display_Window::highlight_drp_node(DRP_Node* node) {
    assert(node);

    this->current_drp_node = node;
    update_graph_positions();
    update_display();

    if (drp_display_window) {
        drp_display_window->highlight_drp_node(node, node->ancestors());
        drp_display_window->update_graph_positions();
        drp_display_window->update_display();
    }
}


void Graph_Display_Window::get_drp() {
    if (drp) delete(drp);

    this->drp = new DR_Plan(*graph);

    // highlight root
    // this->current_drp_node = drp->root();
    highlight_drp_node(drp->root());

    // Graphical output
    update_graph_positions();
    update_display();

    // text output
    std::cout << "Rigid? " << (this->drp->rigid()? "Yes": "No") << std::endl;
    this->drp->print_depth_first(this->drp->root());
}

void Graph_Display_Window::mouse_button_callback(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_1) {
        if (action == GLFW_PRESS) {
            int x, y;
            get_cursor_position_pixels(&x, &y);
            int stencil_buffer_value = drawer->stencil_at_pixel(x, y);

            if (stencil_buffer_value == -1) {
                drag_camera_begin();
            } else {
                vertex_drag_begin(stencil_buffer_value);
            }
        } else if (action == GLFW_RELEASE) {
            if (!vertex_drag()) {
                drag_camera_end();
            } else {
                vertex_drag_end();
            }
        }
    }
}

void Graph_Display_Window::cursor_pos_callback(double xpos, double ypos) {
    if (!vertex_drag()) {
        Movable_App_Window_2D::cursor_pos_callback(xpos, ypos);
    } else {
        update_mouse_pos_world_coords();
        (*graph)[vertex_clicked].x += dx;
        (*graph)[vertex_clicked].y += dy;

        update_graph_positions();
        update_display();
    }
}

bool Graph_Display_Window::vertex_drag() const {
    return vertex_clicked_index != -1;
}

void Graph_Display_Window::vertex_drag_begin(int index) {
    vertex_clicked_index = index;

    int i = 0;
    for (std::pair<Vertex_Iterator, Vertex_Iterator> vs = graph->vertices();
        vs.first != vs.second;
        vs.first++, i++)
    {
        if (i == index) {
            vertex_clicked = *vs.first;
            std::cout << vertex_clicked << ": " << (*graph)[vertex_clicked].name << std::endl;
            break;
        }
    }

    get_cursor_position_pixels(&mouse_xpos_previous, &mouse_ypos_previous);
}

void Graph_Display_Window::vertex_drag_end() {
    vertex_clicked_index = -1;
}


void Graph_Display_Window::key_callback(int key, int scancode, int action, int mods) {
    // std::cout << "Graph_Display_Window::key_callback: begin" << std::endl;
    if (action == GLFW_PRESS) {
        // cout << "\tPressed" << endl;
        if (key == GLFW_KEY_ESCAPE) {
            this->close_window();
        } else if (key == GLFW_KEY_UP) {
            if (current_drp_node && current_drp_node->parent())
                highlight_drp_node(current_drp_node->parent());
        } else if (key == GLFW_KEY_DOWN) {
            if (current_drp_node && current_drp_node->first_child())
                highlight_drp_node(current_drp_node->first_child());
        } else if (key == GLFW_KEY_RIGHT) {
            if (this->current_drp_node != NULL) {
                DRP_Node *next = current_drp_node->next();
                if (!next) next = current_drp_node->first_sibling();
                highlight_drp_node(next);
            }
        } else if (key == GLFW_KEY_LEFT) {
            if (this->current_drp_node != NULL) {
                DRP_Node *prev = current_drp_node->prev();
                if (!prev) prev = current_drp_node->last_sibling();
                highlight_drp_node(prev);
            }
        } else if (key == GLFW_KEY_ENTER) {
            rescale_graph();
        } else if (key == GLFW_KEY_SPACE) {
            recenter_camera();
        } else if (key == GLFW_KEY_Q) {
            if (drp_display_window == NULL) {
                // std::cout << "key_callback: Here 0" << std::endl;
                drp_display_window = new DRP_Display_Window(mgm, drp);
                // std::cout << "key_callback: Here 1" << std::endl;

                float scale = 3;
                int width_screen_coords, height_screen_coords;
                get_window_size_in_screen_coords(&width_screen_coords, &height_screen_coords);
                // std::cout << "key_callback: Here 2" << std::endl;

                mgm->create_window(
                    drp_display_window,
                    width_screen_coords/scale,
                    height_screen_coords/scale,
                    "DR-Plan",
                    3,
                    2);
                drp_display_window->init_drawer();
                // std::cout << "key_callback: Here 3" << std::endl;

                if (current_drp_node)
                    drp_display_window->highlight_drp_node(current_drp_node, current_drp_node->ancestors());
                // std::cout << "key_callback: Here 4" << std::endl;

                drp_display_window->update_graph_positions();
                // std::cout << "key_callback: Here 5" << std::endl;
                drp_display_window->update_display();
                // std::cout << "key_callback: Here 6" << std::endl;
            }
        }
    }
    // std::cout << "Graph_Display_Window::key_callback: end" << std::endl;
};



// void print_string(float x, float y, char *text, float r, float g, float b) {
//     static char buffer[99999]; // ~500 chars
//     int num_quads;
//     num_quads = stb_easy_font_print(x, y, text, NULL, buffer, sizeof(buffer));
//     glColor3f(r,g,b);
//     glEnableClientState(GL_VERTEX_ARRAY);
//     glVertexPointer(2, GL_FLOAT, 16, buffer);
//     glDrawArrays(GL_QUADS, 0, num_quads*4);
//     glDisableClientState(GL_VERTEX_ARRAY);
// }



// void print_string(float x, float y, const char *text, float r, float g, float b) {
//     gl_obj::VertexGroup vg = stb_easy_font_print(text, x, y, gl_obj::color_vec(0.f, 0.f, 1.f, 0.f), 0.03, 0.005);
//     // gl_obj::VertexGroup vg = stb_easy_font_print(text, x, y, gl_obj::color_vec(0.f, 0.f, 0.f, 0.f), 0.1, 0.2);
//     drawer->draw_easy_font(vg);
// }

void Graph_Display_Window::rescale_graph() {
    graph->get_graph_in_range(-0.87, 0.87, -0.87, 0.87);
    update_graph_positions();
    update_display();
}

void Graph_Display_Window::update_display() {
    set_as_context();


    // Clip the view port to match our ratio
    int width, height;
    get_window_size_in_pixels(&width, &height);
    drawer->clear_viewport(width, height);

    // do_font_stuff();

    // string printthis = "Hello,\nthis\nis\na\nfont\ntest!";
    // print_string(0,0,printthis.c_str(),0,0,0);
    // cout << "DISPLAY: Number of verts = " << vertices.size() << endl;
    // cout << "DISPLAY: Number of edges = " << edges.size() << endl;
    drawer->draw_graph_edges(edges, edges_highlight, edges_dashed);
    drawer->draw_graph_vertices(vertices, vertices_highlight);
    drawer->draw_graph_vertices_names(vertices_names, vertices, vertices_highlight_names, vertices_highlight);

    // myProg->draw_colorpicking_scene();
    drawer->flush();

    swap_buffers();
}
