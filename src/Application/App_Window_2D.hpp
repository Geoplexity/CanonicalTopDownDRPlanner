#ifndef APP_WINDOW_2D_HPP
#define APP_WINDOW_2D_HPP


#include "../GUI/Main_GUI_Manager.hpp"
#include "../GUI/Vision_Handler_2D.hpp"
#include "../GUI/program.hpp"


class App_Window_2D : public Window {
public:
    App_Window_2D(Main_GUI_Manager *mgm);
    ~App_Window_2D();

    void init_program();
    void init_vision_handler();

    void update_view_matrix();
    void update_projection_matrix();

    void recenter();

    virtual void update_display() = 0;

    void window_resize_callback(int width, int height);
protected:
    Main_GUI_Manager *mgm;
    Program *program;
    Vision_Handler_2D *vh;
};


class Movable_App_Window_2D : public App_Window_2D {
public:
    Movable_App_Window_2D(Main_GUI_Manager *mgm);

    void mouse_button_callback(int button, int action, int mods);
    void cursor_pos_callback(double xpos, double ypos);
    void scroll_callback(double x_offset, double y_offset);
protected:
    bool left_mouse_clicking;
    int mouse_xpos_previous, mouse_ypos_previous;
};

#endif
