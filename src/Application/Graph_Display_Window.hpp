#ifndef GRAPH_DISPLAY_WINDOW_HPP
#define GRAPH_DISPLAY_WINDOW_HPP

#include "../Graph/Graph.hpp"
#include "../Graph/DR_Plan.hpp"
#include "../Graph/Isostatic_Graph_Realizer.hpp"

#include "App_Window_2D.hpp"
#include "DRP_Display_Window.hpp"

#include <string>




class Graph_Display_Window : public Movable_App_Window_2D {
public:
    Graph_Display_Window(Main_GUI_Manager *mgm, Graph *graph);
    ~Graph_Display_Window();

    void update_graph_positions();
    void highlight_drp_node(DRP_Node* node);

    void get_drp();

    void key_callback(int key, int scancode, int action, int mods);

    // void print_string(float x, float y, char *text, float r, float g, float b);
    // void print_string(float x, float y, const char *text, float r, float g, float b);

    void update_display();
private:
    Graph *graph;

    std::vector<gl_obj::pos_vec>
        vertices,
        vertices_highlight;

    std::vector<std::string>
        vertices_names,
        vertices_highlight_names;

    std::vector<gl_obj::pos_vec>
        edges,
        edges_highlight;

    std::vector<gl_obj::pos_vec>
        edges_dashed;


    // enum display_context {
    //     dc_none, dc_drp, dc_omd
    // };
    // display_context _context;

    DRP_Display_Window* drp_display_window;
    DR_Plan* drp;
    DRP_Node* current_drp_node;


    void _update_graph_positions_none();
    void _update_graph_positions_drp();
};


#endif
