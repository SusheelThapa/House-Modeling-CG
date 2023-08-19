#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int ID;


    Shader(const char *vertexPath, const char *fragmentPath)
    {

        /**
         *********************************************************************************************************
         *                                                                                                       *
         *                                  Reading and Pre-Processing Shaders files                             *
         *                                                                                                       *
         *********************************************************************************************************
         */
        /*Defining the temporary variable that will be used*/
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;


        /*Setting the exception flags for the input file streams*/
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            /*Opening the shader files*/
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);

            /* Loaining the content fo shader into stringstream variable*/
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            /*Closing the shader files which have been opened*/
            vShaderFile.close();
            fShaderFile.close();

            /*Extracting content of stream object and storing them as string*/
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure &e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        }

        /*Converting string into C String string*/
        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();

        /**
         *********************************************************************************************************
         *                                                                                                       *
         *                                   Loading and Compiling Shaders                                       *
         *                                                                                                       *
         *********************************************************************************************************
         */

        unsigned int vertex, fragment;

        /*Creating vertex shader object*/
        vertex = glCreateShader(GL_VERTEX_SHADER);

        /*Setting the source code of the shader*/
        glShaderSource(vertex, 1, &vShaderCode, NULL);

        /*Compiling the attached shader code into machine code, executed by GPU*/
        glCompileShader(vertex);

        /*Checking if there were any compilation errors*/
        checkCompileErrors(vertex, "VERTEX");

        /*-------------------------------------------------------------------*/

        /*Creating vertex shader object*/
        fragment = glCreateShader(GL_FRAGMENT_SHADER);

        /*Setting the source code of the shader*/
        glShaderSource(fragment, 1, &fShaderCode, NULL);

        /*Compiling the atached shader code into machine mode, executed by GPU*/
        glCompileShader(fragment);

        /*Checking if there were any compilation errors*/
        checkCompileErrors(fragment, "FRAGMENT");

        /**
         *********************************************************************************************************
         *                                                                                                       *
         *                                   Creating Shader Program      I                                      *
         *                                                                                                       *
         *********************************************************************************************************
         */

        /*Creating shader program object*/
        ID = glCreateProgram();

        /*Attaching compiled shader(fragment,vertex) to shader program*/
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);

        /*Combining shader objects into complete shader program used for rendering*/
        glLinkProgram(ID);

        /*Checking if there were any linking errors or not*/
        checkCompileErrors(ID, "PROGRAM");

        /*Free up resources used by vertex and fragement shader as they have been attached to shader program*/
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    /**
     *********************************************************************************************************
     *                                                                                                       *
     *                                            Member Functions                                           *
     *                                                                                                       *
     *********************************************************************************************************
     */

    /*Activate the shader program for rendering*/
    void use() const
    {
        glUseProgram(ID);
    }

    /*Set the boolean uniform value in the shader program*/
    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    /*Set an integr uniform value in the shader program*/
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    /*Set floating-point uniform value in the shader program*/
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    /*Sets 2D vector uniform values in the shader program*/
    void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }

    /*Sets 3D uniform values in the shader program*/
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    /*Sets 4D uniform values in the shader program*/
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }

    /* Sets 2X2 matrix uniform value in the shader program*/
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }


    /*Sets 3X3 uniform value in the shader program*/
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    /*Sets 4X4 uniform value in the shader program*/
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    /**
     *********************************************************************************************************
     *                                                                                                       *
     *                                            Utility Functions                                           *
     *                                                                                                       *
     *********************************************************************************************************
     */

    /*Check for compilation or linking erros in the shader programs*/
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;

            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);

                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
#endif
