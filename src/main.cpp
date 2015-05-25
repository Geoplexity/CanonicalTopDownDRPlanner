#include "Graph/Graph.hpp"
#include "Graph/Pebbled_Graph.hpp"
#include "Graph/Isostatic_Graph_Realizer.hpp"

#include "Application/Graph_Display_Window.hpp"

#include <iostream>
#include <vector>
#include <map>
using namespace std;




enum runtime_options {
    ro_drp_2d = 0,
    ro_display_linkage = 1,
    ro_display_framework = 2,
    ro_test = 3
};




namespace global {
    // Program *myProg;

    namespace dflt {
        string dot_file = "test_files/test.dot";
        runtime_options runtime_option = ro_drp_2d;
    }

    namespace window {
        const int width_screen_coords = 1000, height_screen_coords = width_screen_coords/1.618;
        // const int width_screen_coords = 1000, height_screen_coords = 1000;
        // int width_px = 1000, height_px = 1000;
        // const char *title = "Demo";
    }
}



int main(int argc, char **argv) {
    string dot_file = global::dflt::dot_file;
    runtime_options runtime_option = global::dflt::runtime_option;

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
                runtime_option = ro_display_linkage;
            } else if (!val.compare("2")) {
                runtime_option = ro_display_framework;
            } else if (!val.compare("3")) {
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
    if (runtime_option != ro_display_framework) {
        g.set_layout();
        g.get_graph_in_range(-0.87, 0.87, -0.87, 0.87);
    } else {
        g.get_graph_in_range(-0.87, 0.87, -0.87, 0.87);
    }


    Main_GUI_Manager mgm;
    Graph_Display_Window myWindow(&mgm, &g);
    // myWindow.init();


    mgm.create_window(
        &myWindow,
        global::window::width_screen_coords,
        global::window::height_screen_coords,
        "Graph Drawer",
        3,
        2,
        true);
    myWindow.init_program();

    myWindow.update_display();




    if (runtime_option == ro_drp_2d) {
        cout << "Running DRP_2D on \"" << dot_file << "\"." << endl;

        myWindow.get_drp();
    } else if (runtime_option == ro_display_linkage) {
        cout << "Displaying realized graph \"" << dot_file << "\"." << endl;

        // Subgraph sg(&g);
        // std::pair<Vertex_Iterator, Vertex_Iterator> vs = g.vertices();
        // sg.induce(vs.first, vs.second);
        // Pebbled_Graph pg(&sg);

        // cout << "Pebbles remaining: " << pg.pebble_game_2D() << endl;
    } else if (runtime_option == ro_display_framework) {
        cout << "Displaying realized graph \"" << dot_file << "\"." << endl;

        Subgraph sg(&g);
        std::pair<Vertex_Iterator, Vertex_Iterator> vs = g.vertices();
        sg.induce(vs.first, vs.second);
        Pebbled_Graph pg(&sg);

        cout << "Pebbles remaining: " << pg.pebble_game_2D() << endl;
    } else if (runtime_option == ro_test) {
        cout << "Testing realization." << endl;

        Isostatic_Graph_Realizer isg(&g);
        isg.realize();

        g = *isg.working_copy;

        // int i = 0;

        // cout << "Original Vertices (" << g.num_vertices() << " vertices, "<< g.num_edges() << " edges): " << endl;
        // std::pair<Vertex_Iterator, Vertex_Iterator> vs = g.vertices();
        // for (Vertex_Iterator v_it = vs.first; v_it != vs.second; v_it++) {
        //     // cout << "\t" << i++ << ": " << *v_it << endl;
        //     cout << "\t" << i++ << ": " << g[*v_it].name << endl;
        // }

        // // Vertex_ID v0 = *vs.first;
        // // vs.first++;
        // // Vertex_ID v1 = *vs.first;
        // g.contract_edge(*(g.find_vertex("e")), *(g.find_vertex("c")));
        // g.contract_edge(*(g.find_vertex("g")), *(g.find_vertex("c")));
        // g.contract_edge(*(g.find_vertex("a")), *(g.find_vertex("b")));
        // g.contract_edge(*(g.find_vertex("h")), *(g.find_vertex("i")));
        // g.contract_edge(*(g.find_vertex("f")), *(g.find_vertex("d")));


        // i = 0;
        // cout << "New Vertices (" << g.num_vertices() << " vertices, "<< g.num_edges() << " edges): " << endl;
        // vs = g.vertices();
        // for (Vertex_Iterator v_it = vs.first; v_it != vs.second; v_it++) {
        //     // cout << "\t" << i++ << ": " << *v_it << endl;
        //     cout << "\t" << i++ << ": " << g[*v_it].name << endl;
        // }

        myWindow.update_graph_positions();
        myWindow.update_display();
        // std::pair<Vertex_Iterator, Vertex_Iterator> vs = g.vertices();
        // do {
        //     cout << *(vs.first) << endl;
        // } while (++(vs.first) != vs.second);
    }


    while (!myWindow.should_close()) {
        // mgm.poll_for_events();
        // cout << "About to wait." << endl;
        mgm.wait_for_events();
        // cout << "Handled." << endl;
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



//     Main_GUI_Manager mgm;

//     Graph_Display_Window myWindow;
//     mgm.create_window(&myWindow, global::window::width_screen_coords, global::window::height_screen_coords, "Graph Drawer", 3, 2);
//     myWindow.set_as_context();


//     // my stuff
//     global::myProg = new Program("src/GUI/shaders/vs.glsl", "src/GUI/shaders/fs.glsl");


//     update_display(myWindow, g);





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
