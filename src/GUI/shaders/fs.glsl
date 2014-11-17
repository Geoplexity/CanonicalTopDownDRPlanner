#version 150

precision mediump float;

//inputs
in vec4 vColor;

//outputs
out vec4 frag_color;

void main() {
	frag_color = vColor;
}
