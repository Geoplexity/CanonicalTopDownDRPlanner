#include "Graph/Graph.hpp"
#include "Graph/Pebbled_Graph.hpp"

#include "GUI/MainGuiManager.hpp"
#include "GUI/program.hpp"

#include "GUI/Circle.hpp"

#include <iostream>
#include <vector>
using namespace std;




// `freetype-config --libs`









#include "GUI/Font.hpp"







namespace global {
    Program *myProg;

    namespace window {
        const int width_screen_coords = 1000, height_screen_coords = 1000;
        // int width_px = 1000, height_px = 1000;
        // const char *title = "Demo";
    }
}


class Demowindow : public Window {
    void key_callback(int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            cout << "HERE!" << endl;
            // glfwSetWindowShouldClose(window, GL_TRUE);
    };
};





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









vector<gl_obj::pos_vec> verts_from_graph(Graph &g) {
    vector<gl_obj::pos_vec> v;

    for (std::pair<Vertex_Iterator, Vertex_Iterator> verts = g.vertices();
        verts.first != verts.second;
        verts.first++)
    {
        v.push_back(gl_obj::pos_vec(g[*verts.first].x, g[*verts.first].y));
    }

    return v;
}


vector<gl_obj::pos_vec> edges_from_graph(Graph &g) {
    vector<gl_obj::pos_vec> v;

    for (std::pair<Edge_Iterator, Edge_Iterator> edges = g.edges();
        edges.first != edges.second;
        edges.first++)
    {
        std::pair<Vertex_ID, Vertex_ID> uv = g.verts_on_edge(*edges.first);
        v.push_back(gl_obj::pos_vec(g[uv.first].x, g[uv.first].y));
        v.push_back(gl_obj::pos_vec(g[uv.second].x, g[uv.second].y));
    }


    return v;
}


void display_graph(Window &wind, Graph &g) {
    std::vector<gl_obj::pos_vec> vertices = verts_from_graph(g);
    std::vector<gl_obj::pos_vec> edges = edges_from_graph(g);


    wind.set_as_context();


    // Clip the view port to match our ratio
    int width, height;
    wind.get_window_size_in_pixels(&width, &height);
    glViewport(0, 0, width, height);


    global::myProg->clearViewport();
    // do_font_stuff();
    global::myProg->draw_graph_edges(edges);
    global::myProg->draw_graph_vertices(vertices);

    // myProg->draw_colorpicking_scene();
    global::myProg->flush();

    wind.swap_buffers();
}

















enum runtime_options {
    ro_drp_2d,
    ro_display_realized_graph,
    ro_test
};



int main(int argc, char **argv) {
    string dot_file = "test_files/test.dot";

    runtime_options runtime_option = ro_drp_2d;

    // handle command-line arguments
    for (int i = 1; i < argc; i++) {
        string inp = argv[i];
        string flag, val;

        int eqsign = inp.find("=");
        if (eqsign+1 == inp.length()) {
            cerr << "No value for flag " << inp.substr(0, eqsign) << "." << endl;
            return 0;
        } else if (eqsign == -1) {
            flag = inp;
            val = "";
        } else {
            flag = inp.substr(0, eqsign);
            val  = inp.substr(eqsign+1, inp.length());
        }

        // compare returns 0 if the value is the same, so not ! determines if they are the same
        if (!flag.compare("-dot")) {
            if (!val.compare("")) {
                i++;
                if (i == argc) {
                    cerr << "No filename provided for flag " << flag << "." << endl;
                    return 0;
                }
                dot_file = argv[i];
            } else {
                cerr << "Invalid value for flag " << flag << "." << endl;
                return 0;
            }
        }

        else if (!flag.compare("-opt")) {
            if (!val.compare("0")) {
                runtime_option = ro_drp_2d;
            } else if (!val.compare("1")) {
                runtime_option = ro_display_realized_graph;
            } else if (!val.compare("2")) {
                runtime_option = ro_test;
            } else {
                cerr << "Invalid value for flag " << flag << "." << endl;
                return 0;
            }
        }

        // else if (!flag.compare("-nogui")) {
        //     if (!val.compare("")) {
        //         nogui = true;
        //     } else {
        //         cerr << "Invalid value for flag " << flag << "." << endl;
        //         return 0;
        //     }
        // }

        else {
         cerr << "The flag " << flag << " is not recognized." << endl;
         return 0;
        }
    }


    // read in
    Graph g;
    g.read_from_file(dot_file.c_str());

    // // test output
    // Graph copy(g);
    // copy.write_to_file("test_files/out.dot");

    // get layout... x/y coords
    if (runtime_option != ro_display_realized_graph) {
        g.set_layout();
        g.get_graph_in_range(-0.87, 0.87, -0.87, 0.87);
    } else {
        g.get_graph_in_range(-0.87, 0.87, -0.87, 0.87);
    }


    MainGuiManager mgm;

    Demowindow myWindow;
    mgm.create_window(&myWindow, global::window::width_screen_coords, global::window::height_screen_coords, "Graph Drawer", 3, 2);
    myWindow.set_as_context();


    // my stuff
    global::myProg = new Program("src/GUI/shaders/vs.glsl", "src/GUI/shaders/fs.glsl");


    display_graph(myWindow, g);




    if (runtime_option == ro_drp_2d) {
        cout << "Running DRP_2D on \"" << dot_file << "\"." << endl;

        Subgraph sg(&g);
        std::pair<Vertex_Iterator, Vertex_Iterator> vs = g.vertices();
        sg.induce(vs.first, vs.second);

        // sg.remove_vertex(*(g.find_vertex("7")));


        Pebbled_Graph pg(&sg);

        Cluster c = pg.DRP_2D();
        c.print_tree(g);
    } else if (runtime_option == ro_display_realized_graph) {
        cout << "Displaying realized graph \"" << dot_file << "\"." << endl;

        Subgraph sg(&g);
        std::pair<Vertex_Iterator, Vertex_Iterator> vs = g.vertices();
        sg.induce(vs.first, vs.second);
        Pebbled_Graph pg(&sg);

        cout << "Pebbles remaining: " << pg.pebble_game_2D() << endl;
    } else if (runtime_option == ro_test) {
        cout << "Nothing." << endl;
    }


    while (!myWindow.should_close()) {
        // mgm.poll_for_events();
        mgm.wait_for_events();
    }




    return 0;
}






















// #include <wchar.h>

// #include "../ext/freetype-gl-master/freetype-gl.h"
// #include "../ext/freetype-gl-master/mat4.h"
// #include "../ext/freetype-gl-master/shader.h"
// #include "../ext/freetype-gl-master/vertex-buffer.h"


// typedef struct {
//     float x, y, z;    // position
//     float s, t;       // texture
//     float r, g, b, a; // color
// } vertex_t;

// void add_text( vertex_buffer_t * buffer, texture_font_t * font,
//                wchar_t * text, vec4 * color, vec2 * pen )
// {
//     size_t i;
//     float r = color->red, g = color->green, b = color->blue, a = color->alpha;
//     for( i=0; i<wcslen(text); ++i )
//     {
//         texture_glyph_t *glyph = texture_font_get_glyph( font, text[i] );
//         if( glyph != NULL )
//         {
//             float kerning =  0.0f;
//             if( i > 0)
//             {
//                 kerning = texture_glyph_get_kerning( glyph, text[i-1] );
//             }
//             pen->x += kerning;
//             int x0  = (int)( pen->x + glyph->offset_x );
//             int y0  = (int)( pen->y + glyph->offset_y );
//             int x1  = (int)( x0 + glyph->width );
//             int y1  = (int)( y0 - glyph->height );
//             float s0 = glyph->s0;
//             float t0 = glyph->t0;
//             float s1 = glyph->s1;
//             float t1 = glyph->t1;
//             GLuint indices[6] = {0,1,2, 0,2,3};
//             vertex_t vertices[4] = { { x0,y0,0,  s0,t0,  r,g,b,a },
//                                      { x0,y1,0,  s0,t1,  r,g,b,a },
//                                      { x1,y1,0,  s1,t1,  r,g,b,a },
//                                      { x1,y0,0,  s1,t0,  r,g,b,a } };
//             vertex_buffer_push_back( buffer, vertices, 4, indices, 6 );
//             pen->x += glyph->advance_x;
//         }
//     }
// }

// int main()
// {
//     // Graph g = set_up_graph();
//     // g.write_to_file("test.dot");

//     Graph g;
//     g.read_from_file("test.dot");

//     g.set_layout();



//     MainGuiManager mgm;

//     Demowindow myWindow;
//     mgm.create_window(&myWindow, global::window::width_screen_coords, global::window::height_screen_coords, "Graph Drawer", 3, 2);
//     myWindow.set_as_context();


//     // my stuff
//     global::myProg = new Program("src/GUI/shaders/vs.glsl", "src/GUI/shaders/fs.glsl");


//     display_graph(myWindow, g);





//     // Text to be printed
//     wchar_t *text = L"A Quick Brown Fox Jumps Over The Lazy Dog 0123456789";

//     // Texture atlas to store individual glyphs
//     texture_atlas_t *atlas = texture_atlas_new( 512, 512, 1 );

//     // Build a new texture font from its description and size
//     texture_font_t *font = texture_font_new_from_file( atlas, 16, "./Vera.ttf");

//     // Build a new vertex buffer (position, texture & color)
//     vertex_buffer_t *buffer= vertex_buffer_new( "v3i:t2f:c4f" );

//     // Cache some glyphs to speed things up
//     texture_font_load_glyphs( font, L" !\"#$%&'()*+,-./0123456789:;<=>?"
//         L"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
//         L"`abcdefghijklmnopqrstuvwxyz{|}~");

//     // Where to start printing on screen
//     vec2 pen = {0,0};
//     vec4 black = {0,0,0,1};

// // void add_text( vertex_buffer_t * buffer, texture_font_t * font,
//                // wchar_t * text, vec4 * color, vec2 * pen )
//     // Add text tothe buffer (see demo-font.c for the add_text code)
//     add_text( buffer, font, text, &black, &pen );







//     while (!myWindow.should_close()) {
//         // mgm.poll_for_events();
//         mgm.wait_for_events();
//     }




//     return 0;
// }
