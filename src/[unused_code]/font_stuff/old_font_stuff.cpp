#include "fontstash.h"
#include <fstream>

void do_font_stuff() {
    char *ttf_file = "src/OpenSans-Regular.ttf";
    // std::ifstream
    std::ifstream f(ttf_file);
    if (f.is_open()) {
        cout << "Open file." << endl;
    } else {
        cout << "Did not open file." << endl;
    }


    // fp = fopen("DroidSerif-Regular.ttf", "rb");
    // if (!fp) goto error_add_font;
    // fseek(fp, 0, SEEK_END);
    // datasize = (int)ftell(fp);
    // fseek(fp, 0, SEEK_SET);
    // data = (unsigned char*)malloc(datasize);
    // if (data == NULL) goto error_add_font;
    // fread(data, 1, datasize, fp);
    // fclose(fp);


     // create a font stash with a maximum texture size of 512 x 512
    struct sth_stash* stash = sth_create(512, 512);
     // load truetype font
    int font_id = sth_add_font(stash, ttf_file);
     // position of the text

    // float x = 10, y = 10;
    float x = 0, y = 0;


    // draw text during your OpenGL render loop
    sth_begin_draw(stash);
    // position: (x, y); font size: 24
    sth_draw_text(stash, font_id, 24.0f, x, y, "Hello world!", &x);
    // now, the float x contains the x position of the next char
    sth_end_draw(stash);
}

// void draw_font() {
// }



























// #define STB_TRUETYPE_IMPLEMENTATION
// #include "../ext/stb/stb_truetype.h"





// stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
// GLuint ftex;

// void my_stbtt_initfont(void)
// {
//     char *ttf_file = "src/OpenSans-Regular.ttf";

//     unsigned char ttf_buffer[1<<20];
//     unsigned char temp_bitmap[512*512];

//     fread(ttf_buffer, 1, 1<<20, fopen(ttf_file, "rb"));
//     // cout << ttf_buffer << endl;

//     stbtt_BakeFontBitmap(ttf_buffer, 0,
//         32.0,
//         temp_bitmap, 512, 512,
//         32, 96,
//         cdata); // no guarantee this fits!
//     // can free ttf_buffer at this point
//     glGenTextures(1, &ftex);
//     glBindTexture(GL_TEXTURE_2D, ftex);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512,512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
//     // can free temp_bitmap at this point
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// }

// void my_stbtt_print(float x, float y, char *text)
// {
//     // assume orthographic projection with units = screen pixels, origin at top left
//     glBindTexture(GL_TEXTURE_2D, ftex);
//     glBegin(GL_QUADS);
//     while (*text) {
//         if (*text >= 32 && *text < 128) {
//             stbtt_aligned_quad q;
//             stbtt_GetBakedQuad(cdata, 512, 512, *text-32, &x, &y, &q, 1);//1=opengl & d3d10+,0=d3d9
//             glTexCoord2f(q.s0,q.t1); glVertex2f(q.x0,q.y0);
//             glTexCoord2f(q.s1,q.t1); glVertex2f(q.x1,q.y0);
//             glTexCoord2f(q.s1,q.t0); glVertex2f(q.x1,q.y1);
//             glTexCoord2f(q.s0,q.t0); glVertex2f(q.x0,q.y1);
//         }
//         ++text;
//     }
//     glEnd();
// }



// void do_font_stuff() {
//     my_stbtt_initfont();
//     my_stbtt_print(20, 20, "Hello world!");
// }












// // void do_font_stuff() {
// //     char *ttf_file = "src/OpenSans-Regular.ttf";
// //     char *text = "Hello World!";



// //     unsigned char buffer[24<<20];
// //     unsigned char screen[20][79];

// //     stbtt_fontinfo font;
// //     int i,j,ascent,baseline,ch=0;
// //     float scale, xpos=2; // leave a little padding in case the character extends left

// //     fread(buffer, 1, 1000000, fopen(ttf_file, "rb"));
// //     stbtt_InitFont(&font, buffer, 0);
// //     scale = stbtt_ScaleForPixelHeight(&font, 15);
// //     stbtt_GetFontVMetrics(&font, &ascent,0,0);
// //     baseline = (int) (ascent*scale);

// //     while (text[ch]) {
// //         int advance,lsb,x0,y0,x1,y1;
// //         float x_shift = xpos - (float) floor(xpos);
// //         stbtt_GetCodepointHMetrics(&font, text[ch], &advance, &lsb);
// //         stbtt_GetCodepointBitmapBoxSubpixel(&font, text[ch], scale,scale,x_shift,0, &x0,&y0,&x1,&y1);
// //         stbtt_MakeCodepointBitmapSubpixel(&font, &screen[baseline + y0][(int) xpos + x0], x1-x0,y1-y0, 79, scale,scale,x_shift,0, text[ch]);
// //         // note that this stomps the old data, so where character boxes overlap (e.g. 'lj') it's wrong
// //         // because this API is really for baking character bitmaps into textures. if you want to render
// //         // a sequence of characters, you really need to render each bitmap to a temp buffer, then
// //         // "alpha blend" that into the working buffer
// //         xpos += (advance * scale);
// //         if (text[ch+1])
// //             xpos += scale*stbtt_GetCodepointKernAdvance(&font, text[ch],text[ch+1]);
// //         ++ch;
// //     }

// //     for (j=0; j < 20; ++j) {
// //         for (i=0; i < 78; ++i)
// //             putchar(" .:ioVM@"[screen[j][i]>>5]);
// //         putchar('\n');
// //     }
// //     // return 0;
// // }
