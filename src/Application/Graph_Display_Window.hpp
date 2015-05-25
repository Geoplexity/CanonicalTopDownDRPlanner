#ifndef GRAPH_DISPLAY_WINDOW_HPP
#define GRAPH_DISPLAY_WINDOW_HPP

#include "../Graph/Graph.hpp"
#include "../Graph/Pebbled_Graph.hpp"

#include "App_Window_2D.hpp"
#include "DRP_Display_Window.hpp"

#include <string>




class Graph_Display_Window : public App_Window_2D {
public:
    bool left_mouse_clicking;
    int mouse_xpos_previous, mouse_ypos_previous;

    DRP_Display_Window *drp_display_window;

    Graph *graph;
    std::vector<gl_obj::pos_vec> vertices, vertices_highlight;
    std::vector<std::string> vertices_names, vertices_highlight_names;
    std::vector<gl_obj::pos_vec> edges;
    Cluster* drp;
    Cluster* current_drp_node;

    // std::set<unsigned int> highlight_vertices;


    Graph_Display_Window(Main_GUI_Manager *mgm, Graph *graph) {
        this->mgm = mgm;

        this->left_mouse_clicking = false;

        this->drp_display_window = NULL;

        this->graph = graph;
        this->drp = NULL;
        this->current_drp_node = NULL;

        update_graph_positions();
    }

    // void init() {

    // }

    void update_graph_positions() {
        vertices.clear();
        vertices_highlight.clear();
        vertices_names.clear();
        vertices_highlight_names.clear();
        for (std::pair<Vertex_Iterator, Vertex_Iterator> vs = graph->vertices();
            vs.first != vs.second;
            vs.first++)
        {
            gl_obj::pos_vec pos((*graph)[*vs.first].x, (*graph)[*vs.first].y);
            if (current_drp_node != NULL && current_drp_node->vertices.find(*vs.first) != current_drp_node->vertices.end()) {
                vertices_highlight.push_back(pos);
                vertices_highlight_names.push_back((*graph)[*vs.first].name);
            } else {
                vertices.push_back(pos);
                vertices_names.push_back((*graph)[*vs.first].name);
            }
        }



        edges.clear();
        for (std::pair<Edge_Iterator, Edge_Iterator> es = graph->edges();
            es.first != es.second;
            es.first++)
        {
            std::pair<Vertex_ID, Vertex_ID> uv = graph->verts_on_edge(*es.first);
            edges.push_back(gl_obj::pos_vec((*graph)[uv.first].x, (*graph)[uv.first].y));
            edges.push_back(gl_obj::pos_vec((*graph)[uv.second].x, (*graph)[uv.second].y));
        }
    }

    void get_drp() {
        Subgraph sg(graph);
        std::pair<Vertex_Iterator, Vertex_Iterator> vs = graph->vertices();
        sg.induce(vs.first, vs.second);

        // sg.remove_vertex(*(g.find_vertex("7")));


        Pebbled_Graph pg(&sg);

        this->drp = pg.DRP_2D();
        this->drp->print_tree(graph);
        // cout << "Height: " << this->drp->height() << endl;
        // cout << "Width: " << this->drp->width() << endl;

        // this->highlight_vertices = this->current_drp_node->vertices;

        // cout << "Num vertices " << g.num_vertices() << endl;
        // myWindow.update_graph_positions();

        this->current_drp_node = drp;

        update_graph_positions();
        update_display();
    }

    void highlight_cluster(Cluster* c) {
        if (c) {
            this->current_drp_node = c;
            update_graph_positions();
            update_display();
        }
    }



    void key_callback(int key, int scancode, int action, int mods) {
        // cout << "Key" << endl;
        if (action == GLFW_PRESS) {
            // cout << "\tPressed" << endl;
            if (key == GLFW_KEY_ESCAPE) {
                this->close_window();
            } else if (key == GLFW_KEY_UP) {
                if (this->current_drp_node != NULL) {
                    highlight_cluster(current_drp_node->parent());
                }
            } else if (key == GLFW_KEY_DOWN) {
                if (this->current_drp_node != NULL) {
                    highlight_cluster(current_drp_node->first_child());
                }
            } else if (key == GLFW_KEY_RIGHT) {
                if (this->current_drp_node != NULL) {
                    highlight_cluster(current_drp_node->next());
                }
            } else if (key == GLFW_KEY_LEFT) {
                if (this->current_drp_node != NULL) {
                    highlight_cluster(current_drp_node->prev());
                }
            } else if (key == GLFW_KEY_SPACE) {
                if (drp_display_window == NULL) {
                    drp_display_window = new DRP_Display_Window(drp);

                    float scale = 3;
                    int width_screen_coords, height_screen_coords;
                    get_window_size_in_screen_coords(&width_screen_coords, &height_screen_coords);

                    mgm->create_window(
                        drp_display_window,
                        width_screen_coords/scale,
                        height_screen_coords/scale,
                        "DR-Plan",
                        3,
                        2);
                    drp_display_window->init_program();

                    drp_display_window->update_graph_positions();
                    // cout << "GDW: Here 0" << endl;
                    drp_display_window->update_display();
                    // cout << "GDW: Here 1" << endl;
                }
            }
        }
    };


    void mouse_button_callback(int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_1) {
            if (action == GLFW_PRESS) {
                this->left_mouse_clicking = true;
                this->get_cursor_position_pixels(&mouse_xpos_previous, &mouse_ypos_previous);
            } else if (action == GLFW_RELEASE) {
                this->left_mouse_clicking = false;
            }
        }
    }

    void cursor_pos_callback(double xpos, double ypos) {
        // if (this->left_mouse_clicking) {
        //     this->get_cursor_position(&xpos, &ypos);
        //     cout << "Position: " << xpos << " " << ypos << endl;

        //     double diff_x = xpos - mouse_xpos_previous;
        //     double diff_y = ypos - mouse_ypos_previous;

        //     cout << "\tChange: "  << diff_x << " " << diff_y << endl;
        //     vh->translate(diff_x, diff_y);

        //     mouse_xpos_previous = xpos;
        //     mouse_ypos_previous = ypos;

        //     // this->program->setUniform_ViewMatrix(vh->get_view_matrix());
        //     update_view_matrix();
        //     update_display();
        // }
        if (this->left_mouse_clicking) {
            int x, y;
            this->get_cursor_position_pixels(&x, &y);
            // cout << x << " " << y << endl;

            int width, height;
            this->get_window_size_in_pixels(&width, &height);

            float dx,dy;
            dx = 2*((float)(mouse_xpos_previous - x))/width;
            dy = 2*((float)(mouse_ypos_previous - y))/height;
            // cout << "\tChange: "  << dx << " " << dy << endl;

            float ar = vh->aspect_ratio();
            if (ar > 1)
                dx *= ar;
            else
                dy /= ar;

            vh->translate(dx, dy);

            mouse_xpos_previous = x;
            mouse_ypos_previous = y;

            update_view_matrix();
            update_display();
        }
    }




    void scroll_callback(double x_offset, double y_offset) {
        // cout << "scroll_callback " << vh->zoom() << " ";
        if (y_offset > 0) {
            //scroll up
            vh->scale_zoom(0.95, 0.01f, 50.f);
        } else {
            //scroll down
            vh->scale_zoom(1/0.95, 0.01f, 50.f);
        }
        // cout << vh->zoom() << endl;
        update_view_matrix();
        update_display();
    }



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
    //     program->draw_easy_font(vg);
    // }


    void update_display() {
        set_as_context();


        // Clip the view port to match our ratio
        int width, height;
        get_window_size_in_pixels(&width, &height);
        glViewport(0, 0, width, height);


        program->clearViewport();
        // do_font_stuff();

        // string printthis = "Hello,\nthis\nis\na\nfont\ntest!";
        // print_string(0,0,printthis.c_str(),0,0,0);
        // cout << "DISPLAY: Number of verts = " << vertices.size() << endl;
        // cout << "DISPLAY: Number of edges = " << edges.size() << endl;
        program->draw_graph_vertices_names(vertices_names, vertices, vertices_highlight_names, vertices_highlight);
        program->draw_graph_edges(edges);
        program->draw_graph_vertices(vertices, vertices_highlight);

        // myProg->draw_colorpicking_scene();
        program->flush();

        swap_buffers();
    }
};


#endif
