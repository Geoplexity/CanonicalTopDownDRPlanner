#ifndef MAINGUIMANAGER_HPP
#define MAINGUIMANAGER_HPP

#include <GL/glew.h> // must come before GLFW include
#define GLFW_INCLUDE_GL_3
#include <GLFW/glfw3.h>

#include <exception>
#include <map>
#include <vector>




struct Frame {
unsigned int bottom, left, width, height;
};


class Window {
friend class MainGuiManager;
public:
    Window();

    void set_as_context();
    bool should_close();
    void swap_buffers();

    void get_window_size_in_pixels(int *width, int *height);
    void get_window_size_in_screen_coords(int *width, int *height);

    // from bottom left
    void get_cursor_position_pixels(int *x, int *y);



    // addframe
    // clearframe
    // preparebufferforframe
    // swapbufferforframe




    // typedef void(*  GLFWerrorfun )(int, const char *) The function signature for error callbacks. More...
    // typedef void(*  GLFWwindowposfun )(GLFWwindow *, int, int) The function signature for window position callbacks. More...
    // static void window_resize_callback_wrapper  (GLFWwindow *window, int width, int height);
    // static void window_close_callback_wrapper   (GLFWwindow *window);
    // static void window_refresh_callback_wrapper (GLFWwindow *window);
    // typedef void(*  GLFWwindowfocusfun )(GLFWwindow *, int) The function signature for window focus/defocus callbacks. More...
    // typedef void(*  GLFWwindowiconifyfun )(GLFWwindow *, int) The function signature for window iconify/restore callbacks. More...
    // typedef void(*  GLFWframebuffersizefun )(GLFWwindow *, int, int) The function signature for framebuffer resize callbacks. More...
    virtual void mouse_button_callback(int button, int action, int mods) {};
    virtual void cursor_pos_callback  (double xpos, double ypos) {};
    // typedef void(*  GLFWcursorenterfun )(GLFWwindow *, int) The function signature for cursor enter/leave callbacks. More...
    virtual void scroll_callback      (double x_offset, double y_offset) {};
    virtual void key_callback         (int key, int scancode, int action, int mods) {};
    // typedef void(*  GLFWcharfun )(GLFWwindow *, unsigned int) The function signature for Unicode character callbacks. More...
    // typedef void(*  GLFWmonitorfun )(GLFWmonitor *, int) The function signature for monitor configuration callbacks. More...
private:
    GLFWwindow* _glfw_window;

    int width_screen_coords, height_screen_coords;

    // std::vector<Frame> frames;

    void glfw_window(GLFWwindow* glfwwindow) {_glfw_window = glfwwindow;}
    GLFWwindow* glfw_window() {return _glfw_window;}
};





class MainGuiManager {
public:
    MainGuiManager();
    ~MainGuiManager();

    // returns new window reference
    void create_window(
        Window* window,
        int width_in_screen_coordinates,
        int height_in_screen_coordinates,
        const char *title,
        unsigned int GL_major_version,
        unsigned int GL_minor_version,
        bool resizable = false,
        bool decorated = true);

    // returns the old current_window_context and sets it to the given one
    static GLFWwindow* set_window_context(GLFWwindow* new_window);

    void close_window(GLFWwindow* window);

    void poll_for_events();

    void wait_for_events();

private:
    static bool instance_exists;
    // static Window *current_window_context;
    static GLFWwindow *current_window_context;
    static std::map<GLFWwindow*, Window*> window_mapper;

    // typedef void(*  GLFWerrorfun )(int, const char *) The function signature for error callbacks. More...
    // typedef void(*  GLFWwindowposfun )(GLFWwindow *, int, int) The function signature for window position callbacks. More...
    // static void window_resize_callback_wrapper  (GLFWwindow *window, int width, int height);
    // static void window_close_callback_wrapper   (GLFWwindow *window);
    // static void window_refresh_callback_wrapper (GLFWwindow *window);
    // typedef void(*  GLFWwindowfocusfun )(GLFWwindow *, int) The function signature for window focus/defocus callbacks. More...
    // typedef void(*  GLFWwindowiconifyfun )(GLFWwindow *, int) The function signature for window iconify/restore callbacks. More...
    // typedef void(*  GLFWframebuffersizefun )(GLFWwindow *, int, int) The function signature for framebuffer resize callbacks. More...
    static void mouse_button_callback_wrapper   (GLFWwindow *window, int button, int action, int mods);
    static void cursor_pos_callback_wrapper     (GLFWwindow *window, double xpos, double ypos);
    // typedef void(*  GLFWcursorenterfun )(GLFWwindow *, int) The function signature for cursor enter/leave callbacks. More...
    static void scroll_callback_wrapper         (GLFWwindow *window, double x_offset, double y_offset);
    static void key_callback_wrapper            (GLFWwindow *window, int key, int scancode, int action, int mods);
    // typedef void(*  GLFWcharfun )(GLFWwindow *, unsigned int) The function signature for Unicode character callbacks. More...
    // typedef void(*  GLFWmonitorfun )(GLFWmonitor *, int) The function signature for monitor configuration callbacks. More...
};



class ex_instance_exists: public std::exception {
    virtual const char* what() const throw() {
        return "An instance of MainGuiManager already exists.";
    }
};


#endif
