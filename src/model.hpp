#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <string>
#include <vector>


#include "shader.hpp"

struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
};

struct Texture {
        unsigned int id;
        std::string type;
        std::string path;
};

class Mesh {
      public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
             std::vector<Texture> textures);
        void Draw(Shader &shader);

      private:
        unsigned int VAO, VBO, EBO;

        void setupMesh();
};

class Model {
      public:
        Model(char *path);
        void Draw(Shader &shader);

      private:
        std::vector<Mesh> meshes;
        std::vector<Texture> textures_loaded;
        std::string directory;

        void loadModel(std::string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat,
                                                  aiTextureType type,
                                                  std::string typeName);
};

#endif /* MODEL_H */