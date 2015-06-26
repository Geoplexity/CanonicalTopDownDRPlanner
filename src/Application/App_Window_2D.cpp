#include "App_Window_2D.hpp"


App_Window_2D::App_Window_2D(Main_GUI_Manager *mgm) :
    mgm(mgm) {}

App_Window_2D::~App_Window_2D() {
    delete(drawer);
    delete(camera);
}

void App_Window_2D::init_drawer() {
    set_as_context();
    this->drawer = new Drawer("src/GUI/shaders/vs.glsl", "src/GUI/shaders/fs.glsl");

    init_camera();
}

void App_Window_2D::init_camera() {
    int width, height;
    this->get_window_size_in_pixels(&width, &height);
    camera = new Camera_2D(float(width) / float(height));

    // update_view_matrix();
    // update_projection_matrix();
    update_projection_view_matrix();
}

// void App_Window_2D::update_view_matrix() {
//     this->drawer->set_uniform__view_matrix(camera->get_view_matrix());
// }

// void App_Window_2D::update_projection_matrix() {
//     this->drawer->set_uniform__projection_matrix(camera->get_projection_matrix());
// }

void App_Window_2D::update_projection_view_matrix() {
    this->drawer->set_uniform__projection_view_matrix(camera->get_projection_view_matrix());
}

void App_Window_2D::recenter_camera() {
    camera->revert_to_original();
    // update_view_matrix();
    // update_projection_matrix();
    update_projection_view_matrix();
}

void App_Window_2D::window_resize_callback(int width, int height) {
    camera->aspect_ratio(float(width) / float(height));
    // update_projection_matrix();
    update_projection_view_matrix();
    update_display();
}







Movable_App_Window_2D::Movable_App_Window_2D(Main_GUI_Manager *mgm) :
    App_Window_2D(mgm),
    left_mouse_clicking(false) {}

void Movable_App_Window_2D::mouse_button_callback(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_1) {
        if (action == GLFW_PRESS) {
            drag_camera_begin();
        } else if (action == GLFW_RELEASE) {
            drag_camera_end();
        }
    }
}

void Movable_App_Window_2D::cursor_pos_callback(double xpos, double ypos) {
    if (this->left_mouse_clicking) {
        update_mouse_pos_camera_coords();

        camera->translate(-dx, -dy);

        // update_view_matrix();
        update_projection_view_matrix();
        update_display();
    }
}

void Movable_App_Window_2D::scroll_callback(double x_offset, double y_offset) {
    if (y_offset > 0) {
        //scroll up
        camera->scale_zoom(0.95, 0.01f, 50.f);
    } else {
        //scroll down
        camera->scale_zoom(1/0.95, 0.01f, 50.f);
    }

    // update_view_matrix();
    update_projection_view_matrix();
    update_display();
}

void Movable_App_Window_2D::recenter_camera() {
    camera->revert_to_original();
    update_projection_view_matrix();
    update_display();

    if (left_mouse_clicking) {
        update_mouse_pos_camera_coords();
    }
}

void Movable_App_Window_2D::drag_camera_begin() {
    this->left_mouse_clicking = true;
    this->get_cursor_position_pixels(&mouse_xpos_previous, &mouse_ypos_previous);
}

void Movable_App_Window_2D::drag_camera_end() {
    this->left_mouse_clicking = false;
}

void Movable_App_Window_2D::update_mouse_pos_camera_coords() {
    int x, y;
    this->get_cursor_position_pixels(&x, &y);

    int width, height;
    this->get_window_size_in_pixels(&width, &height);

    dx = 2*((float)(x - mouse_xpos_previous))/width;
    dy = 2*((float)(y - mouse_ypos_previous))/height;

    float ar = camera->aspect_ratio();
    if (ar > 1) dx *= ar;
    else        dy /= ar;

    mouse_xpos_previous = x;
    mouse_ypos_previous = y;
}

void Movable_App_Window_2D::update_mouse_pos_world_coords() {
    update_mouse_pos_camera_coords();

    float z  = camera->zoom();
    dx /= z;
    dy /= z;
}
