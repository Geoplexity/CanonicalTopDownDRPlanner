#include "Shader_Program.hpp"

// #include "utils.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>

#include <iostream>
#include <fstream>
#include <sstream>

#include <cassert>



void read_shader_source_code(const char* filename, char *buf, int buf_size) {
    int file_size = 0;

    std::ifstream file(filename, std::ios::in);

    memset(buf, 0, buf_size);
    if (file) {
        file.seekg(0, std::ios::end);
        file_size = file.tellg();
        file.seekg (0, std::ios::beg);

        if (buf_size < file_size)
            std::cerr << "read_shader_source_code: Shader file size too large." << std::endl;

        file.read(buf, std::min(buf_size, file_size));
        file.close();

        buf[std::min(buf_size,file_size)] = '\0';

        file.close();
    } else {
        std::cerr << "read_shader_source_code: Can't open shader file '" << filename << "'." << std::endl;
    }

    return;
}

void check_shader_compile_error(GLuint shader) {
    GLint compile_status;
    char info[1024];
    int info_len = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

    if (!compile_status) {
        glGetShaderInfoLog(shader, 1024, &info_len, info);
        std::cerr << "Shader compile error: " << std::endl;
        std::cerr << info << std::endl;
        exit(EXIT_FAILURE);
    }
    // else {
    //     std::cout << "Shader compile successful." << std::endl;
    // }
}

void check_program_linking_error(GLuint program) {
    GLint link_status;
    char info[1024];
    int info_len = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);

    if (!link_status) {
        glGetProgramInfoLog(program, 1024, &info_len, info);
        std::cerr << "Program linking error: " << std::endl;
        std::cerr << info << std::endl;
        exit(EXIT_FAILURE);
    }
    // else {
    //     std::cout << "Program linking successful." << std::endl;
    // }
}




void check_for_GL_errors(const char* from) {
    GLenum err = glGetError();
    if (err) {
        std::cout << "(" << from << ") Had error type: ";
        switch(err) {
            case GL_INVALID_ENUM: std::cerr << "GL_INVALID_ENUM" << std::endl; break;
            case GL_INVALID_VALUE: std::cerr << "GL_INVALID_VALUE" << std::endl; break;
            case GL_INVALID_OPERATION: std::cerr << "GL_INVALID_OPERATION" << std::endl; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl; break;
            case GL_OUT_OF_MEMORY: std::cerr << "GL_OUT_OF_MEMORY" << std::endl; break;
            default: std::cerr << "Other" << std::endl;
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
    check_shader_compile_error(handle);

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
        check_program_linking_error(program);

        // enable depth test
        // TODO: this probably should go somewhere else
        // glEnable(GL_DEPTH_TEST);

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
    // std::cout << "Shader_Program::add_attribute: " << name << " = " << handle << std::endl;
    attribute_map[name] = handle;
    return handle;
}

GLuint Shader_Program::add_uniform(std::string name) {
    GLuint handle = glGetUniformLocation(program, name.c_str());
    // std::cout << "Shader_Program::add_uniform: " << name << " = " << handle << std::endl;
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
