#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "assimp_glm_helpers.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "mesh.h"
#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <math.h>
#include <vector>
#include <algorithm>
#include "animdata.h"

using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

/**
 * Custom Data types: Bulbs
 * Define the properties for representing light bulbs or light sources within 3D scene
 */
struct Bulbs
{
    glm::vec3 Color;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 position;
    glm::vec3 normal;
    float angle; // in degrees
    float constant;
    float linear;
    float exp;
};

class Model
{
public:
    // model data
    vector<Texture> textures_loaded; // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh> meshes;
    vector<Bulbs> bulbs;
    string directory;
    bool gammaCorrection;

    /* Loading the model from the specified files path*/
    Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    /* Draws the model, and thus all its meshes*/
    void Draw(Shader &shader, bool isLighting, GLuint cubetex)
    {
        if (bulbs.size() > 0)
        {
            shader.setInt("numBulbs", (int)bulbs.size());
            // std::cerr << bulbs.size() << std::endl;
            for (auto i = 0; i < bulbs.size(); ++i)
            {
                shader.setVec3((string("bulbs[") + to_string(i) + string("].base.position")).c_str(), bulbs[i].position);
                // shader.setVec3((string("bulbs[")+to_string(i)+string("].base.Color")).c_str(), bulbs[i].Color);
                shader.setVec3((string("bulbs[") + to_string(i) + string("].base.base.ambient")).c_str(), bulbs[i].ambient);
                shader.setVec3((string("bulbs[") + to_string(i) + string("].base.base.diffuse")).c_str(), bulbs[i].diffuse);
                shader.setVec3((string("bulbs[") + to_string(i) + string("].base.base.specular")).c_str(), bulbs[i].specular);
                shader.setFloat((string("bulbs[") + to_string(i) + string("].base.atten.constant")).c_str(), bulbs[i].constant);
                shader.setFloat((string("bulbs[") + to_string(i) + string("].base.atten.linear")).c_str(), bulbs[i].linear);
                shader.setFloat((string("bulbs[") + to_string(i) + string("].base.atten.exp")).c_str(), bulbs[i].exp);
                shader.setFloat((string("bulbs[") + to_string(i) + string("].cutoff")).c_str(), cos(bulbs[i].angle * 3.1415 / 180));
                shader.setVec3((string("bulbs[") + to_string(i) + string("].direction")).c_str(), bulbs[i].normal);
            }
        }
        else
        {
            shader.setInt("numBulbs", 0);
        }
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader, isLighting, cubetex);
    }

    auto &GetBoneInfoMap() { return m_BoneInfoMap; }
    int &GetBoneCount() { return m_BoneCounter; }

private:
    std::map<string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;

    /**
     *  Loads a model with supported ASSIMP extensions from file and
     * stores the resulting meshes in the meshes vector.
     * */
    void loadModel(string const &path)
    {
        /*Create an instance, used to read model file*/
        Assimp::Importer importer;

        /**
         * Attempt to load the model
         * aiProcess_Triangulate, aiProcess_CalcTangentSpace are the processing options
         * It indicates that the model's polygons should be converted in to triangules
         * and tangent space calculation should be performed for normal mapping.
         */
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

        /*Error handling*/
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }

        /*Extract the directory path from the given file path*/
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    /**
     * Recursively travering the node hierarchy of the imported model scene
     * and processing each node's mesh data and its children.
     */
    void processNode(aiNode *node, const aiScene *scene)
    {
        /* process each mesh located at the current node*/
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene.
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    /**
     * Initialize the bone data of vertex to default values.
     *
     * In skeletal animation, this function is called reset the bone influence
     * data of vertex to a neutral state
     */
    void SetVertexBoneDataToDefault(Vertex &vertex)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            vertex.m_BoneIDs[i] = -1;
            vertex.m_Weights[i] = 0.0f;
        }
    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
        glm::vec3 position; // for light bulbs
        glm::vec3 normal;

        /*Iterating through each vertex of the current aiMesh i.e *mesh */
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            /**
             * Creating vertex object of Vertex Class and
             * setting the bone data to default*/
            Vertex vertex;
            SetVertexBoneDataToDefault(vertex);

            /**
             * Declaring temp vector since assimp used its own vector class that doesnot
             * directly convert to glm's vec3 class, so we firt transfer data to this temp vector
             */
            glm::vec3 vector;

            /**
             * Extracting the vertex position of mesh data
             * and storing them in temp vector we have created
             */
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;

            position = vector;
            vertex.Position = vector;

            /**
             * If mesh have normal them extract them and
             * store them in temp vector Normal.
             */
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                normal = vector;
                vertex.Normal = vector;
            }

            /**
             * If mesh have Texture Coordinate then storing them.
             * Also, we will be storing Tangents and BitTangents.
             *
             * If mesh doesn't have Texture Coordinate then
             * it will store (0,0) as coordinates
             */
            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;

                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;

                /*Tangents*/
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;

                /* Bitangent*/
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            /*Adding the processed vertex to vertices vector*/
            vertices.push_back(vertex);
        }

        /**
         * Iterating through each vertex of the currnet aiMesh
         */
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            /*Creating currect face from mesh*/
            aiFace face = mesh->mFaces[i];

            /**
             * Iterating through each index within 'face'
             * And face indices vector are store in indices
             */
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        /**
         * Processing the materials associated with the mesh
         * and checking certain conditions related to the mesh
         * material's name.
         */
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        aiString meshName = material->GetName();

        bool condition1 = strcmp(meshName.C_Str(), "light") == 0;
        bool condition2 = strcmp(meshName.C_Str(), "spotlight") == 0;

        /*If meshName is either "light" and "spotlight"*/
        if (condition1 || condition2)
        {
            /**
             * Creating object of Bulb structure
             * and assigning value of position,specific RGB value
             * representing ambient, diffuse and specular properties of the bulb's
             * light
             */
            Bulbs bulb;
            bulb.position = position;
            bulb.ambient = glm::vec3(0.24725, 0.1995, 0.0745);
            bulb.diffuse = glm::vec3(0.75164, 0.60648, 0.22648);
            bulb.specular = glm::vec3(0.628281, 0.555802, 0.366065);

            /*Applying specific setting for each condition*/
            if (condition2)
            {
                bulb.normal = glm::vec3(10.0, -1.0, -1.0);
                bulb.angle = 10;
                bulb.constant = 1.f;
                bulb.linear = 0.4f;
                bulb.exp = 0.8f;
            }
            else if (condition1)
            {
                bulb.normal = glm::vec3(0.0, -7.0, 0.0);
                bulb.angle = 45;
                bulb.constant = 1.f;
                bulb.linear = 0.0f;
                bulb.exp = 0.2f;
            }

            /*Adding the 'bulb' structure to the 'bulbs' vector*/
            bulbs.push_back(bulb);
        }

        /**
         * Defining the Material object to store and manipulate
         * materials propertie during processing of the mesh.
         */
        Material mat;
        aiColor3D color;

        /**
         * Extracts the material properties and store them in 'mat' structure
         */
        float shininess, transparency;

        material->Get(AI_MATKEY_SHININESS, shininess);
        mat.shininess = shininess;
        material->Get(AI_MATKEY_COLOR_TRANSPARENT, transparency);

        if (strcmp(meshName.C_Str(), "glass") == 0)
            transparency = 0.9;

        material->Get(AI_MATKEY_COLOR_AMBIENT, color);
        mat.Ka = glm::vec4(color.r, color.g, color.b, transparency);

        material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        mat.Kd = glm::vec4(color.r, color.g, color.b, transparency);

        material->Get(AI_MATKEY_COLOR_SPECULAR, color);
        mat.Ks = glm::vec4(color.r, color.g, color.b, transparency);

        /** we assume a convention for sampler names in the shaders. Each diffuse texture should be named
         * as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
         * Same applies to other texture as the following list summarizes:
         * diffuse: texture_diffuseN
         * specular: texture_specularN
         * normal: texture_normalN
         */

        if (material->GetTextureCount(aiTextureType_DIFFUSE) == 0)
            mat.hasTexture = false;
        else
            mat.hasTexture = true;

        // 1. diffuse maps
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        // 2. specular maps
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        ExtractBoneWeightForVertices(vertices, mesh, scene);

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures, mat, meshName);
    }

    /**
     * Assigning bone data to a vertex in a mesh
     *
     * In skeletan animation, vertices are influenced by multiple
     * bones to create smooth deformations when animated
     */
    void SetVertexBoneData(Vertex &vertex, int boneID, float weight)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
        {
            /*Checking if current vertex contain bone or not*/
            if (vertex.m_BoneIDs[i] < 0)
            {
                vertex.m_Weights[i] = weight;
                vertex.m_BoneIDs[i] = boneID;
                break;
            }
        }
    }

    /**
     * Extracting bone weight information from mesh data and
     * associating it with the corresponding vertices in 'vertices' vector
     */
    void ExtractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh *mesh, const aiScene *scene)
    {
        /*Using refrence variable*/
        auto &boneInfoMap = m_BoneInfoMap;
        int &boneCount = m_BoneCounter;

        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            /*Initialize a variable to hold bone id*/
            int boneID = -1;

            /*Getting the name of current bone*/
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

            /**
             * Checking if bone name us present in boneInfoMap or not
             * If not present it creates a new entry in the map with bone
             * information
             */
            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                BoneInfo newBoneInfo;

                /*Assign boneCount to id*/
                newBoneInfo.id = boneCount;

                /* Coverting and Storing offset matrix of bone*/
                newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);

                /*Adding bone information to the map*/
                boneInfoMap[boneName] = newBoneInfo;
                boneID = boneCount;
                boneCount++;
            }
            else
            {
                boneID = boneInfoMap[boneName].id;
            }
            /*Ensuring bone ID is valid*/
            assert(boneID != -1);

            /*Storing bone weights and number of bone weights in variables*/
            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            /*Iterating over each bone weight*/
            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;

                /*Ensuring vertex index is within bounds*/
                assert(vertexId <= vertices.size());

                /*Set the bone influence data for the vertex*/
                SetVertexBoneData(vertices[vertexId], boneID, weight);
            }
        }
    }

    /**
     * Iterates through the textures associated with a materials,
     * Loads each texture if it hasn't been loaded before,
     * and keep track of loaded textures to avoid duplication
     *
     * It returns vector of 'Texture' structure containing the loaded textures.
     */
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
    {
        /*Initializing a vector to store loaded textures.*/
        vector<Texture> textures;

        /*Iterate through all the texture of given types in the materials*/
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            /*Variable to store texture path*/
            aiString str;

            /*Get the current texture path from current texture index*/
            mat->GetTexture(type, i, &str);

            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture

            /*Boolean variable to indicate skip loading of current texture or not*/
            bool skip = false;

            /**
             * Checks if the current texture has already been loaded or not
             */
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                /**
                 * If matching texture have been found among loaded texture,
                 * using that texture instead of loading a new one.
                 */
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }

            /*If texture hasn't been loaded already, load it*/
            if (!skip)
            {
                Texture texture;

                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();

                textures.push_back(texture);
                textures_loaded.push_back(texture); // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        return textures;
    }
};

inline unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
    /*Convert string path to a string, and combine directory and filename to get full path*/
    string filename = string(path);
    filename = directory + '/' + filename;

    /*Declaring and Generating OpenGL texture ID*/
    unsigned int textureID;
    glGenTextures(1, &textureID);

    /*Declaring variable to store texture properties*/
    int width, height, nrComponents;

    /*Loading image data*/
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

    /*If image is loaded successfully*/
    if (data)
    {
        /*Variable to store texture format*/
        GLenum format;

        /*Determine texture format based on number of components in the image data*/
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        /*Binds the texture to the active texture unit*/
        glBindTexture(GL_TEXTURE_2D, textureID);

        /*Upload texture data to OpenGL*/
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        /*Generate Mipmips for texture to enable automatic LOD selection*/
        glGenerateMipmap(GL_TEXTURE_2D);

        /*Setting texture wrapping and filtering options*/
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        /**
         * If image failed to load successfully,
         * prining error message and free loaded image data
         */
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    /*Return OpenGL texture ID*/
    return textureID;
}
#endif
