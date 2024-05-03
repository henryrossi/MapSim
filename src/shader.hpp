#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

enum ShaderType {
        VERTEX,
        FRAGMENT,
        GEOMETRY,
};

class Shader {
public:
        unsigned int id;

        Shader(const char *vertex_path, const char *fragment_path);
        Shader(const char *vertex_path, const char *fragment_path, const char *geometry_path);

        void use();

        void set_uniform(const char *name, int i) const;
        void set_uniform(const char *name, float f) const;
        void set_uniform(const char *name, float x, float y) const;
        void set_uniform(const char *name, glm::vec2 &vec) const;
        void set_uniform(const char *name, float x, float y, float z) const;
        void set_uniform(const char *name, glm::vec3 &vec) const;
        void set_uniform(const char *name, glm::mat4 &mat) const;
private:
        std::string read_glsl_shader(const char *path);
        GLuint compile_shader(const char *shader_contents, ShaderType type);
};

#endif /* SHADER_H */