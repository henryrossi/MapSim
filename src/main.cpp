#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include "mapcamera.hpp"
#include "noise.hpp"
#include "shader.hpp"


void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void process_input(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
GLuint load_texture(const char *);

void render_quad(GLuint vao, GLuint vbo);
void create_noise(GLbyte *data, int width, int height);

static int screen_width = 800;
static int screen_height = 800;

MapCamera camera{0.0f, 0.0f, 4.0f};

static float deltaTime = 0.0f;
static float lastFrame = 0.0f;

glm::vec3 sun_dir{1.0, 0.0, -1.0};

int main() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        GLFWwindow *window = glfwCreateWindow(screen_width, screen_height,
                                              "MapSim", NULL, NULL);
        if (window == NULL) {
                std::cout << "Failed to create glfw window\n";
                glfwTerminate();
                return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                std::cout << "Failed to initialize GLAD\n";
                return -1;
        }

        // stbi_set_flip_vertically_on_load(true);

        glEnable(GL_DEPTH_TEST);

        Shader texShader{"src/shaders/TexShader.vs",
                         "src/shaders/StepShadow.fs"};

        GLuint perlin_map;
        glGenTextures(1, &perlin_map);
        glBindTexture(GL_TEXTURE_2D, perlin_map);

        GLbyte *perlin_data = new GLbyte[1024 * 1024];
        create_noise(perlin_data, 1024, 1024);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0,
                     GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, perlin_data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, perlin_map);

        GLuint quad_vao{}, quad_vbo{};

        while (!glfwWindowShouldClose(window)) {
                float currentFrame = glfwGetTime();
                deltaTime = currentFrame - lastFrame;
                lastFrame = currentFrame;

                process_input(window);

                glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                texShader.use();
                texShader.set_uniform("sun_dir", sun_dir);
                texShader.set_uniform("perlin_map", 0);
                render_quad(quad_vao, quad_vbo);

                glfwSwapBuffers(window);
                glfwPollEvents();
        }

        glfwTerminate();
        return 0;
}

void create_noise(GLbyte *data, int width, int height) {
        perlin p{};
        for (int x = 0; x < width; x++) {
                for (int y = 0; y < height; y++) {
                        float v = p.fbm_noise(x, y, 8);
                        v *= 0.5;
                        v +=  0.2;
                        float xpos = (float)x / width;
                        float ypos = (float)y / height;
                        float c = glm::distance(glm::vec2(xpos, ypos), glm::vec2(0.5));
                        v -= c;
                        if (v < 0.0) {
                                v = 0.0;
                        }
                        data[y * height + x] = v * 255;
                }
        }
}

void render_quad(GLuint vao, GLuint vbo) {
        if (vao == 0) {
                // clang-format off
                float quadVertices[] = {
                        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                };
                // clang-format on
                glGenVertexArrays(1, &vao);
                glGenBuffers(1, &vbo);
                glBindVertexArray(vao);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices),
                             &quadVertices, GL_STATIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                                      5 * sizeof(float), (void *)0);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                                      5 * sizeof(float),
                                      (void *)(3 * sizeof(float)));
        }
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
        screen_width = width;
        screen_height = height;
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

        sun_dir = glm::normalize(glm::vec3((screen_width >> 1) - xpos,
                                           -((screen_height >> 1) - ypos), -1000.0));
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