#version 330 core
in vec2 tex_coords;

uniform sampler2D perlin_map;

out vec4 FragColor;

void main () {
        float height = texture(perlin_map, tex_coords).r;
        FragColor = vec4(vec3(height), 1.0);
}