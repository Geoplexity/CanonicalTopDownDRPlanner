#version 150

precision mediump float;

// inputs
in vec2 in_position;

// uniforms
uniform vec4 color;

uniform mat4 projection_view_matrix;
uniform mat4 model_matrix;

//outputs
out vec4 vColor;


void main() {
    gl_Position = projection_view_matrix * model_matrix * vec4(in_position, 0.f, 1.f);
    vColor = color;
}
