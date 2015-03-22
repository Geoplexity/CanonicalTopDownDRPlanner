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

uniform mat4 view_matrix;
uniform mat4 projection_matrix;


//outputs
out vec4 vColor;
out vec2 vTexCoord;


void main() {
    // vec4 new_position = vec4(in_position+translate, 0.f, 1.f);
    // gl_Position = view_matrix * vec4(in_position+translate, 0.f, 1.f);
    gl_Position = projection_matrix * view_matrix * vec4(in_position+translate, 0.f, 1.f);
    vColor = in_color;
    vTexCoord = in_texCoord;
}
