#ifndef CIRCLE_HPP
#define CIRCLE_HPP


#include "../../ext/glm/glm.hpp"

#define GLM_FORCE_RADIANS
#include "../../ext/glm/gtx/rotate_vector.hpp"

// #include <GLFW/glfw3.h>


#include <vector>
#include <cmath>
#include <iostream>


#define PI 3.14159265359

namespace gl_obj {
  typedef glm::vec2 pos_vec;
  typedef glm::vec4 color_vec;

  class Vertex {
  public:
    pos_vec pos;
    color_vec color;

    Vertex(pos_vec p, color_vec c) {
      pos = p; color = c;
    }

    ~Vertex() {}

    void setColor(color_vec c) {
      color = c;
    }
  };


  class VertexGroup : public std::vector<Vertex> {
  public:
    VertexGroup() {}

    void setAllColor(color_vec color) {
      for (int i = 0; i < this->size(); i++) {
        at(i).setColor(color);
      }
    }
  };



  class TriangleFan {
  public:
    // GLuint type;
    VertexGroup vg;

    // BEGIN: CONSTRUCTORS
    //////////////////////
    TriangleFan(Vertex hub, Vertex first, Vertex second) {
      // type = GL_TRIANGLE_FAN;
      vg.push_back(hub);
      vg.push_back(first);
      vg.push_back(second);
    }

    ~TriangleFan () {}
    //////////////////////
    // END:   CONSTRUCTORS



    void setAllColor(color_vec color) {
      vg.setAllColor(color);
    }

    void addVertex(Vertex v) {
      vg.push_back(v);
    }
  };



  // a polygon inscribed in a circle of given radius
  class Circle {
  public:
    TriangleFan *tf;

    float radius;
    color_vec color;
    // color_vec black = color_vec(0.f, 0.f, 0.f, 0.f);


    Circle(float radius, color_vec color) {
      this->radius = radius;
      this->color = color;


      unsigned int edges = 2000*radius;
      if (edges < 3) edges = 3;
      if (edges > 200) edges = 200;

      pos_vec vhub = pos_vec(0.f, 0.f);
      pos_vec v0 = t_2_xy(0.f);
      pos_vec v1 = t_2_xy(1.f/edges);
      tf = new TriangleFan(Vertex(vhub, color),
        Vertex(v0, color),
        Vertex(v1, color));

      for (int i = 2; i < edges; i++) {
        pos_vec vnew= t_2_xy(float(i)/edges);
        tf->addVertex(Vertex(vnew, color));
      }

      tf->addVertex(Vertex(v0, color));

    };
  private:
    pos_vec t_2_xy(float t) {
      return pos_vec(radius*sin(t*2*PI), radius*cos(t*2*PI));
    }
  };



  // a line with width
  class WideLine {
  public:
    TriangleFan *tf;

    float width;
    color_vec color;
    // color_vec black = color_vec(0.f, 0.f, 0.f, 0.f);


    WideLine(pos_vec from, pos_vec to, float width, color_vec color) {
      this->width = width;
      this->color = color;

      float wo2 = width/2.f;
      float length = glm::distance(from, to);
      pos_vec
        bl = pos_vec(0.f,    -wo2),
        br = pos_vec(0.f,     wo2),
        tl = pos_vec(length, -wo2),
        tr = pos_vec(length,  wo2);
      // add length to topleft and right. give width. rotate.

      float dx = (to.x - from.x);
      float dy = (to.y - from.y);
      float angle = atan(dy/dx); // gives [-pi/2, pi/2]
      if (dx < 0) angle += PI;

      // std::cout
      //   << "(" << from.x << ", " << from.y << ") "
      //   << "(" << to.x << ", " << to.y << ") "
      //   << "= " << angle
      //   << std::endl;
      bl = glm::rotate(bl, angle);
      br = glm::rotate(br, angle);
      tl = glm::rotate(tl, angle);
      tr = glm::rotate(tr, angle);

      // glm::mat2 rot = glm::mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
      // bl = rot * bl;
      // br = rot * br;
      // tl = rot * tl;
      // tr = rot * tr;

      // std::cout
      //   << "(" << bl.x << ", " << bl.y << ") "
      //   << "(" << br.x << ", " << br.y << ") "
      //   << "(" << tr.x << ", " << tr.y << ") "
      //   << "(" << tl.x << ", " << tl.y << ") "
      //   << std::endl;


      tf = new TriangleFan(Vertex(bl, color),
        Vertex(br, color),
        Vertex(tr, color));
      tf->addVertex(Vertex(tl, color));

    };
  };
}




#endif
