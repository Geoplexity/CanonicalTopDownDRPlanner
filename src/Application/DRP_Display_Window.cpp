#include "DRP_Display_Window.hpp"



DRP_Display_Window::DRP_Display_Window(
    Main_GUI_Manager *mgm,
    DR_Plan *drp) :
        App_Window_2D(mgm),
        drp(drp)
{
    update_graph_positions();
}

void DRP_Display_Window::highlight_drp_node(DRP_Node_v *node, std::set<DRP_Node_v*> ancestors) {
    current_drp_node = node;
    current_and_ancestors = ancestors;
    // current_and_ancestors = current_drp_node->ancestors();
    current_and_ancestors.insert(current_drp_node);
}

void DRP_Display_Window::update_graph_positions() {
    // erase old
    vertices.clear();
    vertices_highlight.clear();
    edges.clear();
    edges_highlight.clear();


    std::vector<unsigned int> width_per_level = drp->root()->width_per_level();
    unsigned int height = width_per_level.size();

    std::map<DRP_Node_v*, gl_obj::pos_vec> node_position;


    std::vector<DRP_Node_v*> this_level, next_level;
    this_level.push_back(drp->root());

    gl_obj::pos_vec parent(0, 1);

    std::vector<unsigned int> position(height);

    for (unsigned int i = 0; i < height; i++) {
        position[i] = 0;
        float y = (height == 1)? 0 : 1 - ((float)i)/(height-1)*2;

        for (std::vector<DRP_Node_v*>::iterator c = this_level.begin(); c != this_level.end(); c++) {
            float x = (width_per_level[i] == 1)? 0 : ((float)position[i])/(width_per_level[i]-1)*2 - 1;
            position[i]++;

            gl_obj::pos_vec pos(x*.95, y*.95);
            if (current_and_ancestors.find(*c) != current_and_ancestors.end()) {
                vertices_highlight.push_back(pos);
            } else {
                vertices.push_back(pos);
            }
            node_position[*c] = pos;

            if (i != 0) {
                if (current_and_ancestors.find(*c) != current_and_ancestors.end() && current_and_ancestors.find((*c)->parent()) != current_and_ancestors.end()) {
                    edges_highlight.push_back(node_position[(*c)->parent()]);
                    edges_highlight.push_back(pos);
                } else {
                    edges.push_back(node_position[(*c)->parent()]);
                    edges.push_back(pos);
                }
            }

            for (DRP_Node_v *child = (*c)->first_child(); child != NULL; child = child->next()) {
                next_level.push_back(child);
            }
        }

        this_level = next_level;
        next_level.clear();
    }
}

void DRP_Display_Window::update_display() {
    set_as_context();

    // cout << "DDW, display: Here 2" << endl;
    // Clip the view port to match our ratio
    int width, height;
    get_window_size_in_pixels(&width, &height);
    drawer->clear_viewport(width, height);

    // do_font_stuff();
    // cout << "DDW, display: Here 5" << endl;

    // cout << "DISPLAY: Number of verts = " << vertices.size() << endl;
    // cout << "DISPLAY: Number of edges = " << edges.size() << endl;
    drawer->draw_graph_edges(edges, edges_highlight, std::vector<gl_obj::pos_vec>());
    drawer->draw_graph_vertices(vertices, vertices_highlight);
    // cout << "DDW, display: Here 6" << endl;

    // myProg->draw_colorpicking_scene();
    drawer->flush();
    // cout << "DDW, display: Here 7" << endl;

    swap_buffers();
    // cout << "DDW, display: Here 8" << endl;
}
