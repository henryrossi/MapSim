#version 330 core
in vec2 tex_coords;

uniform sampler2D perlin_map;

out vec4 FragColor;

void main () {
        float height = texture(perlin_map, tex_coords).r;
        vec3 color;
        if (height < 0.5) {
                color = vec3(0.18, 0.67, 0.84);
        } else if (height < 0.52) {
                color = vec3(0.95, 0.89, 0.64);
        } else if (height < 0.7) {
                color = vec3(0.33, .78, 0.33);
        } else if (height < 0.9) {
                color = vec3(0.09, 0.63, 0.08);
        } else {
                color = vec3(0.83,0.84, 0.81);
        }
        FragColor = vec4(color, 1.0);
}