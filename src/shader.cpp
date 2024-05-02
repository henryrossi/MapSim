#include "shader.hpp"

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
        std::ifstream vertexFile, fragmentFile;

        vertexFile.open(vertexPath);
        if (!vertexFile.is_open()) {
                std::cout << "Failed to open vertex file\n";
        }
        std::stringstream vertexSS;
        vertexSS << vertexFile.rdbuf();
        std::string vertexContentsString = vertexSS.str();
        const char *vertexContents = vertexContentsString.c_str();

        fragmentFile.open(fragmentPath);
        if (!fragmentFile.is_open()) {
                std::cout << "Failed to open vertex file\n";
        }
        std::stringstream fragmentSS;
        fragmentSS << fragmentFile.rdbuf();
        std::string fragmentContentsString = fragmentSS.str();
        const char *fragmentContents = fragmentContentsString.c_str();

        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexContents, NULL);
        glCompileShader(vertexShader);

        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
                glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentContents, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
                glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        ID = glCreateProgram();
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragmentShader);
        glLinkProgram(ID);

        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
                glGetProgramInfoLog(ID, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
}

void Shader::use() {
        glUseProgram(ID);
}

void Shader::set_uniform(const char *name, int i) const {
        glUniform1i(glGetUniformLocation(ID, name), i);
}

void Shader::set_uniform(const char *name, float f) const {
        glUniform1f(glGetUniformLocation(ID, name), f);
}

void Shader::set_uniform(const char *name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}

void Shader::set_uniform(const char *name, glm::vec3 &vec) const {
        glUniform3fv(glGetUniformLocation(ID, name), 1, &vec[0]);
}


void Shader::set_uniform(const char *name, glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
}
