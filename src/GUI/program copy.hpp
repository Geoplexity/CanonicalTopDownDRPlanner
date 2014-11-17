#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <string>
#include <vector>


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


#include "vertex_classes.hpp"
#include "solids.hpp"


namespace RenderMode {
  const unsigned int WITH_LIGHTING = 0;
  const unsigned int WITHOUT_LIGHTING = 1;
}

class Program {
public:
  static const unsigned int MAX_LIGHTS = 10;

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

  void setUniform_light(unsigned int idx, Light l);
  void setUniform_renderMode(unsigned int rm);
  void setUniform_useTex(bool use);
  void setUniform_texSampler(unsigned int ts);
  void setUniform_VMMatrix(glm::mat4 VM);
  void setUniform_projectionMatrix(glm::mat4 p);
  void setUniform_numberOfLights(unsigned int nol);

  bool addLight(Light l);
  void setLights(std::vector<Light> l);
  // ViewMatrix is not actually a uniform. It's composed with the model matrix at draw time
  void setViewMatrix(glm::mat4 v);
  void setViewMatrix(glm::vec3 eye, glm::vec3 center, glm::vec3 up);

  //// END UNIFORM SETTERS
  ////




  ////
  //// BEGIN DRAWING FUNCTIONS

  // it should...
    // if (it wasn't already in the scene) add to scene; return true;
    // else return false;
  // but instead... it just returns true
  bool addToScene_points(VertexGroupObject *vgo, unsigned int thickness, unsigned int renderMode, bool colorPicking);
  bool addToScene_lines(VertexGroupObject *vgo, unsigned int thickness, unsigned int renderMode, bool colorPicking);
  bool addToScene_sn(Solid::SolidNode *sn, unsigned int renderMode, bool colorPicking);
  bool addToScene_sn(Solid::SolidNode *sn, unsigned int renderMode, bool colorPicking, GLuint texID);

  // if (it was in the scene) remove from scene; return true;
  // else return false
  bool removeFromScene_points(VertexGroupObject *vgo);
  bool removeFromScene_lines(VertexGroupObject *vgo);
  bool removeFromScene_sn(Solid::SolidNode *sn);

  void draw_scene();
  void draw_colorpicking_scene();

  //// END DRAWING FUNCTIONS
  ////


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

  struct int_loc {
    int val;
    GLuint loc;
  };

  struct mat4_loc {
    glm::mat4 val;
    GLuint loc;
  };

  struct Light_loc {
    Light l;

    glm::vec3 position_V;

    GLuint position_V_loc,
           diffuse_intensities_loc,
           specular_intensities_loc;
  };


  // program info
  GLuint program;
  shaders sh;


  // attribute locations
  GLuint vao;
  GLuint vbo;
  GLuint vbo_indices;
  GLuint position_location,
         normal_location,
         color_location,
         texCoord_location;


  //uniforms
  int_loc numberOfLights;
  int_loc rendermode;
  int_loc useTex;
  int_loc texSampler;

  mat4_loc projection;
  mat4_loc view; //not actually a uniform, location is useless
  mat4_loc viewModel;

  Light_loc lights[MAX_LIGHTS];

  //objects to draw
  struct sn_details {
    Solid::SolidNode *sn; // pointer to the solidnode
    unsigned int      rm; // rendermode
    bool              cp; // is there colorpicking?

    bool              tx; // is there a texture attached?
    GLuint            txID; // the texture id if it is attached
  };
  struct vgo_details {
    VertexGroupObject *vgo; // pointer to the vgo
    unsigned int       rm; // rendermode
    unsigned int       thickness; // how thick is the point/line?
    bool               cp; // is there colorpicking?
  };
  std::vector<sn_details>  draw_sn; //solidnodes
  std::vector<vgo_details> draw_ln; //lines
  std::vector<vgo_details> draw_pt; //points



  void draw_vertices(
    VertexGroup &vg,
    glm::mat4 model,
    GLuint typeOfDraw,
    unsigned int thickness,
    unsigned int renderMode,
    std::vector<unsigned int> *idx_arr);
  void draw_solid(Solid::Solid &s, unsigned int renderMode, glm::mat4 model);
  void draw_object_helper(Solid::SolidNode &sn, bool colorCoded, unsigned int renderMode, glm::mat4 model);
  void draw_object(Solid::SolidNode &sn, bool colorCoded, unsigned int renderMode);
};


#endif
