#ifndef DRP_DISPLAY_WINDOW_HPP
#define DRP_DISPLAY_WINDOW_HPP


#include "../Graph/Graph.hpp"
#include "../Graph/Pebbled_Graph.hpp"

#include "App_Window_2D.hpp"


class DRP_Display_Window : public App_Window_2D {
public:
    Cluster *drp;
    std::vector<gl_obj::pos_vec> vertices, vertices_highlight;
    std::vector<gl_obj::pos_vec> edges;

    // unsigned int height;
    // std::vector<unsigned int> width_per_level;

    DRP_Display_Window(Cluster *drp) {
        this->drp = drp;


        // this->graph = graph;
        // this->current_drp_node = NULL;

        update_graph_positions();
        // update_display();
    }

    void update_graph_positions() {
        // erase old
        vertices.clear();
        vertices_highlight.clear();
        edges.clear();


        std::vector<unsigned int> width_per_level = drp->width_per_level();
        unsigned int height = width_per_level.size();

        std::map<Cluster*, gl_obj::pos_vec> node_position;


        std::vector<Cluster*> this_level, next_level;
        this_level.push_back(drp);

        gl_obj::pos_vec parent(0, 1);

        std::vector<unsigned int> position(height);

        for (unsigned int i = 0; i < height; i++) {
            position[i] = 0;
            float y = (height == 1)? 0 : 1 - ((float)i)/(height-1)*2;

            for (std::vector<Cluster*>::iterator c = this_level.begin(); c != this_level.end(); c++) {
                float x = (width_per_level[i] == 1)? 0 : ((float)position[i])/(width_per_level[i]-1)*2 - 1;
                position[i]++;

                gl_obj::pos_vec pos(x*.95, y*.95);
                vertices.push_back(pos);
                node_position[*c] = pos;

                if (i != 0) {
                    edges.push_back(node_position[(*c)->parent()]);
                    edges.push_back(pos);
                }

                for (Cluster *child = (*c)->first_child(); child != NULL; child = child->next()) {
                    next_level.push_back(child);
                }
            }

            this_level = next_level;
            next_level.clear();
        }
    }

    void update_display() {
        set_as_context();

        // cout << "DDW, display: Here 2" << endl;
        // Clip the view port to match our ratio
        int width, height;
        get_window_size_in_pixels(&width, &height);
        // width /= 10.f; height /= 10.f;
        // cout << "DDW, display: Here 3" << endl;
        glViewport(0, 0, width, height);
        // cout << "DDW, display: Here 4" << endl;


        program->clearViewport();
        // do_font_stuff();
        // cout << "DDW, display: Here 5" << endl;

        // cout << "DISPLAY: Number of verts = " << vertices.size() << endl;
        // cout << "DISPLAY: Number of edges = " << edges.size() << endl;
        program->draw_graph_edges(edges);
        program->draw_graph_vertices(vertices, vertices_highlight);
        // cout << "DDW, display: Here 6" << endl;

        // myProg->draw_colorpicking_scene();
        program->flush();
        // cout << "DDW, display: Here 7" << endl;

        swap_buffers();
        // cout << "DDW, display: Here 8" << endl;
    }
};


#endif
