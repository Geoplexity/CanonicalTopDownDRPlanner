#ifndef DRP_DISPLAY_WINDOW_HPP
#define DRP_DISPLAY_WINDOW_HPP


#include "../Graph/Graph.hpp"
#include "../Graph/DR_Plan.hpp"

#include "App_Window_2D.hpp"


class DRP_Display_Window : public App_Window_2D {
public:
    DR_Plan *drp;
    DRP_Node *highlight;
    std::vector<gl_obj::pos_vec> vertices, vertices_highlight;
    std::vector<gl_obj::pos_vec> edges, edges_highlight;

    // unsigned int height;
    // std::vector<unsigned int> width_per_level;

    DRP_Display_Window(DR_Plan *drp);

    void highlight_drp_node(DRP_Node *node);

    void update_graph_positions();

    void update_display();
};


#endif
