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
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 tex_coords;
};

/**
 * Textures currently are required to follow a certain naming cnvention.
 * For diffuse textures,
 *      texture_diffuseN where this is the Nth diffuse texture.
 * And for specular textures,
 *      texture_specularN where this is the Nth specular texture.
*/
struct Texture {
        GLuint id;
        std::string type;
        std::string path;
};

class Mesh {
      public:
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;

        Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices,
             std::vector<Texture> textures);
        void draw(Shader &shader);

      private:
        unsigned int VAO, VBO, EBO;

        void setup_mesh();
};

class Model {
      public:
        Model(const char *path);
        void draw(Shader &shader);

      private:
        std::vector<Mesh> meshes;
        std::vector<Texture> textures_loaded;
        std::string directory;

        void load_model(std::string path);
        void process_node(aiNode *node, const aiScene *scene);
        Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> load_material_textures(aiMaterial *mat,
                                                  aiTextureType type,
                                                  std::string typeName);
};

#endif /* MODEL_H */