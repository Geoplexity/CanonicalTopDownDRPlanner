#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP


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




#include <string>
#include <map>



void check_for_GL_errors(const char* from);



class Shader_Program {
public:
    Shader_Program();
    ~Shader_Program();

    void compile_and_attach_vertex_shader(const char* file_name);
    void compile_and_attach_fragment_shader(const char* file_name);
    // void compile_and_attach_geometry_shader(const char* file_name);
    // void compile_and_attach_tess_control_shader(const char* file_name);
    // void compile_and_attach_tess_evaluation_shader(const char* file_name);

    void link();

    void use();
    void un_use();

    GLuint add_attribute(std::string name);
    GLuint add_uniform(std::string name);

    GLuint get_attribute(std::string name);
    GLuint get_uniform(std::string name);

private:
    GLuint program;

    GLuint
        vertex_shader,
        fragment_shader;

    bool has_vertex_shader;
    bool has_fragment_shader;
    bool linked;

    std::map<std::string, GLuint> attribute_map;
    std::map<std::string, GLuint> uniform_map;

    GLuint compile_and_attach_shader(GLuint type, const char* file_name);
    void detach_and_delete_shader(GLuint shader);
    void detach_and_delete_shaders();
};



#endif
