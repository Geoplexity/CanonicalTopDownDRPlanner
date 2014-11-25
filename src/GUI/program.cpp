#include "program.hpp"

#include "utils.h"

#include "../../ext/glm/gtc/matrix_transform.hpp"

#include <cstring>
#include <cstdio>

#include <iostream>

using namespace std;



namespace Render_Mode {
  GLuint
    basic2D = 0,
    font = 1;
}



namespace color {
  // offwhite/green/black
  // const gl_obj::color_vec bg   = gl_obj::color_vec(241/255.f, 242/255.f, 228/255.f, 1.f);
  // const gl_obj::color_vec vert = gl_obj::color_vec(104/255.f, 129/255.f, 80/255.f, 0.f);
  // const gl_obj::color_vec edge = gl_obj::color_vec(0.f, 0.f, 0.f, 0.f);

  // gray/green/black
  const gl_obj::color_vec bg   = gl_obj::color_vec(0.85f, 0.85f, 0.85f, 1.f);
  const gl_obj::color_vec vert = gl_obj::color_vec(0.2f, 0.6f, 0.2f, 0.f);
  // const gl_obj::color_vec vert = gl_obj::color_vec(0.6f, 0.2f, 0.6f, 0.f);
  const gl_obj::color_vec edge = gl_obj::color_vec(0.f, 0.f, 0.f, 0.f);

  // black/green/white
  // const gl_obj::color_vec bg   = gl_obj::color_vec(0.f, 0.f, 0.f, 1.f);
  // const gl_obj::color_vec vert = gl_obj::color_vec(0.1f, 1.0f, 0.1f, 0.f);
  // const gl_obj::color_vec edge = gl_obj::color_vec(1.f, 1.f, 1.f, 0.f);

  // gray/blue/black
  // const gl_obj::color_vec bg   = gl_obj::color_vec(0.85f, 0.85f, 0.85f, 1.f);
  // const gl_obj::color_vec vert = gl_obj::color_vec(0.f, 0.f, 1.f, 0.f);
  // const gl_obj::color_vec edge = gl_obj::color_vec(0.f, 0.f, 0.f, 0.f);

}




// void outsize(string s, int i) {
//   cout << "Size " << s << ": " << i << endl;
// }

Program::Program(string vs_file, string fs_file) {
	sh.v_file = vs_file;
	sh.f_file = fs_file;

	init_graphics();
}

Program::~Program() {
  // unbind VAO and VBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);


  // Delete the program and shaders
  glDeleteProgram(program);
  glDeleteShader(sh.vertex);
  glDeleteShader(sh.fragment);

  // Delete the VBOs
  glDeleteBuffers(1, &vbo);

  // Delete the VAOs
  glDeleteVertexArrays(1, &vao);
}




////
//// BEGIN INITIALIZATION FUNCTIONS

void Program::init_graphics() {
  // outsize("before shader init", draw_pt.size());
  init_shaders();
  // outsize("before attrib init", draw_pt.size());
  init_attributes();
  // outsize("before uni init", draw_pt.size());
  init_uniforms();
  // outsize("after uni init", draw_pt.size());
}

void Program::init_shaders() {
  const int buflength = 4000;
  char buf[buflength];

  // init vertex shader
  //cout << "compiling vertex shader" << endl;
  read_shader_source_code(sh.v_file.c_str(), buf, buflength);
  int vertex_shader_source_length = strlen(buf);
  const char *vertex_shader_sources[] = { buf };

  sh.vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(sh.vertex, 1, vertex_shader_sources, &vertex_shader_source_length);
  glCompileShader(sh.vertex);

  // cout<<buf<<endl;
  //check_shader_compile_error(sh.vertex);

  // init fragment shader
  //cout << "compiling fragment shader" << endl;
  read_shader_source_code(sh.f_file.c_str(), buf, buflength);
  int fragment_shader_source_length = strlen(buf);
  const char *fragment_shader_sources[] = { buf };

  sh.fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(sh.fragment, 1, fragment_shader_sources, &fragment_shader_source_length);
  glCompileShader(sh.fragment);
  //check_shader_compile_error(sh.fragment);

  // init program
  //cout << "linking program" << endl;
  program = glCreateProgram();
  glAttachShader(program, sh.vertex);
  glAttachShader(program, sh.fragment);

  //
  // glBindFragDataLocation(sh.fragment, 0, "frag_color");

  glLinkProgram(program);

  // enable depth test
  glEnable(GL_DEPTH_TEST);
  // glEnable(GL_MULTISAMPLE_ARB);


  // specify the shaders we want to use
  glUseProgram(program);

  // cout << "compiled..." <<endl;
}

void Program::init_attributes() {
  // create and bind a VAO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // get the input variable location in this shader
  position_location = glGetAttribLocation(program, "in_position");
  color_location    = glGetAttribLocation(program, "in_color");
  texCoord_location = glGetAttribLocation(program, "in_texCoord");

  // enable the input locations we want to use for this VAO
  glEnableVertexAttribArray(position_location);
  glEnableVertexAttribArray(color_location);
  glEnableVertexAttribArray(texCoord_location);



  // generate and bind a vertex buffer to hold the vertex data on GPU
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // specify the input data format for this VBO
  glVertexAttribPointer(position_location,  2, GL_FLOAT, GL_FALSE, sizeof(gl_obj::Vertex), (void*)offsetof(gl_obj::Vertex, pos));
  glVertexAttribPointer(color_location,     4, GL_FLOAT, GL_FALSE, sizeof(gl_obj::Vertex), (void*)offsetof(gl_obj::Vertex, color));
  glVertexAttribPointer(texCoord_location,  2, GL_FLOAT, GL_FALSE, sizeof(gl_obj::Vertex), (void*)offsetof(gl_obj::Vertex, texCoord));



  // unbind the VAO, we're done for now
  // glBindVertexArray(0);



  // for indices
  glGenBuffers(1, &vbo_indices);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
}

void Program::init_uniforms() {
  cout << "0 TL: " << translate_location << " RL: " << render_mode_location << endl;
  translate_location = glGetUniformLocation(program, "translate");

  cout << "1 TL: " << translate_location << " RL: " << render_mode_location << endl;
  render_mode_location = glGetUniformLocation(program, "render_mode");

  cout << "2 TL: " << translate_location << " RL: " << render_mode_location << endl;

  tex_sampler_location = glGetUniformLocation(program, "tex_sampler");
  glUniform1i(tex_sampler_location, 0);
  // cout << "3 TL: " << translate_location << " RL: " << render_mode_location << endl;
  // rand_trans = glGetUniformLocation(program, "rand_trans");

  // glUniform1i(rand_trans, 1);
}

//// END INITIALIZATION FUNCTIONS
////




////
//// BEGIN UNIFORM SETTERS

//// END UNIFORM SETTERS
////


void Program::check_for_GL_errors(const char* from) {
  GLenum err = glGetError();
  if (err) {
    cout << "(" << from << ") Had error type: ";
    switch(err) {
      case GL_INVALID_ENUM: cout << "GL_INVALID_ENUM" << endl; break;
      case GL_INVALID_VALUE: cout << "GL_INVALID_VALUE" << endl; break;
      case GL_INVALID_OPERATION: cout << "GL_INVALID_OPERATION" << endl; break;
      case GL_INVALID_FRAMEBUFFER_OPERATION: cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
      case GL_OUT_OF_MEMORY: cout << "GL_OUT_OF_MEMORY" << endl; break;
      default: cout << "Other" << endl;
    }
  }
}


void Program::draw_graph_vertices(
  vector<gl_obj::pos_vec> &p)
{
  float radius = 0.05;
  bool border = true;

  check_for_GL_errors("Program::draw_graph_vertices - 0");

  glUniform1i(render_mode_location, Render_Mode::basic2D);
  check_for_GL_errors("Program::draw_graph_vertices - 1");


  if (border) {
    // glLineWidth(10);


    gl_obj::Circle c_outline = gl_obj::Circle(radius, color::edge);
    gl_obj::VertexGroup *vg = &(c_outline.tf->vg);

    // initialize the vertex buffer with the vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
  check_for_GL_errors("Program::draw_graph_vertices - 2");
    // glBufferData(GL_ARRAY_BUFFER, vg.size() * sizeof(Vertex), &vg[0] , GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, vg->size() * sizeof(gl_obj::Vertex), &vg->at(0) , GL_STREAM_DRAW);
  check_for_GL_errors("Program::draw_graph_vertices - 3");

    for (int i = 0; i < p.size(); i++) {
      //define uniform
      glUniform2fv(translate_location, 1, (float*)&(p[i]));
  check_for_GL_errors("Program::draw_graph_vertices - 4");

      // starts from 1
      glDrawArrays(GL_LINE_STRIP, 1, vg->size()-1);
    }

    check_for_GL_errors("Program::draw_graph_vertices - border");
  }


  // TODO: Figure out transparency
  gl_obj::Circle c = gl_obj::Circle(radius, color::vert);
  gl_obj::VertexGroup *vg = &(c.tf->vg);

  // initialize the vertex buffer with the vertex data
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // glBufferData(GL_ARRAY_BUFFER, vg.size() * sizeof(Vertex), &vg[0] , GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, vg->size() * sizeof(gl_obj::Vertex), &vg->at(0) , GL_STREAM_DRAW);

  for (int i = 0; i < p.size(); i++) {
    //define uniform
    glUniform2fv(translate_location, 1, (float*)&(p[i]));

    glDrawArrays(GL_TRIANGLE_FAN, 0, vg->size());
  }

  check_for_GL_errors("Program::draw_graph_vertices");
}



// void Program::draw_graph_edges(
//   vector<gl_obj::pos_vec> &e)
// {
//   float thickness = 10.f;


//   // prepare edges
//   std::vector<gl_obj::Vertex> v;
//   for (int i = 0; i < e.size(); i++) {
//     v.push_back(gl_obj::Vertex(e[i], color::edge));
//   }

//   check_for_GL_errors("Program::draw_graph_edges - 0");

//   //define uniform
//   gl_obj::pos_vec no_t = gl_obj::pos_vec(0.f, 0.f);
//   glUniform3fv(translate_location, 1, (float*)&(no_t[0]));

//   check_for_GL_errors("Program::draw_graph_edges - 1");

//   // initialize the vertex buffer with the vertex data
//   glBindBuffer(GL_ARRAY_BUFFER, vbo);
//   // glBufferData(GL_ARRAY_BUFFER, vg.size() * sizeof(Vertex), &vg[0] , GL_STATIC_DRAW);
//   glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(gl_obj::Vertex), &v[0] , GL_STREAM_DRAW);

//   check_for_GL_errors("Program::draw_graph_edges - 2");

//   // specify some parameters
//   glLineWidth(thickness);
//   check_for_GL_errors("Program::draw_graph_edges - 3");

//   // draw points
//   glDrawArrays(GL_LINES, 0, v.size());

//   check_for_GL_errors("Program::draw_graph_edges - 4");
// }



// TODO: draw a line also, so width never goes below 1 pixel
void Program::draw_graph_edges(
  vector<gl_obj::pos_vec> &e)
{
  float thickness = 0.004f;
    check_for_GL_errors("Program::draw_graph_edges - 0");

  glUniform1i(render_mode_location, Render_Mode::basic2D);
    check_for_GL_errors("Program::draw_graph_edges - 1");

  // prepare edges
  for (int i = 0; i < e.size(); i+=2) {
    gl_obj::WideLine wl = gl_obj::WideLine(e[i], e[i+1], thickness, color::edge);
    gl_obj::VertexGroup *vg = &(wl.tf->vg);



    // initialize the vertex buffer with the vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glBufferData(GL_ARRAY_BUFFER, vg.size() * sizeof(Vertex), &vg[0] , GL_STATIC_DRAW);
    check_for_GL_errors("Program::draw_graph_edges - 2");
    glBufferData(GL_ARRAY_BUFFER, vg->size() * sizeof(gl_obj::Vertex), &vg->at(0), GL_STREAM_DRAW);
    check_for_GL_errors("Program::draw_graph_edges - 3");


    glUniform2fv(translate_location, 1, (float*)&(e[i]));
    check_for_GL_errors("Program::draw_graph_edges - 4");

    // draw points
    glDrawArrays(GL_TRIANGLE_FAN, 0, vg->size());

    check_for_GL_errors("Program::draw_graph_edges");
  }
}



// TODO: what is the first argument in testure in fs.glsl?
void Program::draw_letter(const GLfloat pos[4], const GLfloat tex_coord[4], GLuint tex_id) {
  check_for_GL_errors("Program::draw_letter - 0");
  // render_mode = 1 is for letters
  // glUniform1i(render_mode_location, Render_Mode::font);

  check_for_GL_errors("Program::draw_letter - 1");

  // set up textures
  glActiveTexture(GL_TEXTURE0);
  check_for_GL_errors("Program::draw_letter - 2.1");
  glBindTexture(GL_TEXTURE_RECTANGLE, tex_id);
  check_for_GL_errors("Program::draw_letter - 2.2");

  gl_obj::TexQuad letter(pos[1], pos[3], pos[2], pos[0],
    tex_coord[1], tex_coord[3], tex_coord[2], tex_coord[0]);
  gl_obj::VertexGroup *vg = &(letter.tf->vg);


  // initialize the vertex buffer with the vertex data
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // glBufferData(GL_ARRAY_BUFFER, vg.size() * sizeof(Vertex), &vg[0] , GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, vg->size() * sizeof(gl_obj::Vertex), &vg->at(0), GL_STREAM_DRAW);
  check_for_GL_errors("Program::draw_letter - 3");


  // gl_obj::pos_vec p = gl_obj::pos_vec(0.f, 0.f);
  // GLfloat p[2] = {0.f, 0.f};
  glUniform2f(translate_location, 0.f, 0.f);
  check_for_GL_errors("Program::draw_letter - 4");

  // draw points
  glDrawArrays(GL_TRIANGLE_FAN, 0, vg->size());

  check_for_GL_errors("Program::draw_letter");
}



//// END DRAWING FUNCTIONS
////




void Program::clearViewport() {
  // glClearColor(0.2f,0.3f,0.3f,1.0f);
  glClearColor(color::bg[0], color::bg[1], color::bg[2], color::bg[3]);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Program::flush() {
  glFlush();
}

glm::vec4 Program::colorAtPixel(int x, int y) {
  GLfloat data[4];
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, data);
  return glm::vec4(data[0], data[1], data[2], data[3]);// how do you convert the int to vec4?
}
