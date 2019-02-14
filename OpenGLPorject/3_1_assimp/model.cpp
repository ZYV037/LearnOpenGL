#include "model.h"
#include <iostream>
#include "stb_image.h"

Model::Model(const char* path)
{
    loadModel(path);
}

void Model::Draw(MyShader shader)
{
    for(unsigned int i = 0; i < meshs.size(); ++i)
    {
        meshs[i].Draw(shader);
    }
}

void Model::loadModel(std::string path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace );

    if(nullptr == scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || nullptr == scene->mRootNode )
    {
        std::cout << "Error::Assimp::" << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}


void Model::processNode(aiNode* node, const aiScene* scene)
{
    for(unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshs.push_back(processMesh(mesh, scene));
    }

    for(unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int > indices;
    std::vector<Texture> textures;

    for( unsigned int i = 0; i < mesh->mNumVertices; ++i )
    {
        Vertex vertex;

        vertex.Position.x = mesh->mVertices[i].x;
        vertex.Position.y = mesh->mVertices[i].y;
        vertex.Position.z = mesh->mVertices[i].z;

        vertex.Normal.x = mesh->mNormals[i].x;
        vertex.Normal.y = mesh->mNormals[i].y;
        vertex.Normal.z = mesh->mNormals[i].z;

        if(mesh->mTextureCoords[0])
        {
            vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
        }
        else
        {
            vertex.TexCoords.x = 0.0f;
            vertex.TexCoords.y = 0.0f;
        }

        vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; ++i )
    {
        const aiFace& face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }


    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end() );
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        bool skip = false;
        if( textures_loaded.find(str.C_Str()) != textures_loaded.end() )
        {
            textures.push_back( textures_loaded[str.C_Str()] );
            skip = true;
            continue;
        }

        Texture texture;
        texture.id = textureFromFile( str.C_Str(), directory, false );
        texture.type = typeName;
        texture.path = str.C_Str();
        textures.push_back(texture);
        textures_loaded[texture.path] = (texture);
    }

    return textures;
}

unsigned int Model::textureFromFile(const char* path, const std::string& directory, bool gamma )
{
    std::string fileName = directory + '/' + path;

    int width, height, nrComponents;
    unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrComponents, 0);
    if( nullptr == data)
    {
        std::cout << "Error::stbi_load texture file failed! " << fileName.c_str() << std::endl;
        return 0;
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);

    GLenum format;
    switch(nrComponents)
    {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return textureID;

}






























