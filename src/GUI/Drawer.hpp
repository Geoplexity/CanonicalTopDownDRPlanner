#ifndef DRAWER_HPP
#define DRAWER_HPP


#include "Shader_Program.hpp"
#include "gl_objects.hpp"




#ifdef __APPLE__
    #include <GL/glew.h>

    // #include <OpenGL/gl.h>
    // #include <OpenGL/glu.h>
    // #include <GLUT/glut.h>

    #include <GL/gl.h>
    #include <GL/glu.h>
    // #include <GL/freeglut.h>
#else
    #include <GL/glew.h>
    #include <GL/freeglut.h>
#endif

#include <GLFW/glfw3.h>





#include "../../ext/glm/glm.hpp"



#include <string>
#include <vector>
#include <set>




template <typename T>
struct uniform_with_location {
    T uniform;
    GLuint location;
};

class Drawer {
public:
    Drawer(std::string vs_file, std::string fs_file);

    ~Drawer();

    ////
    //// BEGIN INITIALIZATION FUNCTIONS
    void init_shaders(const char* vs_file_name, const char* fs_file_name);
    void init_attributes();
    //// END INITIALIZATION FUNCTIONS
    ////




    ////
    //// BEGIN UNIFORM SETTERS
    void setUniform_ViewMatrix(glm::mat4 view_matrix);
    void setUniform_ProjectionMatrix(glm::mat4 projection_matrix);
    //// END UNIFORM SETTERS
    ////




    ////
    //// BEGIN DRAWING FUNCTIONS

    void draw_graph_vertices(
        const std::vector<gl_obj::pos_vec> &p,
        const std::vector<gl_obj::pos_vec> &highlight);
    void draw_graph_edges(
        const std::vector<gl_obj::pos_vec> &e,
        const std::vector<gl_obj::pos_vec> &highlight,
        const std::vector<gl_obj::pos_vec> &dashed);
    void draw_graph_vertices_names(
        const std::vector<std::string> &p_names,
        const std::vector<gl_obj::pos_vec> &p,
        const std::vector<std::string> &highlight_names,
        const std::vector<gl_obj::pos_vec> &highlight);

    // void draw_easy_font(gl_obj::VertexGroup &vg);
    void draw_letter(const GLfloat pos[4], const GLfloat tex_coord[4], GLuint tex_id);

    //// END DRAWING FUNCTIONS
    ////



    void clearViewport();
    void flush();
    glm::vec4 colorAtPixel(int x, int y);

private:
    // program info
    Shader_Program shader_program;


    // vao, vbo, ebo locations
    GLuint vao;
    GLuint vbo;
    GLuint vbo_indices;



    // attribute locations
    GLuint position_location,
        color_location,
        texCoord_location;

    //uniform location
    GLuint translate_location,
        render_mode_location,
        tex_sampler_location;

    uniform_with_location<glm::mat4> _view_matrix;
    uniform_with_location<glm::mat4> _projection_matrix;



    void _draw_graph_vertices_aux(
        const std::vector<gl_obj::pos_vec> &p,
        float radius,
        gl_obj::color_vec color,
        bool border = true);

    void _draw_graph_edges_aux(
        const std::vector<gl_obj::pos_vec> &e,
        float width,
        gl_obj::color_vec color);

    void _draw_graph_edges_dashed_aux(
        const std::vector<gl_obj::pos_vec> &e,
        float width, float dash_length, float undash_length,
        gl_obj::color_vec color);

    void _draw_graph_vertices_names_aux(
        const std::vector<std::string> &p_names,
        const std::vector<gl_obj::pos_vec> &p,
        float radius,
        gl_obj::color_vec color);
};


#endif
