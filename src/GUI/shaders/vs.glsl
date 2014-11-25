#version 150

precision mediump float;

// inputs
in vec2 in_position;
in vec4 in_color;
in vec2 in_texCoord;

// vertex uniforms
uniform vec2 translate;

// uniform uint rand_trans;

// vertex and fragment uniforms
uniform int render_mode;



//outputs
out vec4 vColor;
out vec2 vTexCoord;


void main() {
    gl_Position = vec4(in_position+translate, 0.f, 1.f);
    vColor = in_color;
    vTexCoord = in_texCoord;
}
