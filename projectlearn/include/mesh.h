#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

/**
 *******************************************************************************************
 *                                                                                         *
 *                                           Defination of Custom Structures                                  *
 *                                                                                         *
 *******************************************************************************************
 */

struct Vertex
{
    glm::vec3 Position;  // represent position of vertex in 3D space
    glm::vec3 Normal;    // represent the normal vector of the vertex
    glm::vec2 TexCoords; // represent texture coordinated of the vertex

    /**
     * Vector that will be used in advanced shading techniques like normal mapping
     * to achieve more detailed surface shading or complete tangent spaces
     */
    glm::vec3 Tangent;
    glm::vec3 Bitangent;

    /**
     * Stores the indices of bones and weight associated with it that will influence vertex
     * and vertex deformation during skeletan animation
     */
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

/**
 * Represent the materils properties assocaited with a 3D model surface for rendering
 */
struct Material
{
    /*Store ambient, diffuse and specular color of materials*/
    glm::vec4 Ka;
    glm::vec4 Kd;
    glm::vec4 Ks;

    /*Represent shininess, transparency and texture applied to of materials*/
    float shininess;
    float transparency;
    bool hasTexture;
};

/**
 * Representation of texture information for a 3D models
 */
struct Texture
{
    unsigned int id;
    string type;
    string path;
};

class Mesh
{
public:
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    Material mat;
    bool isBulb;
    bool isGlass;
    bool isWater;
    aiString name;
    unsigned int VAO;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, Material mat, aiString name)
    {
        /*Initializing the variable of Mesh Class*/
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->mat = mat;
        this->name = name;

        /**
         * Identifying certain type of mesh based on name,
         * and setting up bolean falags
         */
        if (strcmp(this->name.C_Str(), "light") == 0 || strcmp(this->name.C_Str(), "spotlight") == 0)
            this->isBulb = true;
        else
            this->isBulb = false;

        if (strcmp(this->name.C_Str(), "glass") == 0)
            this->isGlass = true;
        else
            this->isGlass = false;

        if (strcmp(this->name.C_Str(), "water") == 0)
            this->isWater = true;
        else
            this->isWater = false;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    // render the mesh
    void Draw(Shader &shader, bool isLighting, GLuint cubetex)
    {
        /* Enabling blending */
        if (isLighting && this->isGlass)
            glEnable(GL_BLEND);

        /*Configuring the material properties and related value for the mesh to be rendered*/
        if (isLighting)
        {
            shader.setVec4("material.ambient", mat.Ka);
            shader.setVec4("material.diffuse", mat.Kd);
            shader.setVec4("material.specular", mat.Ks);
            shader.setFloat("material.shininess", mat.shininess);

            shader.setBool("material.hasTexture", mat.hasTexture);
        }

        /**
         * Variable to keep track of the unit indices for different types of Texture
         * associated with the materials of the mesh
         */
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;

        for (unsigned int i = 1; i <= (textures.size()); i++)
        {
            /*Activating texture unit correponsing to the current texture*/
            glActiveTexture(GL_TEXTURE0 + i);

            /*Retrieves the texture type from current texture for texture vector*/
            string number;
            string name = textures[i - 1].type;

            /*Checking the texture type and incrementing the appropriate counter*/
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to string
            else if (name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to string
            else if (name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to string

            /*Retrieving the location of the shader variable corresponding to the current texture type and number*/
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);

            /*Activating texture unit again*/
            glActiveTexture(GL_TEXTURE0 + i);

            /*Binds the actual texture to the currently active texture unit*/
            glBindTexture(GL_TEXTURE_2D, textures[i - 1].id);
        }

        if (isLighting)
        {
            shader.setBool("isBulb", isBulb);
            shader.setBool("isGlass", isGlass);
            shader.setBool("isWater", isWater);
        }

        /* Rendering the Mesh using defined OpenGL VAO*/
        glBindVertexArray(VAO);                                                                      // Binds previously selected VAO
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0); // Initiates rendering process
        glBindVertexArray(0);                                                                        // Unbinds the previously selected VAO

        /*Setting everything back to defaults once configured.*/
        glActiveTexture(GL_TEXTURE0);

        /* Disabling gl_blend*/
        if (isLighting && this->isGlass)
            glDisable(GL_BLEND);
    }

private:
    /*Variable to store vertex array and element buffers*/
    unsigned int VBO, EBO;

    void setupMesh()
    {
        /**Generating VAO(Vertex Array Object), VBO(Vertex Buffer Object) and EBO(Element buffers objects)*/
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        /**
         * Binding VBO,EBO and VAO in preparation for specifying vertex
         * attribute pointer and renderingI
         */
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        /**
         * Allocating GPU memory for the vertex data and index data and efficiently sending
         * the custom vertex structures and index value from the vertices vector and indices vector to
         * OpenGL using their sequential memory layout.
         */
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        /**
         * Setting up vertex array pointer to index 0
         * Specify layout position attribute and associated it with the vertex structure
         */
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

        /**
         * Setting up vertex array pointer to index 1
         * Specify how normal attribute in interpreted by OpenGL
         */
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));

        /**
         * Setting up vertex array pointer to index 2
         * Specify how texture coordinates shouldbe interpreted by OpenGL
         */
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));

        /**
         * Setting up vertex array pointer to index 3
         * Specify how tangent attributes should be interpreted by OpenGL
         */
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Tangent));

        /**
         * Setting up vertex array pointer to index 4
         * Specify how bitangent attributes should be interpreted by OpenGL
         */
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Bitangent));

        /**
         * Setting up vertex array pointer to index 5
         * Specify how bone IDs attribute should be interpreted by OpenGL
         */
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, m_BoneIDs));

        /**
         * Setting up vertex array pointer to index 6
         * Specify how weights attribute should be interpreted by OpenGL
         */
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_Weights));

        /*Unbinds the currently bound Vertex Array Object*/
        glBindVertexArray(0);
    }
};
#endif
