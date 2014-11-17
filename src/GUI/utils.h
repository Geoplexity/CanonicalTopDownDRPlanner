#ifndef UTILS_H
#define UTILS_H



#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// #ifdef __APPLE__
//     #include <GL/glew.h>

//     // #include <OpenGL/gl.h>
//     // #include <OpenGL/glu.h>
//     // #include <GLUT/glut.h>

//     #include <GL/gl.h>
//     #include <GL/glu.h>
//     #include <GL/freeglut.h>
// #else
//     #include <GL/glew.h>
//     #include <GL/freeglut.h>
// #endif

#include <GLFW/glfw3.h>


float frand();
void read_shader_source_code(const char* filename, char *buf, int buf_size);

void check_shader_compile_error(GLuint shader);
void check_program_linking_error(GLuint program);



#endif
