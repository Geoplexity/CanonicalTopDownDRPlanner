#ifndef DRP_DISPLAY_WINDOW_HPP
#define DRP_DISPLAY_WINDOW_HPP


#include "../Graph/Graph.hpp"
#include "../Graph/DR_Plan.hpp"

#include "App_Window_2D.hpp"


class DRP_Display_Window : public App_Window_2D {
public:
    DRP_Display_Window(
        Main_GUI_Manager *mgm,
        DR_Plan *drp);

    void highlight_drp_node(DRP_Node *node, std::set<DRP_Node*> ancestors);

    void update_graph_positions();

    void update_display();
private:
    std::vector<gl_obj::pos_vec> vertices, vertices_highlight;
    std::vector<gl_obj::pos_vec> edges, edges_highlight;

    DR_Plan *drp;
    DRP_Node *current_drp_node;
    std::set<DRP_Node*> current_and_ancestors;
};


#endif
