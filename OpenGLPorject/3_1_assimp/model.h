#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <map>
#include "mesh.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

class Model
{
public:
    Model(const char* path);

    void Draw(MyShader shader);

private:
    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    unsigned int textureFromFile(const char* path, const std::string& directory, bool gamma );

private:
    std::vector<Mesh> meshs;
    std::string directory;
    std::map<std::string, Texture> textures_loaded;
};

#endif // MODEL_H
