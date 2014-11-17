#version 150

precision mediump float;

//inputs
in vec2 in_position;
in vec4 in_color;

uniform vec2 translate;

//outputs
out vec4 vColor;


void main() {
    gl_Position = vec4(in_position+translate, 0.f, 1.f);
    vColor = in_color;
}
