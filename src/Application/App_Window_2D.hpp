#ifndef APP_WINDOW_2D_HPP
#define APP_WINDOW_2D_HPP


#include "../GUI/Main_GUI_Manager.hpp"
#include "../GUI/Camera_2D.hpp"
#include "../GUI/Drawer.hpp"


class App_Window_2D : public Window {
public:
    App_Window_2D(Main_GUI_Manager *mgm);
    ~App_Window_2D();

    void init_drawer();
    void init_camera();

    // void update_view_matrix();
    // void update_projection_matrix();
    void update_projection_view_matrix();

    void recenter_camera();

    virtual void update_display() = 0;

    void window_resize_callback(int width, int height);
protected:
    Main_GUI_Manager *mgm;
    Drawer *drawer;
    Camera_2D *camera;
};


class Movable_App_Window_2D : public App_Window_2D {
public:
    Movable_App_Window_2D(Main_GUI_Manager *mgm);

    void mouse_button_callback(int button, int action, int mods);
    void cursor_pos_callback(double xpos, double ypos);
    void scroll_callback(double x_offset, double y_offset);

    void recenter_camera();
protected:
    bool left_mouse_clicking;
    int mouse_xpos_previous, mouse_ypos_previous;
    float dx, dy;

    void drag_camera_begin();
    void drag_camera_end();

    void update_mouse_pos_camera_coords();
    void update_mouse_pos_world_coords();
};

#endif
