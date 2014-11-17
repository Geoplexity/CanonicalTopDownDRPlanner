#include "program.hpp"

#include <cstring>
#include <cstdio>

#include "../../ext/glm/gtc/matrix_transform.hpp"

#include "utils.h"


using namespace std;

// void outsize(string s, int i) {
//   cout << "Size " << s << ": " << i << endl;
// }

Program::Program(string vs_file, string fs_file) {
	sh.v_file = vs_file;
	sh.f_file = fs_file;

	// default uniforms
	projection.val = glm::mat4(1,0,0,0,
	                0,1,0,0,
	                0,0,1,0,
	                0,0,0,1);

	view.val = glm::mat4(1,0,0,0,
	                0,1,0,0,
	                0,0,1,0,
	                0,0,0,1);

	rendermode.val = RenderMode::WITH_LIGHTING;
	numberOfLights.val = 0;

  // outsize("in constructor", draw_pt.size());

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
  normal_location   = glGetAttribLocation(program, "in_normal");
  color_location    = glGetAttribLocation(program, "in_color");
  texCoord_location = glGetAttribLocation(program, "in_texCoord");

  // enable the input locations we want to use for this VAO
  glEnableVertexAttribArray(position_location);
  glEnableVertexAttribArray(normal_location);
  glEnableVertexAttribArray(color_location);
  glEnableVertexAttribArray(texCoord_location);



  // generate and bind a vertex buffer to hold the vertex data on GPU
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // specify the input data format for this VBO
  glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
  glVertexAttribPointer(normal_location,   3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
  glVertexAttribPointer(color_location,    4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
  glVertexAttribPointer(texCoord_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));



  // unbind the VAO, we're done for now
  // glBindVertexArray(0);



  // for indices
  glGenBuffers(1, &vbo_indices);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
}

void Program::init_uniforms() {
  // get uniform locations
  viewModel.loc = glGetUniformLocation(program, "VM_matrix");

  projection.loc = glGetUniformLocation(program, "P_matrix");

  // outsize("in uni init 1", draw_pt.size());

  for (int i = 0; i < MAX_LIGHTS; i++) {
    char buf[30];

    sprintf(buf, "lights[%d].position_V", i);
    lights[i].position_V_loc = glGetUniformLocation(program, buf);
    sprintf(buf, "lights[%d].diffuse_intensities", i);
    lights[i].diffuse_intensities_loc = glGetUniformLocation(program, buf);
    sprintf(buf, "lights[%d].specular_intensities", i);
    lights[i].specular_intensities_loc = glGetUniformLocation(program, buf);
  }

  // outsize("in uni init 1.5", draw_pt.size());
  numberOfLights.loc = glGetUniformLocation(program, "number_of_lights");

  rendermode.loc = glGetUniformLocation(program, "render_mode");

  useTex.loc = glGetUniformLocation(program, "useTex");

  texSampler.loc = glGetUniformLocation(program, "tex_sampler");

  // outsize("in uni init 2", draw_pt.size());
  // These are changed only when they want to be
  setUniform_projectionMatrix(projection.val);
  setUniform_numberOfLights(numberOfLights.val);
  for (int i = 0; i < numberOfLights.val; i++)
    setUniform_light(i, lights[i].l);
  setUniform_useTex(false);
  // setViewMatrix(view_mat);

  // These are changed at draw time
  // VM
  // setUniform_renderMode(rendermode);

  // outsize("in uni init 3", draw_pt.size());



  // cout << numberOfLights_loc << " " << numberOfLights << endl;
}

//// END INITIALIZATION FUNCTIONS
////




////
//// BEGIN UNIFORM SETTERS

void Program::setUniform_light(unsigned int idx, Light l) {
  lights[idx].l = l;
  // cout << idx << ": " << lights[idx].diffuse_intensities_loc << ": " << lights[idx].l.diffuse_intensities << endl;
  lights[idx].position_V = glm::vec3(view.val * glm::vec4(lights[idx].l.position_W, 1.f));

  glUniform3fv(lights[idx].position_V_loc,           1, &(lights[idx].position_V[0]));
  glUniform3fv(lights[idx].diffuse_intensities_loc,  1, &(lights[idx].l.diffuse_intensities[0]));
  glUniform3fv(lights[idx].specular_intensities_loc, 1, &(lights[idx].l.specular_intensities[0]));
}

void Program::setUniform_renderMode(unsigned int rm) {
  rendermode.val = rm;
  glUniform1i(rendermode.loc, rm);
}

void Program::setUniform_useTex(bool use) {
  useTex.val = (use)? 1 : 0;
  glUniform1i(useTex.loc, useTex.val);
}

void Program::setUniform_texSampler(unsigned int ts) {
  texSampler.val = ts;
  glUniform1i(texSampler.loc, texSampler.val);
}

void Program::setUniform_VMMatrix(glm::mat4 VM) {
  glUniformMatrix4fv(viewModel.loc, 1, GL_FALSE, &(VM[0][0]));
}

void Program::setUniform_projectionMatrix(glm::mat4 p) {
  projection.val = p;
  glUniformMatrix4fv(projection.loc, 1, GL_FALSE, &(projection.val[0][0]));
}

//should be private...
void Program::setUniform_numberOfLights(unsigned int nol) {
  numberOfLights.val = nol;
  glUniform1i(numberOfLights.loc, numberOfLights.val);
}



bool Program::addLight(Light l) {
  if (numberOfLights.val + 1 == MAX_LIGHTS) return false;

  setUniform_light(numberOfLights.val, l);
  setUniform_numberOfLights(numberOfLights.val + 1);

  // cout << numberOfLights_loc << " " << numberOfLights << endl;

  return true;
}

void Program::setLights(vector<Light> l) {
  setUniform_numberOfLights(0);
  for (int i = 0; i < l.size(); i++)
    addLight(l[i]);
}

// not actually a uniform. It's composed with the model matrix at draw time
void Program::setViewMatrix(glm::mat4 v) {
  view.val = v;

  // alters the light
  for (int i = 0; i < numberOfLights.val; i++) {
    lights[i].position_V = glm::vec3(view.val * glm::vec4(lights[i].l.position_W, 1.f));
    glUniform3fv(lights[i].position_V_loc, 1, &(lights[i].position_V[0]));
  }
}

// not actually a uniform. It's composed with the model matrix at draw time
void Program::setViewMatrix(glm::vec3 eye, glm::vec3 center, glm::vec3 up) {
  setViewMatrix(glm::lookAt(eye, center, up));
}

//// END UNIFORM SETTERS
////




////
//// BEGIN DRAWING FUNCTIONS

// int locateInScene(vector<sn_details> &v, sn_details *p) {
//   int size = v.size();
//   int pos=0;
//   while ((pos != size) && (v[pos].sn != p)) pos++;
//   return (pos==size)? -1 : pos;
// }
// int locateInScene(vector<sn_details> &v, sn_details *p) {
//   int size = v.size();
//   int pos=0;
//   while ((pos != size) && (v[pos].sn != p)) pos++;
//   return (pos==size)? -1 : pos;
// }

bool Program::addToScene_points(VertexGroupObject *vgo, unsigned int thickness, unsigned int renderMode, bool colorPicking) {
  if (vgo == NULL) return false;
  vgo_details temp;
  temp.vgo = vgo;
  temp.rm = renderMode;
  temp.thickness = thickness;
  temp.cp = colorPicking;
  draw_pt.push_back(temp);

  return true;
}

bool Program::addToScene_lines(VertexGroupObject *vgo, unsigned int thickness, unsigned int renderMode, bool colorPicking) {
  if (vgo == NULL) return false;
  vgo_details temp;
  temp.vgo = vgo;
  temp.rm = renderMode;
  temp.thickness = thickness;
  temp.cp = colorPicking;
  draw_ln.push_back(temp);

  return true;
}

bool Program::addToScene_sn(Solid::SolidNode *sn, unsigned int renderMode, bool colorPicking) {
  if (sn == NULL) return false;
  sn_details temp;
  temp.sn = sn;
  temp.rm = renderMode;
  temp.cp = colorPicking;
  temp.tx = false;
  draw_sn.push_back(temp);

  return true;
}

bool Program::addToScene_sn(Solid::SolidNode *sn, unsigned int renderMode, bool colorPicking, GLuint texID) {
  if (sn == NULL) return false;
  sn_details temp;
  temp.sn = sn;
  temp.rm = renderMode;
  temp.cp = colorPicking;
  temp.tx = true;
  temp.txID = texID;
  draw_sn.push_back(temp);

  return true;
}

bool Program::removeFromScene_points(VertexGroupObject *vgo) {
  // cout << "CALLED!!!" << endl;
  int size = draw_pt.size();
  int pos=0;

  while ((pos != size) && (draw_pt[pos].vgo != vgo)) pos++;

  if (pos == size) return false;
  else {
    draw_pt.erase(draw_pt.begin()+pos);
    return true;
  }
}

bool Program::removeFromScene_lines(VertexGroupObject *vgo) {
  int size = draw_ln.size();
  int pos=0;

  while ((pos != size) && (draw_ln[pos].vgo != vgo)) pos++;

  if (pos == size) return false;
  else {
    draw_ln.erase(draw_ln.begin()+pos);
    return true;
  }
}

bool Program::removeFromScene_sn(Solid::SolidNode *sn) {
  int size = draw_sn.size();
  int pos=0;

  while ((pos != size) && (draw_sn[pos].sn != sn)) pos++;

  if (pos == size) return false;
  else {
    draw_sn.erase(draw_sn.begin()+pos);
    return true;
  }
}


void Program::draw_vertices(
  VertexGroup &vg,
  glm::mat4 model,
  GLuint typeOfDraw,
  unsigned int thickness,
  unsigned int renderMode,
  vector<unsigned int> *idx_arr)
{

  //define uniform
  //calculate the modelView matrix (compose it as VM to multiply directly on vector)
  setUniform_VMMatrix(view.val * model);
  setUniform_renderMode(renderMode);

  // initialize the vertex buffer with the vertex data
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // glBufferData(GL_ARRAY_BUFFER, vg.size() * sizeof(Vertex), &vg[0] , GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, vg.size() * sizeof(Vertex), &vg[0] , GL_STREAM_DRAW);


  // specify some parameters
  if (typeOfDraw == GL_POINTS) {
    glPointSize(thickness);
  } else if (typeOfDraw == GL_LINE_STRIP or typeOfDraw == GL_LINES) {
    glLineWidth(thickness);
  }

  // draw points
  if (typeOfDraw == GL_POINTS or
    typeOfDraw == GL_LINES or
    typeOfDraw == GL_LINE_STRIP or
    typeOfDraw == GL_TRIANGLE_FAN)
  {
    glDrawArrays(typeOfDraw, 0, vg.size());
  }
  else if (typeOfDraw == GL_TRIANGLES or
    typeOfDraw == GL_TRIANGLE_STRIP)
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_arr->size() * sizeof(unsigned int), &(idx_arr->at(0)), GL_STREAM_DRAW);
    // glDrawElements(typeOfDraw, idx_arr->size(), GL_UNSIGNED_INT, &(idx_arr->at(0)));
    glDrawElements(typeOfDraw, idx_arr->size(), GL_UNSIGNED_INT, 0);
  }
  else {
    cout << "Don't know how to draw type: " << typeOfDraw << endl;
  }

  GLenum err = glGetError();
  if (err) {
    cout << "For draw type " << typeOfDraw << ", had error type: ";
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

void Program::draw_solid(Solid::Solid &s, unsigned int renderMode, glm::mat4 model) {
    // cout << "draw_solid: here 1 " << endl;
  Solid::Face::Basic *face = s.firstFace;

  while (face != NULL) {

    // cout << "drawing face" << endl;
    if (face->type == GL_TRIANGLES) {
      Solid::Face::Triangles *f = (Solid::Face::Triangles *)face;
      draw_vertices(f->vg, model, face->type, face->thickness, renderMode, &f->idx_arr);
    } else if (face->type == GL_TRIANGLE_FAN) {
      Solid::Face::TriangleFan *f = (Solid::Face::TriangleFan *)face;
      draw_vertices(f->vg, model, face->type, face->thickness, renderMode, NULL);
    } else if (face->type == GL_TRIANGLE_STRIP) {
      Solid::Face::TriangleStrip *f = (Solid::Face::TriangleStrip *)face;
      draw_vertices(f->vg, model, face->type, face->thickness, renderMode, &f->idx_arr);
    }

    face = face->sibling;
  }
}

void Program::draw_object_helper(Solid::SolidNode &sn, bool colorCoded, unsigned int renderMode, glm::mat4 model) {
  // cout<<"damn0 "<<sn.sibling() <<endl;
  if (sn.sibling() != NULL) {
    draw_object_helper(*sn.sibling(), colorCoded, renderMode, model);
  }
  // try {
  //   draw_object_helper(*sn.sibling(), colorCoded, renderMode, model);
  // } catch (...) {
  //   cout << "aint no sib" << endl;
  // }

  // cout<<"damn1"<<endl;

  model = model * sn.model()->matrix();

  // cout<<"damn2"<<endl;
  if (sn.child() != NULL) {
    draw_object_helper(*sn.child(), colorCoded, renderMode, model);
  }

  // cout<<"damn3"<<endl;
  if (colorCoded) {
    if (sn.self_colorCoded() != NULL) {
      draw_solid(*sn.self_colorCoded(), renderMode, model);
    }
  }
  else {
    // cout << "draw_object_helper: here 1.2" << endl;

    draw_solid(*sn.self(), renderMode, model);
  }
    // cout << "draw_object_helper: here 2" << endl;

}

void Program::draw_object(Solid::SolidNode &sn, bool colorCoded, unsigned int renderMode) {

  // cout << "drawing start" << &sn << colorCoded << renderMode << endl;
  draw_object_helper(sn, colorCoded, renderMode, glm::mat4(1.0f));
    // cout << "drawing end" << endl;
}

void Program::draw_scene() {
  for (int i = 0; i < draw_pt.size(); i++) {
    setUniform_useTex(false);
    draw_vertices(draw_pt[i].vgo->vg, draw_pt[i].vgo->m.matrix(), GL_POINTS, draw_pt[i].thickness, draw_pt[i].rm, NULL);
  }
  for (int i = 0; i < draw_ln.size(); i++) {
    setUniform_useTex(false);
    draw_vertices(draw_ln[i].vgo->vg, draw_ln[i].vgo->m.matrix(), GL_LINES, draw_ln[i].thickness, draw_ln[i].rm, NULL);
  }
  for (int i = 0; i < draw_sn.size(); i++) {
    if  (draw_sn[i].tx) {
      setUniform_useTex(true);

      // set up textures
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, draw_sn[i].txID);
      setUniform_texSampler(0);
    } else {
      setUniform_useTex(false);
    }

    draw_object(*draw_sn[i].sn, false, draw_sn[i].rm);
  }
}

void Program::draw_colorpicking_scene() {
  setUniform_useTex(false);

  // cout << "drawing cp scene" << endl;
  for (int i = 0; i < draw_pt.size(); i++) {
    // cout << "point " << i << " is " << draw_pt[i].cp << endl;
    if (draw_pt[i].cp) {
      // cout << "im really drawing now..." << draw_pt[i].vgo->ccvg->at(0).color[2] << endl;
      draw_vertices(*draw_pt[i].vgo->ccvg, draw_pt[i].vgo->m.matrix(), GL_POINTS, draw_pt[i].thickness, RenderMode::WITHOUT_LIGHTING, NULL);
    }
  }
  for (int i = 0; i < draw_ln.size(); i++) {
    if (draw_ln[i].cp) {
      draw_vertices(*draw_ln[i].vgo->ccvg, draw_ln[i].vgo->m.matrix(), GL_LINES, draw_ln[i].thickness, RenderMode::WITHOUT_LIGHTING, NULL);
    }
  }
  for (int i = 0; i < draw_sn.size(); i++) {
    if (draw_sn[i].cp) {
      draw_object(*draw_sn[i].sn, true, RenderMode::WITHOUT_LIGHTING);
    }
  }
}

//// END DRAWING FUNCTIONS
////




void Program::clearViewport() {
  // glClearColor(0.2f,0.3f,0.3f,1.0f);
  glClearColor(0.0f,0.0f,0.0f,1.0f);
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
