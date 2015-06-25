#include "Shader_Program.hpp"

#include "utils.h"

#include <cassert>
#include <iostream>




void check_for_GL_errors(const char* from) {
    GLenum err = glGetError();
    if (err) {
        std::cout << "(" << from << ") Had error type: ";
        switch(err) {
            case GL_INVALID_ENUM: std::cout << "GL_INVALID_ENUM" << std::endl; break;
            case GL_INVALID_VALUE: std::cout << "GL_INVALID_VALUE" << std::endl; break;
            case GL_INVALID_OPERATION: std::cout << "GL_INVALID_OPERATION" << std::endl; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl; break;
            case GL_OUT_OF_MEMORY: std::cout << "GL_OUT_OF_MEMORY" << std::endl; break;
            default: std::cout << "Other" << std::endl;
        }
    }
}




Shader_Program::Shader_Program() :
    has_vertex_shader(false),
    has_fragment_shader(false),
    linked(false)
{
    program = glCreateProgram();
}

Shader_Program::~Shader_Program() {
    if (!linked) detach_and_delete_shaders();
    glDeleteProgram(program);
}

GLuint Shader_Program::compile_and_attach_shader(GLuint type, const char* file_name) {
    GLuint handle;

    const int buflength = 4000;
    char buf[buflength];

    read_shader_source_code(file_name, buf, buflength);
    int source_length = strlen(buf);
    const char *sources[] = { buf };

    handle = glCreateShader(type);
    glShaderSource(handle, 1, sources, &source_length);
    glCompileShader(handle);

    glAttachShader(program, handle);

    return handle;
}

void Shader_Program::detach_and_delete_shader(GLuint shader) {
    glDetachShader(program, shader);
    glDeleteShader(shader);
}

void Shader_Program::detach_and_delete_shaders() {
    if (has_vertex_shader)   detach_and_delete_shader(vertex_shader);
    if (has_fragment_shader) detach_and_delete_shader(fragment_shader);
    // if (has_x_shader) detach_and_delete_shader(x_shader);
}

void Shader_Program::compile_and_attach_vertex_shader(const char* file_name) {
    if (!linked) {
        vertex_shader = compile_and_attach_shader(GL_VERTEX_SHADER, file_name);
        has_vertex_shader = true;
    }
}

void Shader_Program::compile_and_attach_fragment_shader(const char* file_name) {
    if (!linked) {
        fragment_shader = compile_and_attach_shader(GL_FRAGMENT_SHADER, file_name);
        has_fragment_shader = true;
    }
}

void Shader_Program::link() {
    if (has_vertex_shader && has_fragment_shader) {
        // link
        glLinkProgram(program);

        // enable depth test
        // TODO: this probably should go somewhere else
        glEnable(GL_DEPTH_TEST);

        // detach and delete
        detach_and_delete_shaders();

        linked = true;
    }
}

void Shader_Program::use(){
    glUseProgram(program);
}

void Shader_Program::un_use(){
    glUseProgram(0);
}

GLuint Shader_Program::add_attribute(std::string name) {
    GLuint handle = glGetAttribLocation(program, name.c_str());
    std::cout << "Shader_Program::add_attribute: " << name << " = " << handle << std::endl;
    attribute_map[name] = handle;
    return handle;
}

GLuint Shader_Program::add_uniform(std::string name) {
    GLuint handle = glGetUniformLocation(program, name.c_str());
    std::cout << "Shader_Program::add_uniform: " << name << " = " << handle << std::endl;
    uniform_map[name] = handle;
    return handle;
}

GLuint Shader_Program::get_attribute(std::string name) {
    // return attribute_map[name];
    std::map<std::string, GLuint>::iterator handle = attribute_map.find(name);
    assert(handle != attribute_map.end());
    return handle->second;
}

GLuint Shader_Program::get_uniform(std::string name) {
    // return uniform_map[name];
    std::map<std::string, GLuint>::iterator handle = uniform_map.find(name);
    assert(handle != uniform_map.end());
    return handle->second;
}



// {
//     Shader_Program shader_program;
//     shader_program.compile_and_attach_vertex_shader(vs_file);
//     shader_program.compile_and_attach_fragment_shader(fs_file);
//     shader_program.link();

//     shader_program.use();
//     // render
//     shader_program.un_use();
// }
