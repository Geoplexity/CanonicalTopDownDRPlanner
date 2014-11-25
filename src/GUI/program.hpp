#ifndef PROGRAM_HPP
#define PROGRAM_HPP


#include "Circle.hpp"




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






class Program {
public:
  Program(std::string vs_file, std::string fs_file);

  ~Program();

  ////
  //// BEGIN INITIALIZATION FUNCTIONS

  void init_graphics();
  void init_shaders();
  void init_attributes();
  void init_uniforms();

  //// END INITIALIZATION FUNCTIONS
  ////




  ////
  //// BEGIN UNIFORM SETTERS

  //// END UNIFORM SETTERS
  ////




  ////
  //// BEGIN DRAWING FUNCTIONS

  void draw_graph_vertices(std::vector<gl_obj::pos_vec> &p);
  void draw_graph_edges(std::vector<gl_obj::pos_vec> &p);

  void draw_letter(const GLfloat pos[4], const GLfloat tex_coord[4], GLuint tex_id);

  //// END DRAWING FUNCTIONS
  ////


  void check_for_GL_errors(const char* from);

  void clearViewport();
  void flush();
  glm::vec4 colorAtPixel(int x, int y);

private:
  struct shaders {
    GLuint vertex;
    std::string v_file;

    GLuint fragment;
    std::string f_file;
  };



  // program info
  GLuint program;
  shaders sh;


  // attribute locations
  GLuint vao;
  GLuint vbo;
  GLuint vbo_indices;
  GLuint position_location,
         color_location,
         texCoord_location;


  //uniforms
  GLuint translate_location,
         render_mode_location,
         tex_sampler_location;
};


#endif
