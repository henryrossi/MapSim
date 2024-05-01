#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <map>

#include "shaders/shader.hpp"
#include "mapcamera.hpp"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void process_input(GLFWwindow *window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int load_texture(const char *);

const int screen_width = 800;
const int screen_height = 600;

MapCamera camera{0.0f, 0.0f,  4.0f};

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

	GLFWwindow *window = glfwCreateWindow(screen_width, screen_height, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create glfw window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetScrollCallback(window, scroll_callback); 

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD\n";
		return -1;
	}

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        Shader shader{"src/shaders/shader.vs", "src/shaders/shader.fs"};
        Shader screenShader {"src/shaders/screenShader.vs", "src/shaders/screenShader.fs"};


        float cubeVertices[] = {
                // Back face
                -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
                0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
                -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
                // Front face
                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
                0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
                0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
                0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
                -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
                // Left face
                -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
                -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
                -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
                // Right face
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
                0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
                0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
                0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
                // Bottom face
                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
                0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
                0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
                0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
                // Top face
                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
                -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
        };
        float planeVertices[] = {
                // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
                5.0f, -0.51f,  5.0f,  2.0f, 0.0f,
                -5.0f, -0.51f,  5.0f,  0.0f, 0.0f,
                -5.0f, -0.51f, -5.0f,  0.0f, 2.0f,

                5.0f, -0.51f,  5.0f,  2.0f, 0.0f,
                -5.0f, -0.51f, -5.0f,  0.0f, 2.0f,
                5.0f, -0.51f, -5.0f,  2.0f, 2.0f								
        };
        float transparentVertices[] = {
                // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
                0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
                0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
                1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

                0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
                1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
                1.0f,  0.5f,  0.0f,  1.0f,  0.0f
        };

        unsigned int cubeVAO, cubeVBO;
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        unsigned int planeVAO, planeVBO;
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        unsigned int transparentVAO, transparentVBO;
        glGenVertexArrays(1, &transparentVAO);
        glGenBuffers(1, &transparentVBO);
        glBindVertexArray(transparentVAO);
        glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        unsigned int FBO;
        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        unsigned int textureColorbuffer;
        glGenTextures(1, &textureColorbuffer);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        // attach it to currently bound framebuffer object
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0); 

        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);  
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);  

        float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
                // positions   // texCoords
                -1.0f,  1.0f,  0.0f, 1.0f,
                -1.0f, -1.0f,  0.0f, 0.0f,
                1.0f, -1.0f,  1.0f, 0.0f,

                -1.0f,  1.0f,  0.0f, 1.0f,
                1.0f, -1.0f,  1.0f, 0.0f,
                1.0f,  1.0f,  1.0f, 1.0f
        };
        unsigned int screenVAO, screenVBO;
        glGenVertexArrays(1, &screenVAO);
        glGenBuffers(1, &screenVBO);
        glBindVertexArray(screenVAO);
        glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        unsigned int cubeTexture = load_texture("src/textures/marble.jpg");
        unsigned int planeTexture = load_texture("src/textures/metal.png");
        unsigned int windowTexture = load_texture("src/textures/blending_transparent_window.png");
 
        
        shader.use();
        shader.set_uniform("texture1", 0);
        screenShader.use();
        screenShader.set_uniform("screenTexture", 0);

        std::vector<glm::vec3> windows;
        windows.push_back(glm::vec3(-1.5f,  0.0f, -0.48f));
        windows.push_back(glm::vec3( 1.5f,  0.0f,  0.51f));
        windows.push_back(glm::vec3( 0.0f,  0.0f,  0.7f));
        windows.push_back(glm::vec3(-0.3f,  0.0f, -2.3f));
        windows.push_back(glm::vec3( 0.5f,  0.0f, -0.6f));  

	while (!glfwWindowShouldClose(window)) {
                float currentFrame = glfwGetTime();
                deltaTime = currentFrame - lastFrame;
                lastFrame = currentFrame;

		process_input(window);
                
                glBindFramebuffer(GL_FRAMEBUFFER, FBO);
                glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
                glEnable(GL_DEPTH_TEST);

                glm::mat4 view = camera.GetViewMatrix(); 
                glm::mat4 proj = glm::perspective(glm::radians(camera.FOV), 
                                                  (float)screen_width/(float)screen_height,
                                                  0.1f, 100.0f);
                shader.use();
                shader.set_uniform("view", view);
                shader.set_uniform("projection", proj);
                glm::mat4 model{1.0f};

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, planeTexture);
                glBindVertexArray(planeVAO);
                shader.set_uniform("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);

                glBindTexture(GL_TEXTURE_2D, cubeTexture);
                glBindVertexArray(cubeVAO);
                model = glm::mat4{1.0f};
                model = glm::translate(model, glm::vec3{-1.0f, 0.0f, -1.0f});
                shader.set_uniform("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                model = glm::mat4{1.0f};
                model = glm::translate(model, glm::vec3{2.0f, 0.0f, 0.0f});
                shader.set_uniform("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);

                glBindVertexArray(transparentVAO);
                glBindTexture(GL_TEXTURE_2D, windowTexture);
                std::map<float, glm::vec3> sorted;
                for (unsigned int i = 0; i < windows.size(); i++) {
                        float distance = glm::length(camera.Position - windows[i]);
                        sorted[distance] = windows[i];
                }
                for(auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
                        model = glm::mat4(1.0f);
                        model = glm::translate(model, it->second);				
                        shader.set_uniform("model", model);
                        glDrawArrays(GL_TRIANGLES, 0, 6);
                }  


                glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
                glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
                glClear(GL_COLOR_BUFFER_BIT);

                screenShader.use();  
                glBindVertexArray(screenVAO);
                glDisable(GL_DEPTH_TEST);
                glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
                glDrawArrays(GL_TRIANGLES, 0, 6);  


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteVertexArrays(1, &planeVAO);
        glDeleteVertexArrays(1, &transparentVAO);
        glDeleteBuffers(1, &cubeVBO);
        glDeleteBuffers(1, &planeVBO);
        glDeleteBuffers(1, &transparentVBO);
        glDeleteFramebuffers(1, &FBO);  

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window) {
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
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

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
        camera.ProcessMouseScroll(yoffset);
}

unsigned int load_texture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
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
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}