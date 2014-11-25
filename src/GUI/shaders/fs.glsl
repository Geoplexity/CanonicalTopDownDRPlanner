#version 150

precision mediump float;

// inputs
in vec4 vColor;
in vec2 vTexCoord;

// vertex and fragment uniforms
uniform int render_mode;
uniform sampler2DRect tex_sampler;

// outputs
out vec4 frag_color;

void main() {
    if (render_mode == 0) {
        frag_color = vColor;
    } else if (render_mode == 1) {
        // sampler2DRect​ tex_sampler = 0;
        frag_color.x = 1;
        frag_color = texture(tex_sampler, vTexCoord);
        // color = vec4( fontcolor.xyz, fontcolor.w * texture(texture0, texcoord_final).x );
    }
}
