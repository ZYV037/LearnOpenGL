#include "mesh.h"

Mesh::Mesh(std::vector<Vertex>& vertices_, std::vector<unsigned int>& indices_, std::vector<Texture>& textures_)\
    :vertices(vertices_),indices(indices_), textures(textures_)
{
    setupMesh();
}

void Mesh::Draw(MyShader shader)
{
    unsigned int diffuserNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;

    for(unsigned int i = 0; i < textures.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number;
        std::string name = textures[i].type;

        if(name == "texture_diffuse")
        {
            number = std::to_string(diffuserNr++);
        }
        else if(name == "texture_specular")
        {
            number = std::to_string(specularNr++);
        }
        else if(name == "texture_normal")
        {
            number = std::to_string(normalNr++); // transfer unsigned int to stream
        }
        else if(name == "texture_height")
        {
            number = std::to_string(heightNr++);
        }

        shader.setInt((name+number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData( GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), &vertices[0], GL_STATIC_DRAW );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0 );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    glBindVertexArray(0);
}
