#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include "mapcamera.hpp"
#include "shader.hpp"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void process_input(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
GLuint load_texture(const char *);

void render_quad(GLuint vao, GLuint vbo);
void create_noise(unsigned char *data, int width, int height);

const int screen_width = 800;
const int screen_height = 600;

MapCamera camera{0.0f, 0.0f, 4.0f};

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

        // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                std::cout << "Failed to initialize GLAD\n";
                return -1;
        }

        // stbi_set_flip_vertically_on_load(true);

        glEnable(GL_DEPTH_TEST);

        Shader texShader{"src/shaders/TexShader.vs",
                         "src/shaders/TexShader.fs"};

        GLuint perlin_map;
        glGenTextures(1, &perlin_map);
        glBindTexture(GL_TEXTURE_2D, perlin_map);

        // unsigned char perlin_data[1024 * 1024];
        unsigned char perlin_data[1024 * 1024];
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
                texShader.set_uniform("perlin_map", 0);
                render_quad(quad_vao, quad_vbo);

                glfwSwapBuffers(window);
                glfwPollEvents();
        }

        glfwTerminate();
        return 0;
}

static int permutation[] = {
        151, 160, 137, 91,  90,  15,  131, 13,  201, 95,  96,  53,  194, 233,
        7,   225, 140, 36,  103, 30,  69,  142, 8,   99,  37,  240, 21,  10,
        23,  190, 6,   148, 247, 120, 234, 75,  0,   26,  197, 62,  94,  252,
        219, 203, 117, 35,  11,  32,  57,  177, 33,  88,  237, 149, 56,  87,
        174, 20,  125, 136, 171, 168, 68,  175, 74,  165, 71,  134, 139, 48,
        27,  166, 77,  146, 158, 231, 83,  111, 229, 122, 60,  211, 133, 230,
        220, 105, 92,  41,  55,  46,  245, 40,  244, 102, 143, 54,  65,  25,
        63,  161, 1,   216, 80,  73,  209, 76,  132, 187, 208, 89,  18,  169,
        200, 196, 135, 130, 116, 188, 159, 86,  164, 100, 109, 198, 173, 186,
        3,   64,  52,  217, 226, 250, 124, 123, 5,   202, 38,  147, 118, 126,
        255, 82,  85,  212, 207, 206, 59,  227, 47,  16,  58,  17,  182, 189,
        28,  42,  223, 183, 170, 213, 119, 248, 152, 2,   44,  154, 163, 70,
        221, 153, 101, 155, 167, 43,  172, 9,   129, 22,  39,  253, 19,  98,
        108, 110, 79,  113, 224, 232, 178, 185, 112, 104, 218, 246, 97,  228,
        251, 34,  242, 193, 238, 210, 144, 12,  191, 179, 162, 241, 81,  51,
        145, 235, 249, 14,  239, 107, 49,  192, 214, 31,  181, 199, 106, 157,
        184, 84,  204, 176, 115, 121, 50,  45,  127, 4,   150, 254, 138, 236,
        205, 93,  222, 114, 67,  29,  24,  72,  243, 141, 128, 195, 78,  66,
        215, 61,  156, 180};

static int fade(int t) { return t * t * t * (t * (t * 6 - 15) + 10); }

static int lerp(int t, int a, int b) { return a + t * (b - a); }

static int grad(int hash, int x, int y) {
        int h = hash & 15;
        int u = h < 8 ? x : y, v = h < 4 ? y : x;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

unsigned char perlin(int x, int y) {
        int X = x & 255, Y = y & 255;
        x -= X;
        y -= Y;
        int u = fade(x), v = fade(y);
        int A = permutation[X], AA = permutation[A], AB = permutation[A + 1],
            B = permutation[X + 1], BA = permutation[B],
            BB = permutation[B + 1];
        std::cout << A << " " << B << " " << AA << " " << AB << " " << BA << " "
                  << BB << "\n";
        return lerp(v,
                    lerp(u, grad(permutation[AA], x, y),
                         grad(permutation[BA], x - 1, y)),
                    lerp(u, grad(permutation[AB], x, y - 1),
                         grad(permutation[BB], x - 1, y - 1)));
}

void create_noise(unsigned char *data, int width, int height) {
        for (int x = 0; x < width; x++) {
                for (int y = 0; y < height; y++) {
                        data[y * height + x] = perlin(x, y);
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