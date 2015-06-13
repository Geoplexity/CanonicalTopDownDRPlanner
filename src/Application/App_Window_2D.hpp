#ifndef APP_WINDOW_2D_HPP
#define APP_WINDOW_2D_HPP


#include "../GUI/Main_GUI_Manager.hpp"
#include "../GUI/Vision_Handler_2D.hpp"
#include "../GUI/program.hpp"


class App_Window_2D : public Window {
public:
    Main_GUI_Manager *mgm;
    Program *program;
    Vision_Handler_2D *vh;

    App_Window_2D(Main_GUI_Manager *mgm) : mgm(mgm) {}

    void init_program() {
        set_as_context();
        this->program = new Program("src/GUI/shaders/vs.glsl", "src/GUI/shaders/fs.glsl");

        init_vision_handler();
    }

    void init_vision_handler() {
        int width, height;
        this->get_window_size_in_pixels(&width, &height);
        vh = new Vision_Handler_2D(float(width) / float(height));

        update_view_matrix();
        update_projection_matrix();
    }

    void update_view_matrix() {
        this->program->setUniform_ViewMatrix(vh->get_view_matrix());
    }

    void update_projection_matrix() {
        this->program->setUniform_ProjectionMatrix(vh->get_projection_matrix());
    }

    virtual void update_display() = 0;

    void window_resize_callback(int width, int height) {
        vh->aspect_ratio(float(width) / float(height));
        update_projection_matrix();
        update_display();
    }
};




class Movable_App_Window_2D : public App_Window_2D {
public:
    Movable_App_Window_2D(Main_GUI_Manager *mgm) :
        App_Window_2D(mgm),
        left_mouse_clicking(false) {}

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
        if (this->left_mouse_clicking) {
            int x, y;
            this->get_cursor_position_pixels(&x, &y);

            int width, height;
            this->get_window_size_in_pixels(&width, &height);

            float dx,dy;
            dx = 2*((float)(mouse_xpos_previous - x))/width;
            dy = 2*((float)(mouse_ypos_previous - y))/height;

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
        if (y_offset > 0) {
            //scroll up
            vh->scale_zoom(0.95, 0.01f, 50.f);
        } else {
            //scroll down
            vh->scale_zoom(1/0.95, 0.01f, 50.f);
        }

        update_view_matrix();
        update_display();
    }
protected:
    bool left_mouse_clicking;
    int mouse_xpos_previous, mouse_ypos_previous;
};

#endif
