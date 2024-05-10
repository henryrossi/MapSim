#version 330 core
in vec2 tex_coords;

uniform sampler2D perlin_map;
uniform vec3 sun_dir;

out vec4 FragColor;


float shadow_brightness = 0.5;
float steps = 200.0;

void main () {
        float height = texture(perlin_map, tex_coords).r;
        vec3 color;
        if (height == 0.0) {
                color = vec3(0.18, 0.67, 0.84);
        } else if (height < 0.1) {
                color = vec3(0.95, 0.89, 0.64);
        } else if (height < 0.3) {
                color = vec3(0.33, .78, 0.33);
        } else if (height < 0.5) {
                color = vec3(0.09, 0.63, 0.08);
        } else {
                color = vec3(0.83,0.84, 0.81);
        }

        vec3 cur_pos = vec3(tex_coords.x, tex_coords.y, height);        
        vec3 step_dir = sun_dir/steps;
        for (float i = 0.0; i < steps; i++) {
                cur_pos -= step_dir;
                // check bounds
                if (cur_pos.x < 0.0 || cur_pos.y < 0.0) {
                        break;
                }

                float h = texture(perlin_map, cur_pos.xy).r;
                if (h > cur_pos.z) {
                        color *= shadow_brightness;
                        break;
                }
                if (cur_pos.z > 1.0) {
                        break;
                }
        }

        FragColor = vec4(color, 1.0);
}