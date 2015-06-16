#ifndef ANIMATED_GRAPH_REALIZATION_WINDOW_HPP
#define ANIMATED_GRAPH_REALIZATION_WINDOW_HPP

#include "../Graph/Graph.hpp"
#include "../Graph/DR_Plan.hpp"
#include "../Graph/Isostatic_Graph_Realizer.hpp"

#include "App_Window_2D.hpp"
#include "DRP_Display_Window.hpp"

#include <string>




class Animated_Graph_Realization_Window : public Movable_App_Window_2D {
public:
    Animated_Graph_Realization_Window(Main_GUI_Manager *mgm, Graph *graph);
    ~Animated_Graph_Realization_Window();

    bool step();

    void update_graph_positions();

    void key_callback(int key, int scancode, int action, int mods);

    void update_display();
private:
    // Graph *original_graph;
    Graph *graph;
    const Mapped_Graph_Copy *display_graph;

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

    Isostatic_Graph_Realizer *igr;

    void _update_graph_positions_none();
    void _update_graph_positions_omd();
};


#endif
