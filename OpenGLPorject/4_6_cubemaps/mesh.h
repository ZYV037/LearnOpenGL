#ifndef MESH_H
#define MESH_H

#include "glad/glad.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "myshader.h"

#include "assimp/types.h"

#include <vector>

struct Vertex{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Texture{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex>& vertices_, std::vector<unsigned int>& indices_, std::vector<Texture>& textures_);

    void Draw(MyShader shader);

private:
    unsigned int VAO, VBO, EBO;
    void setupMesh();

};

#endif // MESH_H
