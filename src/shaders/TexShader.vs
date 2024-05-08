#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex_c;

out vec2 tex_coords;

void main () {
        tex_coords = tex_c;
        gl_Position = vec4(pos, 1.0);
}