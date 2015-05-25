


// `freetype-config --libs`
#include "GUI/Font.hpp"



void do_font_stuff() {
    char *ttf_file = "src/resources/OpenSans-Regular.ttf";


    // Create a library
    Font::Library * fontlib = new Font::Library();

    // Create a face in the library
    Font::Face * fontface = new Font::Face(fontlib, ttf_file);

    // Set the face size to be used
    fontface->size(16);

    // Fetch a glyph using utf-8 index
    const Font::Glyph * g;
    g = fontface->glyph('A');

    // Do something with the glyph, like store it in a vbo
    // g.vertices
    // g.texcoords
    // Both are GLfloat[4] containing the lower-left corner in the first two values and the upper right corner in the last two values
    // And bind the texture using the OpenGL texture id of the library:
    GLuint t = fontlib->texture();

    cout << fontlib->_texture_width << " " << fontlib->_texture_height << endl;

    cout << "tex id: " << t << endl;
    cout << g->vertices[0] << " "
        << g->vertices[1] << " "
        << g->vertices[2] << " "
        << g->vertices[3] << endl;
    cout << g->texcoords[0] << " "
        << g->texcoords[1] << " "
        << g->texcoords[2] << " "
        << g->texcoords[3] << endl;

    global::myProg->draw_letter(g->vertices, g->texcoords, t);

    // GLfloat verts[4] = {g->vertices[0], g->vertices[1], g->texcoords[0], g->texcoords[1]};
    // GLfloat texs[4]  = {g->vertices[2], g->vertices[3], g->texcoords[2], g->texcoords[3]};
    // global::myProg->draw_letter(verts, texs, t);


    // Check for any errors
    const char * err = fontlib->getErrorString();
    if (err) cout << "Font errors: " << err << endl;

    // If you wanted to write out a string, advance your cursor with
    float next_pen_x = fontface->advance('A', 'F');
    cout << next_pen_x << endl;

}

