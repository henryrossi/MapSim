#include "shader.hpp"

Shader::Shader(const char *vertex_path, const char *fragment_path) {
        std::string vertex_contents = read_glsl_shader(vertex_path);
        std::string fragment_contents = read_glsl_shader(fragment_path);
        GLuint vertex_shader = compile_shader(vertex_contents.c_str(), VERTEX);
        GLuint fragment_shader = compile_shader(fragment_contents.c_str(), FRAGMENT);

        id = glCreateProgram();
        glAttachShader(id, vertex_shader);
        glAttachShader(id, fragment_shader);
        glLinkProgram(id);
        int success;
        char infoLog[512];
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success) {
                glGetProgramInfoLog(id, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
}

Shader::Shader(const char *vertex_path, const char *fragment_path, const char *geometry_path) {
        std::string vertex_contents = read_glsl_shader(vertex_path);
        std::string fragment_contents = read_glsl_shader(fragment_path);
        std::string geometry_contents = read_glsl_shader(geometry_path);
        GLuint vertex_shader = compile_shader(vertex_contents.c_str(), VERTEX);
        GLuint fragment_shader = compile_shader(fragment_contents.c_str(), FRAGMENT);
        GLuint geometry_shader = compile_shader(geometry_contents.c_str(), GEOMETRY);

        id = glCreateProgram();
        glAttachShader(id, vertex_shader);
        glAttachShader(id, fragment_shader);
        glAttachShader(id, geometry_shader);
        glLinkProgram(id);
        int success;
        char infoLog[512];
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success) {
                glGetProgramInfoLog(id, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        glDeleteShader(geometry_shader);
}

std::string Shader::read_glsl_shader(const char *path) {
        std::ifstream glsl_file;
        glsl_file.open(path);
        if (!glsl_file.is_open()) {
                std::cout << "Failed to open glsl shader file at path: " 
                          << path << "\n";
        }
        std::stringstream shader_ss;
        shader_ss << glsl_file.rdbuf();
        return shader_ss.str();
}

GLuint Shader::compile_shader(const char *shader_contents, ShaderType type) {
        int GL_shader_type;
        switch (type) {
        case VERTEX:
                GL_shader_type = GL_VERTEX_SHADER;
                break;
        case FRAGMENT:
                GL_shader_type = GL_FRAGMENT_SHADER;
                break;
        case GEOMETRY:
                GL_shader_type = GL_GEOMETRY_SHADER;
                break;
        }

        GLuint shader = glCreateShader(GL_shader_type);
        glShaderSource(shader, 1, &shader_contents, NULL);
        glCompileShader(shader);

        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
                glGetShaderInfoLog(shader, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        return shader;
}

void Shader::use() {
        glUseProgram(id);
}

void Shader::set_uniform(const char *name, int i) const {
        glUniform1i(glGetUniformLocation(id, name), i);
}

void Shader::set_uniform(const char *name, unsigned int i) const {
        glUniform1i(glGetUniformLocation(id, name), i);
}

void Shader::set_uniform(const char *name, float f) const {
        glUniform1f(glGetUniformLocation(id, name), f);
}

void Shader::set_uniform(const char *name, float x, float y) const {
        glUniform2f(glGetUniformLocation(id, name), x, y);
}

void Shader::set_uniform(const char *name, glm::vec2 &vec) const {
        glUniform2fv(glGetUniformLocation(id, name), 1, &vec[0]);
}

void Shader::set_uniform(const char *name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(id, name), x, y, z);
}

void Shader::set_uniform(const char *name, glm::vec3 &vec) const {
        glUniform3fv(glGetUniformLocation(id, name), 1, &vec[0]);
}


void Shader::set_uniform(const char *name, glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, &mat[0][0]);
}
