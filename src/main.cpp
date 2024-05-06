#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <iostream>
#include <map>
#include <vector>

#include "flycamera.hpp"
#include "mapcamera.hpp"
#include "model.hpp"
#include "shader.hpp"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void process_input(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
GLuint load_texture(const char *);
GLuint load_cubemap(std::vector<const char *> faces);

const int screen_width = 800;
const int screen_height = 600;

// MapCamera camera{0.0f, 0.0f,  4.0f};
FlyCamera camera{0.0f, 5.0f, 50.0f, screen_width, screen_height};

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        GLFWwindow *window = glfwCreateWindow(screen_width, screen_height,
                                              "LearnOpenGL", NULL, NULL);
        if (window == NULL) {
                std::cout << "Failed to create glfw window\n";
                glfwTerminate();
                return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                std::cout << "Failed to initialize GLAD\n";
                return -1;
        }

        // stbi_set_flip_vertically_on_load(true);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Shader shader{"src/shaders/shader.vs", "src/shaders/shader.fs"};

        Shader shader{"src/shaders/instancing.vs", "src/shaders/instancing.fs"};
        Model planet{"resources/planet/planet.obj"};
        Shader asteroid_shader{"src/shaders/ast_instancing.vs", "src/shaders/instancing.fs"};
        Model rock{"resources/rock/rock.obj"};

        unsigned int amount = 2000000;
        glm::mat4 *modelMatrices;
        modelMatrices = new glm::mat4[amount];
        srand(glfwGetTime());
        float radius = 150.0f;
        float offset = 100.0f;
        for (unsigned int i = 0; i < amount; i++) {
                glm::mat4 model = glm::mat4(1.0f);
                // 1. translation: displace along circle with 'radius' in range
                // [-offset, offset]
                float angle = (float)i / (float)amount * 360.0f;
                float displacement =
                        (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
                float x = sin(angle) * radius + displacement;
                displacement =
                        (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
                float y = displacement * 0.4f; // keep height of field smaller
                                               // compared to width of x and z
                displacement =
                        (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
                float z = cos(angle) * radius + displacement;
                model = glm::translate(model, glm::vec3(x, y, z));

                // 2. scale: scale between 0.05 and 0.25f
                float scale = (rand() % 20) / 100.0f + 0.05;
                model = glm::scale(model, glm::vec3(scale));

                // 3. rotation: add random rotation around a (semi)randomly
                // picked rotation axis vector
                float rotAngle = (rand() % 360);
                model = glm::rotate(model, rotAngle,
                                    glm::vec3(0.4f, 0.6f, 0.8f));

                // 4. now add to list of matrices
                modelMatrices[i] = model;
        }

        GLuint buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4),
                     &modelMatrices[0], GL_STATIC_DRAW);

        for (unsigned int i = 0; i < rock.meshes.size(); i++) {
                GLuint VAO = rock.meshes[i].VAO;
                glBindVertexArray(VAO);
                std::size_t vec4size = sizeof(glm::vec4);
                glEnableVertexAttribArray(3);
                glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE,
                                      4 * vec4size, (void *)0);
                glEnableVertexAttribArray(4);
                glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE,
                                      4 * vec4size, (void *)(vec4size));
                glEnableVertexAttribArray(5);
                glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE,
                                      4 * vec4size, (void *)(2 * vec4size));
                glEnableVertexAttribArray(6);
                glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE,
                                      4 * vec4size, (void *)(3 * vec4size));

                glVertexAttribDivisor(3, 1);
                glVertexAttribDivisor(4, 1);
                glVertexAttribDivisor(5, 1);
                glVertexAttribDivisor(6, 1);

                glBindVertexArray(0);
        }

        while (!glfwWindowShouldClose(window)) {
                float currentFrame = glfwGetTime();
                deltaTime = currentFrame - lastFrame;
                lastFrame = currentFrame;

                process_input(window);

                glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_BLEND);

                glm::mat4 projection = glm::perspective(
                        glm::radians(camera.FOV),
                        (float)screen_width / (float)screen_height, 0.1f,
                        1000.0f);
                glm::mat4 view = camera.GetViewMatrix();

                shader.use();
                shader.set_uniform("projection", projection);
                shader.set_uniform("view", view);
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
                model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
                shader.set_uniform("model", model);
                planet.draw(shader);

                asteroid_shader.use();
                asteroid_shader.set_uniform("projection", projection);
                asteroid_shader.set_uniform("view", view);
                asteroid_shader.set_uniform("texture_diffuse1", 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, rock.textures_loaded[0].id);
                for (unsigned int i = 0; i < rock.meshes.size(); i++) {
                        glBindVertexArray(rock.meshes[i].VAO);
                        glDrawElementsInstanced(GL_TRIANGLES,
                                                rock.meshes[i].indices.size(),
                                                GL_UNSIGNED_INT, 0, amount);
                }

                glfwSwapBuffers(window);
                glfwPollEvents();
        }

        glfwTerminate();
        return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, true);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                camera.ProcessKeyboard(NORTH, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                camera.ProcessKeyboard(SOUTH, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                camera.ProcessKeyboard(EAST, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                camera.ProcessKeyboard(WEST, deltaTime);
        }
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);
        camera.ProcessMouseMovement(xpos, ypos);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
        camera.ProcessMouseScroll(yoffset);
}

GLuint load_texture(char const *path) {
        GLuint textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data =
                stbi_load(path, &width, &height, &nrComponents, 0);
        if (data) {
                GLenum format;
                if (nrComponents == 1) {
                        format = GL_RED;
                } else if (nrComponents == 3) {
                        format = GL_RGB;
                } else if (nrComponents == 4) {
                        format = GL_RGBA;
                }

                glBindTexture(GL_TEXTURE_2D, textureID);
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                             GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(
                        GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        format == GL_RGBA
                                ? GL_CLAMP_TO_EDGE
                                : GL_REPEAT); // for this tutorial: use
                                              // GL_CLAMP_TO_EDGE to prevent
                                              // semi-transparent borders. Due
                                              // to interpolation it takes
                                              // texels from next repeat
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                                format == GL_RGBA ? GL_CLAMP_TO_EDGE
                                                  : GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                                GL_LINEAR);
        } else {
                std::cout << "Texture failed to load at path: " << path
                          << std::endl;
        }
        stbi_image_free(data);

        return textureID;
}

GLuint load_cubemap(std::vector<const char *> faces) {
        if (faces.size() != 6) {
                std::cout << "Vector of cube map faces must have a size of 6\n";
        }

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++) {
                unsigned char *data =
                        stbi_load(faces[i], &width, &height, &nrChannels, 0);
                if (data) {
                        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                                     GL_RGB, width, height, 0, GL_RGB,
                                     GL_UNSIGNED_BYTE, data);
                } else {
                        std::cout << "Cubemap tex failed to load at path: "
                                  << faces[i] << std::endl;
                }
                stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                        GL_CLAMP_TO_EDGE);

        return textureID;
}