#ifndef GL_OBJECTS_HPP
#define GL_OBJECTS_HPP


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
    typedef glm::vec2 texc_vec;

    class Vertex {
    public:
        pos_vec   _pos;
        color_vec _color;
        texc_vec  _tex_coord;

        Vertex(pos_vec p, color_vec c) :
            _pos(p),
            _color(c),
            _tex_coord(texc_vec(0.f, 0.f))
        { }

        Vertex(pos_vec p, texc_vec t) :
            _pos(p),
            _color(color_vec(0.f,0.f,0.f,0.f)),
            _tex_coord(t)
        { }

        ~Vertex() {}

        void color(const color_vec &c) {_color = c;}
    };

    // class Vertex {
    // public:
    //     pos_vec _pos;

    //     Vertex(pos_vec p) :
    //         _pos(p)
    //     { }

    //     ~Vertex() {}
    // };


    class Vertex_Array : public std::vector<Vertex> {
    public:
        Vertex_Array() {}

        void add_vertex(Vertex v) {
            push_back(v);
        }

        void set_color_all(color_vec color) {
            for (int i = 0; i < this->size(); i++) {
                at(i).color(color);
            }
        }
    };

    class Triangles : public Vertex_Array {
    public:
        // BEGIN: CONSTRUCTORS
        //////////////////////
        Triangles() {}

        ~Triangles () {}
        //////////////////////
        // END:   CONSTRUCTORS

        void add_triangle(Vertex v0, Vertex v1, Vertex v2) {
            add_vertex(v0); add_vertex(v1); add_vertex(v2);
        }
    };

    class Triangle_Fan : public Vertex_Array {
    public:
        // BEGIN: CONSTRUCTORS
        //////////////////////
        Triangle_Fan(Vertex hub, Vertex first, Vertex second) {
            // type = GL_TRIANGLE_FAN;
            add_vertex(hub);
            add_vertex(first);
            add_vertex(second);
        }

        ~Triangle_Fan () {}
        //////////////////////
        // END:   CONSTRUCTORS
    protected:
        Triangle_Fan() {}
    };



    // a polygon inscribed in a circle of given radius
    class Circle : public Triangle_Fan {
    public:
        static const unsigned int edges_to_radius_ratio = 2000;
        static const unsigned int edges_max = 3;
        static const unsigned int edges_min = 200;

        float radius;
        color_vec color;

        Circle(float radius, color_vec color) :
            radius(radius),
            color(color)
        {
            unsigned int edges = edges_to_radius_ratio*radius;
            if (edges < edges_max) edges = edges_max;
            if (edges > edges_min) edges = edges_min;

            pos_vec vhub = pos_vec(0.f, 0.f);
            pos_vec v0 = t_2_xy(0.f);
            pos_vec v1 = t_2_xy(1.f/edges);

            add_vertex(Vertex(vhub, color));
            add_vertex(Vertex(v0, color));
            add_vertex(Vertex(v1, color));

            for (int i = 2; i < edges; i++) {
                pos_vec vnew= t_2_xy(float(i)/edges);
                add_vertex(Vertex(vnew, color));
            }

            add_vertex(Vertex(v0, color));
        };
    private:
        pos_vec t_2_xy(float t) {
            return pos_vec(radius*sin(t*2*PI), radius*cos(t*2*PI));
        }
    };



    // a line with width
    class Wide_Line : public Triangle_Fan {
    public:
        float width;
        color_vec color;

        Wide_Line(pos_vec from, pos_vec to, float width, color_vec color) :
            width(width),
            color(color)
        {
            float wo2 = width/2.f;
            float _distance = distance(from, to);
            float _angle = angle(from, to);

            pos_vec
                bl = pos_vec(0.f,    -wo2),
                br = pos_vec(0.f,     wo2),
                tl = pos_vec(_distance, -wo2),
                tr = pos_vec(_distance,  wo2);
            // add _distance to topleft and right. give width. rotate.

            bl = glm::rotate(bl, _angle);
            br = glm::rotate(br, _angle);
            tl = glm::rotate(tl, _angle);
            tr = glm::rotate(tr, _angle);

            add_vertex(Vertex(bl, color));
            add_vertex(Vertex(br, color));
            add_vertex(Vertex(tr, color));
            add_vertex(Vertex(tl, color));
        };

        static float distance(const pos_vec& a, const pos_vec& b) {
            return glm::distance(a, b);
        }

        // gives [0, 2*pi]
        static float angle(const pos_vec& a, const pos_vec& b) {
            float dx = (b.x - a.x);
            float dy = (b.y - a.y);
            float _angle = atan(dy/dx); // gives [-pi/2, pi/2]
            if (dx < 0) _angle += PI;
            return _angle;
        }
    };


    // A dashed line with width
    class Dashed_Wide_Line : public Triangles {
    public:
        float width, dash_length, undash_length;
        color_vec color;

        Dashed_Wide_Line(
            pos_vec from, pos_vec to,
            float width, float dash_length, float undash_length,
            color_vec color) :
                width(width),
                dash_length(dash_length),
                undash_length(undash_length),
                color(color)
        {
            float wo2 = width/2.f;
            float length = Wide_Line::distance(from, to);
            float _angle = Wide_Line::angle(from, to);
            // add length to topleft and right. give width. rotate.

            float begin, end;
            bool dash = true;
            for (begin = 0.f; begin < length; begin = end) {
                end = begin + (dash? dash_length: undash_length);
                dash = !dash;

                // check if dash, since we just negated
                if (dash) continue;

                if (end > length) end = length;

                pos_vec
                    bl = pos_vec(begin, -wo2),
                    br = pos_vec(begin,  wo2),
                    tl = pos_vec(end,   -wo2),
                    tr = pos_vec(end,    wo2);

                bl = glm::rotate(bl, _angle);
                br = glm::rotate(br, _angle);
                tl = glm::rotate(tl, _angle);
                tr = glm::rotate(tr, _angle);

                add_triangle(
                    Vertex(bl, color),
                    Vertex(br, color),
                    Vertex(tl, color));

                add_triangle(
                    Vertex(br, color),
                    Vertex(tr, color),
                    Vertex(tl, color));
            }
        };
    };



    // a quad with tex_coords
    // print at 0, do not translate
    class Tex_Quad : public Triangle_Fan {
    public:
        Tex_Quad(
            float pos_l, float pos_r, float pos_t, float pos_b,
            float texc_l, float texc_r, float texc_t, float texc_b)
        {
            pos_vec
                pbl = pos_vec(pos_l, pos_b),
                pbr = pos_vec(pos_r, pos_b),
                ptl = pos_vec(pos_l, pos_t),
                ptr = pos_vec(pos_r, pos_t);
            texc_vec
                tbl = texc_vec(texc_l, texc_b),
                tbr = texc_vec(texc_r, texc_b),
                ttl = texc_vec(texc_l, texc_t),
                ttr = texc_vec(texc_r, texc_t);

            add_vertex(Vertex(pbl, tbl));
            add_vertex(Vertex(pbr, tbr));
            add_vertex(Vertex(ptr, ttr));
            add_vertex(Vertex(ptl, ttl));

        };
    };
}




#endif
