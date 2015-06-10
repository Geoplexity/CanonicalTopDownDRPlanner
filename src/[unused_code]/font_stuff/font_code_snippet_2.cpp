

#include <wchar.h>

#include "../ext/freetype-gl-master/freetype-gl.h"
#include "../ext/freetype-gl-master/mat4.h"
#include "../ext/freetype-gl-master/shader.h"
#include "../ext/freetype-gl-master/vertex-buffer.h"


typedef struct {
    float x, y, z;    // position
    float s, t;       // texture
    float r, g, b, a; // color
} vertex_t;

void add_text( vertex_buffer_t * buffer, texture_font_t * font,
               wchar_t * text, vec4 * color, vec2 * pen )
{
    size_t i;
    float r = color->red, g = color->green, b = color->blue, a = color->alpha;
    for( i=0; i<wcslen(text); ++i )
    {
        texture_glyph_t *glyph = texture_font_get_glyph( font, text[i] );
        if( glyph != NULL )
        {
            float kerning =  0.0f;
            if( i > 0)
            {
                kerning = texture_glyph_get_kerning( glyph, text[i-1] );
            }
            pen->x += kerning;
            int x0  = (int)( pen->x + glyph->offset_x );
            int y0  = (int)( pen->y + glyph->offset_y );
            int x1  = (int)( x0 + glyph->width );
            int y1  = (int)( y0 - glyph->height );
            float s0 = glyph->s0;
            float t0 = glyph->t0;
            float s1 = glyph->s1;
            float t1 = glyph->t1;
            GLuint indices[6] = {0,1,2, 0,2,3};
            vertex_t vertices[4] = { { x0,y0,0,  s0,t0,  r,g,b,a },
                                     { x0,y1,0,  s0,t1,  r,g,b,a },
                                     { x1,y1,0,  s1,t1,  r,g,b,a },
                                     { x1,y0,0,  s1,t0,  r,g,b,a } };
            vertex_buffer_push_back( buffer, vertices, 4, indices, 6 );
            pen->x += glyph->advance_x;
        }
    }
}

int main()
{
    // Graph g = set_up_graph();
    // g.write_to_file("test.dot");

    Graph g;
    g.read_from_file("test.dot");

    g.set_layout();



    Main_GUI_Manager mgm;

    Graph_Display_Window myWindow;
    mgm.create_window(&myWindow, global::window::width_screen_coords, global::window::height_screen_coords, "Graph Drawer", 3, 2);
    myWindow.set_as_context();


    // my stuff
    global::myProg = new Program("src/GUI/shaders/vs.glsl", "src/GUI/shaders/fs.glsl");


    update_display(myWindow, g);





    // Text to be printed
    wchar_t *text = L"A Quick Brown Fox Jumps Over The Lazy Dog 0123456789";

    // Texture atlas to store individual glyphs
    texture_atlas_t *atlas = texture_atlas_new( 512, 512, 1 );

    // Build a new texture font from its description and size
    texture_font_t *font = texture_font_new_from_file( atlas, 16, "./Vera.ttf");

    // Build a new vertex buffer (position, texture & color)
    vertex_buffer_t *buffer= vertex_buffer_new( "v3i:t2f:c4f" );

    // Cache some glyphs to speed things up
    texture_font_load_glyphs( font, L" !\"#$%&'()*+,-./0123456789:;<=>?"
        L"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
        L"`abcdefghijklmnopqrstuvwxyz{|}~");

    // Where to start printing on screen
    vec2 pen = {0,0};
    vec4 black = {0,0,0,1};

// void add_text( vertex_buffer_t * buffer, texture_font_t * font,
               // wchar_t * text, vec4 * color, vec2 * pen )
    // Add text tothe buffer (see demo-font.c for the add_text code)
    add_text( buffer, font, text, &black, &pen );







    while (!myWindow.should_close()) {
        // mgm.poll_for_events();
        mgm.wait_for_events();
    }




    return 0;
}
