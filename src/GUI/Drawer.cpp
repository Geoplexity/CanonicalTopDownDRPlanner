#include "Drawer.hpp"

#include "easy_font.hpp"

#include "../../ext/glm/gtc/matrix_transform.hpp"

// #include <cstring>
// #include <cstdio>

#include <iostream>



namespace Render_Mode {
    GLuint
        basic2D = 0,
        font = 1;
}


namespace parameters {
    const float vertex_radius_base = 0.05;
    const float vertex_radius_highlight = vertex_radius_base*1.5;
    const float edge_width = 0.004f, edge_width_highlight = edge_width*3;
    const float edge_dash_length = 0.015, edge_undash_length = 0.025;
    const float base_text_height = vertex_radius_base*0.9;
}


namespace color {
    // offwhite/green/black
    // const gl_obj::color_vec bg   = gl_obj::color_vec(241/255.f, 242/255.f, 228/255.f, 1.f);
    // const gl_obj::color_vec vert = gl_obj::color_vec(104/255.f, 129/255.f, 80/255.f, 0.f);
    // const gl_obj::color_vec edge = gl_obj::color_vec(0.f, 0.f, 0.f, 0.f);

    // const gl_obj::color_vec vert = gl_obj::color_vec(0.6f, 0.2f, 0.6f, 0.f);

    // gray/green/black
    const gl_obj::color_vec bg   = gl_obj::color_vec(0.85f, 0.85f, 0.85f, 1.f);
    const gl_obj::color_vec vert = gl_obj::color_vec(0.6f, 0.2f, 0.2f, 0.f);
    const gl_obj::color_vec h_vert = gl_obj::color_vec(0.2f, 0.6f, 0.2f, 0.f);
    const gl_obj::color_vec edge = gl_obj::color_vec(0.f, 0.f, 0.f, 0.f);
    const gl_obj::color_vec h_edge = gl_obj::color_vec(0.f, 0.f, 0.f, 0.f);
    const gl_obj::color_vec text = gl_obj::color_vec(0.f, 0.f, 1.f, 0.f);

    // black/green/white
    // const gl_obj::color_vec bg   = gl_obj::color_vec(0.f, 0.f, 0.f, 1.f);
    // const gl_obj::color_vec vert = gl_obj::color_vec(0.1f, 1.0f, 0.1f, 0.f);
    // const gl_obj::color_vec edge = gl_obj::color_vec(1.f, 1.f, 1.f, 0.f);

    // gray/blue/black
    // const gl_obj::color_vec bg   = gl_obj::color_vec(0.85f, 0.85f, 0.85f, 1.f);
    // const gl_obj::color_vec vert = gl_obj::color_vec(0.f, 0.f, 1.f, 0.f);
    // const gl_obj::color_vec edge = gl_obj::color_vec(0.f, 0.f, 0.f, 0.f);

}





// void outsize(std::string s, int i) {
//     std::cout << "Size " << s << ": " << i << std::endl;
// }



Drawer::Drawer(std::string vs_file, std::string fs_file) {
    // swallow error from glewInit
    glGetError();


    check_for_GL_errors("Drawer::Drawer: 0");
    init_basic_2d_shaders();
    check_for_GL_errors("Drawer::Drawer: 1");
    init_attributes();
    check_for_GL_errors("Drawer::Drawer: 2");


    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Enable stencil operations
    glEnable(GL_STENCIL_TEST);
    // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    check_for_GL_errors("Drawer::Drawer: 3");
}

Drawer::~Drawer() {
    // unbind VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // Delete the program and shaders
    shader_program.un_use();

    // Delete the VBOs
    glDeleteBuffers(1, &vbo);

    // Delete the VAOs
    glDeleteVertexArrays(1, &vao);
}




////
//// BEGIN INITIALIZATION FUNCTIONS

// void Drawer::init_shaders(const char* vs_file_name, const char* fs_file_name) {
//     // compile, attach, link
//     // shader_program.compile_and_attach_vertex_shader(vs_file_name);
//     // shader_program.compile_and_attach_fragment_shader(fs_file_name);
//     shader_program.compile_and_attach_vertex_shader("src/GUI/shaders/vs.glsl");
//     shader_program.compile_and_attach_fragment_shader("src/GUI/shaders/fs.glsl");
//     shader_program.link();

//     // get the input attribute locations in this shader
//     position_location = shader_program.add_attribute("in_position");
//     color_location    = shader_program.add_attribute("in_color");
//     texCoord_location = shader_program.add_attribute("in_texCoord");

//     // get the input uniform locations in this shader
//     translate_location          = shader_program.add_uniform("translate");
//     render_mode_location        = shader_program.add_uniform("render_mode");
//     tex_sampler_location        = shader_program.add_uniform("tex_sampler");
//     _view_matrix.location       = shader_program.add_uniform("view_matrix");
//     _projection_matrix.location = shader_program.add_uniform("projection_matrix");

//     // use the shader
//     shader_program.use();

//     // init unchanging uniform
//     glUniform1i(tex_sampler_location, 0);
// }

// void Drawer::init_attributes() {
//     // create and bind a VAO
//     glGenVertexArrays(1, &vao);
//     glBindVertexArray(vao);

//     // enable the input locations we want to use for this VAO
//     glEnableVertexAttribArray(position_location);
//     glEnableVertexAttribArray(color_location);
//     glEnableVertexAttribArray(texCoord_location);



//     // generate and bind a vertex buffer to hold the vertex data on GPU
//     glGenBuffers(1, &vbo);
//     glBindBuffer(GL_ARRAY_BUFFER, vbo);

//     // specify the input data format for this VBO
//     glVertexAttribPointer(position_location,  2, GL_FLOAT, GL_FALSE, sizeof(gl_obj::Vertex), (void*)offsetof(gl_obj::Vertex, _pos));
//     glVertexAttribPointer(color_location,     4, GL_FLOAT, GL_FALSE, sizeof(gl_obj::Vertex), (void*)offsetof(gl_obj::Vertex, _color));
//     glVertexAttribPointer(texCoord_location,  2, GL_FLOAT, GL_FALSE, sizeof(gl_obj::Vertex), (void*)offsetof(gl_obj::Vertex, _tex_coord));



//     // unbind the VAO, we're done for now
//     // glBindVertexArray(0);



//     // for indices
//     glGenBuffers(1, &vbo_indices);
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
// }

void Drawer::init_basic_2d_shaders() {
    // compile, attach, link
    shader_program.compile_and_attach_vertex_shader("src/GUI/shaders/basic_2d.vs.glsl");
    shader_program.compile_and_attach_fragment_shader("src/GUI/shaders/basic_2d.fs.glsl");
    shader_program.link();

    // // get the input attribute locations in this shader
    // position_location = shader_program.add_attribute("in_position");

    // // get the input uniform locations in this shader
    // color_location              = shader_program.add_uniform("color");
    // _model_matrix.location      = shader_program.add_uniform("model_matrix");
    // _view_matrix.location       = shader_program.add_uniform("view_matrix");
    // _projection_matrix.location = shader_program.add_uniform("projection_matrix");

    // get the input attribute locations in this shader
    shader_program.add_attribute("in_position");

    // get the input uniform locations in this shader
    shader_program.add_uniform("projection_view_matrix");
    shader_program.add_uniform("model_matrix");
    shader_program.add_uniform("color");

    // use the shader
    shader_program.use();
}

void Drawer::init_attributes() {
    // create and bind a VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint position_location = shader_program.get_attribute("in_position");

    // enable the input locations we want to use for this VAO
    glEnableVertexAttribArray(position_location);



    // generate and bind a vertex buffer to hold the vertex data on GPU
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // specify the input data format for this VBO
    glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, sizeof(gl_obj::Vertex), (void*)offsetof(gl_obj::Vertex, _pos));

    // unbind the VAO, we're done for now
    // glBindVertexArray(0);



    // for indices
    glGenBuffers(1, &vbo_indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
}


//// END INITIALIZATION FUNCTIONS
////




////
//// BEGIN UNIFORM SETTERS
// void Drawer::set_uniform__view_matrix(glm::mat4 view_matrix) {
//     // _view_matrix.uniform = view_matrix;
//     // cout << "setUniform_ViewMatrix: 0" << endl;
//     glUniformMatrix4fv(_view_matrix.location, 1, GL_FALSE, &(view_matrix[0][0]));
//     // cout << "setUniform_ViewMatrix: 1" << endl;
// }

// void Drawer::set_uniform__projection_matrix(glm::mat4 projection_matrix) {
//     // for (int i = 0; i < 4; i++) {
//     //     for (int j = 0; j < 4; j++) {
//     //         cout << projection_matrix[i][j] << " ";
//     //     }
//     //     cout << endl;
//     // }
//     glUniformMatrix4fv(_projection_matrix.location, 1, GL_FALSE, &(projection_matrix[0][0]));
// }

void Drawer::set_uniform__projection_view_matrix(glm::mat4 projection_view_matrix) {
    glUniformMatrix4fv(
        shader_program.get_uniform("projection_view_matrix"),
        1, GL_FALSE, &(projection_view_matrix[0][0]));
}

void Drawer::set_uniform__model_matrix(glm::mat4 model_matrix) {
    glUniformMatrix4fv(
        shader_program.get_uniform("model_matrix"),
        1, GL_FALSE, &(model_matrix[0][0]));
}

void Drawer::set_uniform__color(glm::vec4 color) {
    glUniform4fv(
        shader_program.get_uniform("color"),
        1, &(color[0]));
}

//// END UNIFORM SETTERS
////




void Drawer::_draw_graph_vertices_aux(
    const std::vector<gl_obj::pos_vec> &p,
    float radius,
    gl_obj::color_vec color,
    bool border)
{
    check_for_GL_errors("Drawer::draw_graph_vertices - 0");

    // shader_program.use();
    // glBindBuffer(GL_ARRAY_BUFFER, vbo);


    // TODO: Figure out transparency

    // Make the circle
    gl_obj::Circle c = gl_obj::Circle(radius, color);
    // initialize the vertex buffer with the vertex data
    glBufferData(GL_ARRAY_BUFFER, c.size() * sizeof(gl_obj::Vertex), &c[0], GL_STREAM_DRAW);

    // set the color of the circles
    set_uniform__color(color);

    // Always want to replace stencil value with index
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    for (unsigned int i = 0; i < p.size(); i++) {
        glm::mat4 trans = translation_matrix(p[i]);
        set_uniform__model_matrix(trans);

        // set the stencil to vertex + 1 (bg is 0), subtract later
        glStencilFunc(GL_ALWAYS, i + 1, -1);

        glDrawArrays(GL_TRIANGLE_FAN, 0, c.size());
    }
    // no more setting stencil
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    check_for_GL_errors("Drawer::draw_graph_vertices");


    if (border) {
        // set the color of the borders
        set_uniform__color(color::edge);

        glm::mat4 scale = scale_matrix(1.05);

        for (int i = 0; i < p.size(); i++) {
            glm::mat4 trans = translation_matrix(p[i]);
            set_uniform__model_matrix(trans * scale);

            glDrawArrays(GL_TRIANGLE_FAN, 1, c.size());
        }

        check_for_GL_errors("Drawer::draw_graph_vertices - border");
    }
}


void Drawer::draw_graph_vertices(
    const std::vector<gl_obj::pos_vec> &p,
    const std::vector<gl_obj::pos_vec> &highlight)
{
    _draw_graph_vertices_aux(highlight, parameters::vertex_radius_highlight, color::h_vert, true);
    _draw_graph_vertices_aux(p, parameters::vertex_radius_base, color::vert, true);
}



// void Drawer::draw_graph_edges(
//     vector<gl_obj::pos_vec> &e)
// {
//     float parameters::edge_width = 10.f;


//     // prepare edges
//     std::vector<gl_obj::Vertex> v;
//     for (int i = 0; i < e.size(); i++) {
//         v.push_back(gl_obj::Vertex(e[i], color::edge));
//     }

//     check_for_GL_errors("Drawer::draw_graph_edges - 0");

//     //define uniform
//     gl_obj::pos_vec no_t = gl_obj::pos_vec(0.f, 0.f);
//     glUniform3fv(translate_location, 1, (float*)&(no_t[0]));

//     check_for_GL_errors("Drawer::draw_graph_edges - 1");

//     // initialize the vertex buffer with the vertex data
//     glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     // glBufferData(GL_ARRAY_BUFFER, vg.size() * sizeof(Vertex), &vg[0] , GL_STATIC_DRAW);
//     glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(gl_obj::Vertex), &v[0] , GL_STREAM_DRAW);

//     check_for_GL_errors("Drawer::draw_graph_edges - 2");

//     // specify some parameters
//     glLineWidth(parameters::edge_width);
//     check_for_GL_errors("Drawer::draw_graph_edges - 3");

//     // draw points
//     glDrawArrays(GL_LINES, 0, v.size());

//     check_for_GL_errors("Drawer::draw_graph_edges - 4");
// }


void Drawer::_draw_graph_edges_aux(
    const std::vector<gl_obj::pos_vec> &e,
    float width,
    gl_obj::color_vec color)
{
    check_for_GL_errors("Drawer::draw_graph_edges - 0");

    // TODO: only draw once, then scale and rotate
    // // make the line
    // gl_obj::Wide_Line wl = gl_obj::Wide_Line(
    //     glm::vec2(0, 0), glm::vec2(0, 1), width, color);

    // set color
    set_uniform__color(color);

    // prepare edges
    for (int i = 0; i < e.size(); i+=2) {
        // make the line
        gl_obj::Wide_Line wl = gl_obj::Wide_Line(e[i], e[i+1], width, color);

        // initialize the vertex buffer with the vertex data
        glBufferData(GL_ARRAY_BUFFER, wl.size() * sizeof(gl_obj::Vertex), &wl[0], GL_STREAM_DRAW);
        check_for_GL_errors("Drawer::draw_graph_edges - 1");

        // translate
        set_uniform__model_matrix(translation_matrix(e[i]));
        check_for_GL_errors("Drawer::draw_graph_edges - 2");

        // draw points
        glDrawArrays(GL_TRIANGLE_FAN, 0, wl.size());
        check_for_GL_errors("Drawer::draw_graph_edges - 3");
    }
}


// TODO: draw a line also, so width never goes below 1 pixel
void Drawer::draw_graph_edges(
    const std::vector<gl_obj::pos_vec> &e,
    const std::vector<gl_obj::pos_vec> &highlight,
    const std::vector<gl_obj::pos_vec> &dashed)
{
    _draw_graph_edges_aux(
        e,
        parameters::edge_width,
        color::edge);
    _draw_graph_edges_aux(
        highlight,
        parameters::edge_width_highlight,
        color::h_edge);
    _draw_graph_edges_dashed_aux(
        dashed,
        parameters::edge_width,
        parameters::edge_dash_length,
        parameters::edge_undash_length,
        color::edge);
}


void Drawer::_draw_graph_edges_dashed_aux(
    const std::vector<gl_obj::pos_vec> &e,
    float width, float dash_length, float undash_length,
    gl_obj::color_vec color)
{
    check_for_GL_errors("Drawer::draw_graph_edges_dashed - 0");

    // set color
    set_uniform__color(color);

    // prepare edges
    for (int i = 0; i < e.size(); i+=2) {
        // make the line
        // TODO: try to minimize the number of times it's prepared
        gl_obj::Dashed_Wide_Line dwl = gl_obj::Dashed_Wide_Line(e[i], e[i+1], width, dash_length, undash_length, color);

        // initialize the vertex buffer with the vertex data
        glBufferData(GL_ARRAY_BUFFER, dwl.size() * sizeof(gl_obj::Vertex), &dwl[0], GL_STREAM_DRAW);
        check_for_GL_errors("Drawer::draw_graph_edges_dashed - 1");

        // translate
        set_uniform__model_matrix(translation_matrix(e[i]));
        check_for_GL_errors("Drawer::draw_graph_edges_dashed - 2");

        // draw points
        glDrawArrays(GL_TRIANGLES, 0, dwl.size());
        check_for_GL_errors("Drawer::draw_graph_edges_dashed - 3");
    }
}



void Drawer::_draw_graph_vertices_names_aux(
    const std::vector<std::string> &p_names,
    const std::vector<gl_obj::pos_vec> &p,
    float radius,
    gl_obj::color_vec color)
{
    // set color
    set_uniform__color(color);

    // initial translation
    glm::mat4 trans = translation_matrix(radius * glm::vec2(1.1, 1.5));

    // draw letters
    for (int i = 0; i < p_names.size(); i++) {
        // TODO: only prepare a letter once
        gl_obj::Vertex_Array vg = stb_easy_font_print(
            p_names[i].c_str(),
            color,
            parameters::base_text_height,
            parameters::base_text_height/6.f);

        check_for_GL_errors("Drawer::draw_vertex_names - 0");

        // initialize the vertex buffer with the vertex data
        glBufferData(GL_ARRAY_BUFFER, vg.size() * sizeof(gl_obj::Vertex), &vg[0], GL_STREAM_DRAW);
        check_for_GL_errors("Drawer::draw_vertex_names - 1");

        // translate
        set_uniform__model_matrix(translation_matrix(p[i]) * trans);
        check_for_GL_errors("Drawer::draw_vertex_names - 2");

        // draw points
        glDrawArrays(GL_TRIANGLES, 0, vg.size());
        check_for_GL_errors("Drawer::draw_vertex_names - 3");
    }
}

void Drawer::draw_graph_vertices_names(
    const std::vector<std::string> &p_names,
    const std::vector<gl_obj::pos_vec> &p,
    const std::vector<std::string> &highlight_names,
    const std::vector<gl_obj::pos_vec> &highlight)
{
    _draw_graph_vertices_names_aux(highlight_names, highlight, parameters::vertex_radius_highlight, color::text);
    _draw_graph_vertices_names_aux(p_names, p, parameters::vertex_radius_base, color::text);
}


// void Drawer::draw_easy_font(
//     gl_obj::Vertex_Array &vg)
// {
//     check_for_GL_errors("Drawer::draw_graph_edges - 0");

//     glUniform1i(render_mode_location, Render_Mode::basic2D);
//     check_for_GL_errors("Drawer::draw_graph_edges - 1");

//     // initialize the vertex buffer with the vertex data
//     glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     // glBufferData(GL_ARRAY_BUFFER, vg.size() * sizeof(Vertex), &vg[0] , GL_STATIC_DRAW);
//     check_for_GL_errors("Drawer::draw_graph_edges - 2");
//     glBufferData(GL_ARRAY_BUFFER, vg.size() * sizeof(gl_obj::Vertex), &vg[0], GL_STREAM_DRAW);
//     check_for_GL_errors("Drawer::draw_graph_edges - 3");


//     glm::vec2 trans(0.f, 0.f);
//     glUniform2fv(translate_location, 1, &(trans[0]));
//     check_for_GL_errors("Drawer::draw_graph_edges - 4");

//     // draw points
//     glDrawArrays(GL_TRIANGLES, 0, vg.size());

//     check_for_GL_errors("Drawer::draw_graph_edges");

// }



// // TODO: what is the first argument in testure in fs.glsl?
// void Drawer::draw_letter(const GLfloat pos[4], const GLfloat tex_coord[4], GLuint tex_id) {
//     check_for_GL_errors("Drawer::draw_letter - 0");
//     // render_mode = 1 is for letters
//     // glUniform1i(render_mode_location, Render_Mode::font);

//     check_for_GL_errors("Drawer::draw_letter - 1");

//     // set up textures
//     glActiveTexture(GL_TEXTURE0);
//     check_for_GL_errors("Drawer::draw_letter - 2.1");
//     glBindTexture(GL_TEXTURE_RECTANGLE, tex_id);
//     check_for_GL_errors("Drawer::draw_letter - 2.2");

//     gl_obj::Tex_Quad letter(pos[1], pos[3], pos[2], pos[0],
//     tex_coord[1], tex_coord[3], tex_coord[2], tex_coord[0]);
//     gl_obj::Vertex_Array *vg = &(letter);


//     // initialize the vertex buffer with the vertex data
//     glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     // glBufferData(GL_ARRAY_BUFFER, vg.size() * sizeof(Vertex), &vg[0] , GL_STATIC_DRAW);
//     glBufferData(GL_ARRAY_BUFFER, vg->size() * sizeof(gl_obj::Vertex), &vg->at(0), GL_STREAM_DRAW);
//     check_for_GL_errors("Drawer::draw_letter - 3");


//     // gl_obj::pos_vec p = gl_obj::pos_vec(0.f, 0.f);
//     // GLfloat p[2] = {0.f, 0.f};
//     glUniform2f(translate_location, 0.f, 0.f);
//     check_for_GL_errors("Drawer::draw_letter - 4");

//     // draw points
//     glDrawArrays(GL_TRIANGLE_FAN, 0, vg->size());

//     check_for_GL_errors("Drawer::draw_letter");
// }



//// END DRAWING FUNCTIONS
////




glm::mat4 Drawer::translation_matrix(const glm::vec2& translation) {
    return glm::translate(glm::mat4(), glm::vec3(translation, 0.f));
}

glm::mat4 Drawer::scale_matrix(const float scaling) {
    return glm::scale(glm::mat4(), glm::vec3(scaling, scaling, 0.f));
}

void Drawer::clear_viewport(int width, int height) {
    glViewport(0, 0, width, height);

    // // glClearColor(0.2f,0.3f,0.3f,1.0f);
    // glClearColor(color::bg[0], color::bg[1], color::bg[2], color::bg[3]);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glClearColor(color::bg[0], color::bg[1], color::bg[2], color::bg[3]);
    glClearStencil(0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
}

void Drawer::flush() {
    glFlush();
}

glm::vec4 Drawer::color_at_pixel(int x, int y) {
    GLfloat data[4];
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, data);
    return glm::vec4(data[0], data[1], data[2], data[3]);// how do you convert the int to vec4?
}

int Drawer::stencil_at_pixel(int x, int y) {
    GLuint ret;
    glReadPixels(x, y, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &ret);
    return ((int) ret) - 1;
}
