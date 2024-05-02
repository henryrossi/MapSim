#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
        unsigned int ID;

        Shader(const char* vertexPath, const char* fragmentPath);

        void use();

        void set_uniform(const char *name, int i) const;
        void set_uniform(const char *name, float f) const;
        void set_uniform(const char *name, float x, float y, float z) const;
        void set_uniform(const char *name, glm::vec3 &vec) const;
        void set_uniform(const char *name, glm::mat4 &mat) const;
};
#endif