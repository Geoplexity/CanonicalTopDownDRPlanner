#include "MainGuiManager.hpp"

#include <iostream>

#include <stdlib.h>
#include <stdio.h>



// initialize static members
bool MainGuiManager::instance_exists = false;
GLFWwindow* MainGuiManager::current_window_context = NULL;
std::map<GLFWwindow*, Window*> MainGuiManager::window_mapper;



Window::Window() {
    _glfw_window = NULL;
}

void Window::set_as_context() {
    MainGuiManager::set_window_context(this->glfw_window());
}

bool Window::should_close() {
    return glfwWindowShouldClose(this->glfw_window());
}

void Window::swap_buffers() {
    glfwSwapBuffers(this->glfw_window());
}

void Window::get_window_size_in_pixels(int *width, int *height) {
    glfwGetFramebufferSize(this->glfw_window(), width, height);
}

void Window::get_cursor_position_pixels(int *x, int *y) {
    int width, height;
    get_window_size_in_pixels(&width, &height);

    double fx, fy;
    glfwGetCursorPos(this->glfw_window(), &fx, &fy);
    *x = (int) fx * (width / this->width_screen_coords);
    *y = (int) fy * (height / this->height_screen_coords);

    *y = height - *y;
}













void print_info() {
    fprintf(
        stdout,
        "INFO: OpenGL Version: %s\n",
        glGetString(GL_VERSION)
        );
    fprintf(stdout, "INFO: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    if(glewIsSupported("GL_ARB_debug_output"))
    {
        fprintf(stdout, "INFO: Support ARB_debug_output\n");
    }
    else
    {
        fprintf(stdout, "INFO: Not support ARB_debug_output\n");
    }
}


static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}




MainGuiManager::MainGuiManager() {
    if (instance_exists) throw ex_instance_exists();

    // setup GLFW
    if (!glfwInit()) exit(EXIT_FAILURE);
    glfwSetErrorCallback(error_callback);

    MainGuiManager::instance_exists = true;
}

MainGuiManager::~MainGuiManager() {
    // for some reason closing the windows in the deconstructor will give segfault

    // for (std::map<GLFWwindow*, Window*>::iterator it = MainGuiManager::window_mapper.begin();
    //     it != MainGuiManager::window_mapper.end();
    //     it++)
    // {
    //     this->close_window(it->first);
    // }


    glfwTerminate();
}


void MainGuiManager::create_window(
        Window* window,
        int width_in_screen_coordinates,
        int height_in_screen_coordinates,
        const char *title,
        unsigned int GL_major_version,
        unsigned int GL_minor_version,
        bool resizable,
        bool decorated)
{
    // window settings, must be done before creating the window

    glfwDefaultWindowHints();

    // resizable would mean recalculating the sizes for a lot of things
    if (!resizable) glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    if (!decorated) glfwWindowHint(GLFW_DECORATED, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_major_version);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_minor_version);
    if (GL_major_version >= 3) {
        if (GL_major_version == 3 && GL_minor_version >= 2) {
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }
    }




    // make the window
    GLFWwindow* new_glfw_window;
    // assumes the size requested is made
    new_glfw_window = glfwCreateWindow(width_in_screen_coordinates,
        height_in_screen_coordinates,
        title,
        NULL, NULL);
    // // this is for fullscreen, the size you request here will not be made! Code won't work.
    // new_glfw_window = glfwCreateWindow(640, 480, "My Title", glfwGetPrimaryMonitor(), NULL); //fullscreen

    if (!new_glfw_window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    window->glfw_window(new_glfw_window);
    window->width_screen_coords = width_in_screen_coordinates;
    window->height_screen_coords = height_in_screen_coordinates;

    MainGuiManager::window_mapper[new_glfw_window] = window;

    GLFWwindow* old_glfw_window = set_window_context(new_glfw_window);



    // Setup GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if(GLEW_OK != err)
    {
        // Problem: glewInit failed, something is seriously wrong.
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }

    print_info();



    // bind functions for window
    glfwSetMouseButtonCallback(new_glfw_window, mouse_button_callback_wrapper);
    glfwSetCursorPosCallback(new_glfw_window, cursor_pos_callback_wrapper);
    glfwSetScrollCallback(new_glfw_window, scroll_callback_wrapper);
    glfwSetKeyCallback(new_glfw_window, key_callback_wrapper);





    // get width and height
    // glfwGetFramebufferSize(new_glfw_window, &global::window::width_px, &global::window::height_px);



    set_window_context(old_glfw_window);
}


GLFWwindow* MainGuiManager::set_window_context(GLFWwindow* new_window) {
    GLFWwindow* old_window_context = MainGuiManager::current_window_context;
    MainGuiManager::current_window_context = new_window;
    glfwMakeContextCurrent(new_window);
    // std::cout << "Old context: " << old_window_context << "."
    //     << std::endl
    //     << "=> New context should be: " << new_window << "."
    //     << " It is... " << glfwGetCurrentContext()
    //     << std::endl;
    return old_window_context;
}



void MainGuiManager::close_window(GLFWwindow* window) {
    size_t erased = MainGuiManager::window_mapper.erase(window);
    if (erased == 1) glfwDestroyWindow(window);
}



void MainGuiManager::poll_for_events(){
    glfwPollEvents();
}


void MainGuiManager::wait_for_events(){
    std::cout << "\tAbout to wait." << std::endl;
    glfwWaitEvents();
    std::cout << "\tHandled." << std::endl;
    // try {
    //     glfwWaitEvents();
    // } catch(...) {
    //     std::cout <<"Error!" << std::endl;
    // }
}








void MainGuiManager::mouse_button_callback_wrapper(GLFWwindow *window, int button, int action, int mods) {
    // MainGuiManager::current_window_context->mouse_button_callback(button, action, mods);

    std::map<GLFWwindow*, Window*>::iterator win = MainGuiManager::window_mapper.find(window);
    if (win != MainGuiManager::window_mapper.end()) {
        // if (win->second->mouse_button_callback != NULL)
        win->second->mouse_button_callback(button, action, mods);
    }
}

void MainGuiManager::cursor_pos_callback_wrapper(GLFWwindow *window, double xpos, double ypos) {
    std::map<GLFWwindow*, Window*>::iterator win = MainGuiManager::window_mapper.find(window);
    if (win != MainGuiManager::window_mapper.end()) {
        win->second->cursor_pos_callback(xpos, ypos);
    }
}

void MainGuiManager::scroll_callback_wrapper(GLFWwindow *window, double x_offset, double y_offset) {
    std::map<GLFWwindow*, Window*>::iterator win = MainGuiManager::window_mapper.find(window);
    if (win != MainGuiManager::window_mapper.end()) {
        win->second->scroll_callback(x_offset, y_offset);
    }
}

void MainGuiManager::key_callback_wrapper(GLFWwindow* window, int key, int scancode, int action, int mods) {
    std::map<GLFWwindow*, Window*>::iterator win = MainGuiManager::window_mapper.find(window);
    if (win != MainGuiManager::window_mapper.end()) {
        win->second->key_callback(key, scancode, action, mods);
    }
}
