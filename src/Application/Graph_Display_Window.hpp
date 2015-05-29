#ifndef GRAPH_DISPLAY_WINDOW_HPP
#define GRAPH_DISPLAY_WINDOW_HPP

#include "../Graph/Graph.hpp"
#include "../Graph/DR_Plan.hpp"

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
    std::vector<gl_obj::pos_vec> edges, edges_highlight;
    DR_Plan* drp;
    DRP_Node* current_drp_node;

    // std::set<unsigned int> highlight_vertices;


    Graph_Display_Window(Main_GUI_Manager *mgm, Graph *graph);

    // void init();

    void update_graph_positions();

    void get_drp();

    void highlight_drp_node(DRP_Node* node);



    void key_callback(int key, int scancode, int action, int mods);


    void mouse_button_callback(int button, int action, int mods);

    void cursor_pos_callback(double xpos, double ypos);




    void scroll_callback(double x_offset, double y_offset);



    // void print_string(float x, float y, char *text, float r, float g, float b);



    // void print_string(float x, float y, const char *text, float r, float g, float b);


    void update_display();
};


#endif
