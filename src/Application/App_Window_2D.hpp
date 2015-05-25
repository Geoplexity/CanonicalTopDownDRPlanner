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

    App_Window_2D() {}

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

#endif
