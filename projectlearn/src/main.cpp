/**
 *******************************************************************************************
 *                                                                                         *
 *                                           HEADER FILES                                  *
 *                                                                                         *
 *******************************************************************************************
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <shader.h>
#include <camera.h>
#include <model.h>
#include <Animator.h>

#include <iostream>

/**
 ***********************************************************************************************
 *                                                                                             *
 *                                   GLOBAL VARIABLES                                          *
 *                                                                                             *
 ***********************************************************************************************
 */
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, -35.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

const char *lightingShadervPath =
    "/home/susheel/Desktop/House-Modeling-CG"
    "/projectlearn/res/shaders/lighting.vs";
const char *lightingShaderfPath =
    "/home/susheel/Desktop/House-Modeling-CG"
    "/projectlearn/res/shaders/lighting.fs";
const char *objFilePath =
    "/home/susheel/Desktop/House-Modeling-CG"
    "/projectlearn/res/models/house.obj";
std::string skyboxFilePath =
    "/home/susheel/Desktop/House-Modeling-CG"
    "/projectlearn/res/models/textures/Cubemaps";
const char *skyboxShadervPath =
    "/home/susheel/Desktop/House-Modeling-CG"
    "/projectlearn/res/shaders/skybox.vs";
const char *skyboxShaderfPath =
    "/home/susheel/Desktop/House-Modeling-CG"
    "/projectlearn/res/shaders/skybox.fs";
const char *animationFilePath =
    "/home/susheel/Desktop/House-Modeling-CG"
    "/projectlearn/res/models/Sitting.dae";
const char *animationShadervPath =
    "/home/susheel/Desktop/House-Modeling-CG"
    "/projectlearn/res/shaders/animation.vs";
const char *animationShaderfPath =
    "/home/susheel/Desktop/House-Modeling-CG"
    "/projectlearn/res/shaders/animation.fs";

/**
 ******************************************************************************************
 *                                                                                        *
 *                                Callback Functions                                      *
 *                                                                                        *
 ******************************************************************************************
 */

/**
 * Callbacks functions:
 *  * whenever size of the frame buffer associated with the window changes
 *  * wheneve cursor position changes within the window
 *  * wheneve the user scrolls using a scrolling input devices
 */

/**
 * This function is used to handle user input, such as
 * keyboard controls, in OpenGL application
 */
void processInput(GLFWwindow *window)
{

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessKeyboard(CRIGHT, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessKeyboard(CLEFT, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

/**
 * A Callback function that used to adjust the OpenGL viewport to match the new dimension of the resized window
 *
 */
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

/**
 *  A callback used to handle mouse movement events within your OpenGL application
 */
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
}

/**
 * A Callback Function used to handle mouse scroll events within your OpenGL application.
 */
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

int main()
{
    /**
     *****************************************************************************************
     *                                                                                       *
     *                            Setting up GLFW Library                                    *
     *                                                                                       *
     ****************************************************************************************
     */

    /**
     * Initialize GLFW library
     * Set the major version of OpenGL that we will be using, i.e 3
     * Set the minor version of OpenGL that we will be using, i.e 3
     * Set the OpenGL profile that we will be using,
     *  And GLFW_OPENGL_CORE_PROFILE indicates we want to used core profile of OpenGL
     *  that include modern OpenGL feaatures and exclude deprecated functionality.
     */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Creating GLFW Window with above mention screen width and height*/
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "House Modeling", NULL, NULL);

    /* Checking if window is successfully created or not*/
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    /* Specifying GLFW window OpenGL context current for rendering*/
    glfwMakeContextCurrent(window);

    /*Setting up callback functions for various events using the GLFW Library*/
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    /**
     *************************************************************************************************
     *                                                                                               *
     *                                     Setting up GLAD Library                                   *
     *                                                                                               *
     *************************************************************************************************
     */

    /**
     * Initializing the GLAD library
     *  * It is used to manage OpenGL function pointer.
     */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    /*Enabling the depth testing in OpenGL*/
    glEnable(GL_DEPTH_TEST);

    /* Sets the blending function for OpenGL*/
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Commonly used blending function for acheiving smooth transparency and alpha blending effects

    /**
     *******************************************************************************************************
     *                                                                                                     *
     *                                     Setting up Shader Objects                                       *
     *                                                                                                     *
     *******************************************************************************************************
     */
    /**
     * Creating Shader object from their respective
     * fragment shader(fs) and vertices shader(vs) files
     */
    Shader lightingShader(lightingShadervPath, lightingShaderfPath);
    Shader animationShader(animationShadervPath, animationShaderfPath);
    Shader skyboxShader(skyboxShadervPath, skyboxShaderfPath);

    /**
     ********************************************************************************************************
     *                                                                                                      *
     *                                      Setting up Model Objects                                        *
     *                                                                                                      *
     ********************************************************************************************************
     */

    /**
     * Creating Model class object called ourModel
     * And, loading the model object we have created in blender in that object
     */
    Model ourModel(objFilePath);

    /**
     * Loading animation model and its animation files
     */
    Model animationModel(animationFilePath);
    Animation danceAnimation(animationFilePath, &animationModel);
    Animator animator(&danceAnimation);

    /**
     ***********************************************************************************************************
     *                                                                                                         *
     *                                        Setting up ImGUI Library                                         *
     *                                                                                                         *
     ***********************************************************************************************************
     */
    /**
     * Integrating ImGui(Dear ImGui) library with OpenGL
     */
    const char *glsl_version = "#version 130"; // Defining glsl(OpenGL Shading Language) version to be used
    ImGui::CreateContext();                    // Initialize ImGui context, which holds the internal state used by ImGui
    ImGuiIO &io = ImGui::GetIO();              // It is used to communicate input and output between ImGui and our application
    (void)io;
    ImGui::StyleColorsDark();                   // Sets the ImGui style to the "dark" color scheme.
    ImGui_ImplGlfw_InitForOpenGL(window, true); // Initialize ImGui integation for GLFW
    ImGui_ImplOpenGL3_Init(glsl_version);       // Initialize ImGui integration for OpenGL 3.0

    /**
     *********************************************************************************************************
     *                                                                                                       *
     *                        Configuring Skybox and binding with OpenGL                                     *
     *                                                                                                       *
     *********************************************************************************************************
     */
    /**
     * Defining an array called skyboxVertices that contains the vertex position for a sky box
     */
    GLfloat skyboxVertices[] = {
        // Positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f};

    /**
     * Defining variable that will store OpenGL handles that
     * corresponds to Vertex Array Objects(VAOs) and Vertex Buffer Objects(VBO)
     */
    GLuint skyboxVAO, skyboxVBO;

    /**
     *  Generating handles for VBO and VAO
     *  These handles are used to manage the vertex data and its configuration with OpenGL rendering pipeline
     */
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    /**
     * Binding the generated VAO and VBO for configuring and
     * loading vertex data into the OpenGL rendering pipeline
     */
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    /**
     * Configuring the vertex attributes of the skybox vertices.
     * It prepares the OpenGl pipeline to properly interpret the vertex data during rendering.
     */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
    glBindVertexArray(0);

    /**
     * Creating a vector namede 'faces' and
     * populating it with file paths to texture
     * images for each face of skybox
     */
    vector<std::string> faces;
    faces.push_back(skyboxFilePath + "/right.jpg");
    faces.push_back(skyboxFilePath + "/left.jpg");
    faces.push_back(skyboxFilePath + "/top.jpg");
    faces.push_back(skyboxFilePath + "/bottom.jpg");
    faces.push_back(skyboxFilePath + "/front.jpg");
    faces.push_back(skyboxFilePath + "/back.jpg");

    /**
     * Generating a texture handle for a cubemap texture using OpenGL functions
     *
     * A cubemap texture is a special type of texture used for creating environment maps or skyboxes.
     */
    GLuint cubemapTexture;
    glGenTextures(1, &cubemapTexture);

    /* Variable for storing information about loaded images*/
    int imageWidth, imageHeight, nrComponents;
    unsigned char *image;

    /* Binding cube map texture with OpenGL texture handle 'cubemapTexture'*/
    glBindTexture(GL_TEXTURE_2D, cubemapTexture);

    /**
     * Loading and configuring images as a cubemap texture for a skybox
     */
    for (GLuint i = 0; i < faces.size(); i++)
    {
        /* Loading images*/
        image = stbi_load(const_cast<char *>(faces[i].c_str()), &imageWidth, &imageHeight, &nrComponents, 0);

        if (image)
        {
            /* Determining the format of cubemap texture based on number of color components in image*/
            GLenum format;
            if (nrComponents == 1)
            {
                format = GL_RED;
            }
            else if (nrComponents == 3)
            {
                format = GL_RGB;
            }
            else if (nrComponents == 4)
            {
                format = GL_RGBA;
            }

            /*Sending the image data to OpenGL pipeline to populate appropriate face of cubemap*/
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, imageWidth, imageHeight, 0, format, GL_UNSIGNED_BYTE, image);
            stbi_image_free(image);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << skyboxFilePath << std::endl;
            stbi_image_free(image);
        }

        /* Setting filtering and wrapping texture parameter to cubemap's faces */
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        /* Finally cubemap is unbound os that OpenGL texture operation won't accidentally modify texture*/
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    /**
     * Flipping the loaded image vertically as OpenGl expect the origin
     * to be at the bottom-left corner but image are stored with origin
     * at top-left corner.
     */
    stbi_set_flip_vertically_on_load(true);

    /**
     *****************************************************************************************************
     *                                                                                                   *
     *                           Lighting Setup and binding with OpenGL                                  *
     *                                                                                                   *
     *****************************************************************************************************
     */

    /**
     * Defining vectors to store 3D coordinates or direction using GLM(OpenGL Mathematices)
     * lightPos and lightDir are used to provide information about the position and direction of light sources in our scene
     */
    glm::vec3 lightPos(0.0f, 200.0f, 100.0f);
    glm::vec3 lightDir(1.0f, 1.0f, 1.0f);

    /**
     * Defining a vector name lightColor and assigning individual components
     * Here, we are setting it to white color.
     */
    glm::vec3 lightColor;
    lightColor.x = static_cast<float>(1.0f);
    lightColor.y = static_cast<float>(1.0f);
    lightColor.z = static_cast<float>(1.0f);

    /**
     * Defining variable to control ambient, diffuse and specular intensity
     *
     *      Ambient lighting is used to stimulate the light that gets
     *      scattered and reflected by surfaces and objects in an environment
     *
     *      Diffuse lighting represents the directional lighting that occurs
     *      when light hits a surface at an angle and gets scattered in all directions.
     *      It is responsible for creaeting brightness and shadow on the surface
     *
     *      Specular Lighting represents the highlights on shiny or reflective surfaces.
     */
    float ambientIntensity = 0.55f;
    float diffuseIntensity = 0.55f;
    float specularIntensity = 0.55f;

    /**
     *******************************************************************************************************
     *                                                                                                     *
     *                                          Application Loop                                           *
     *                                                                                                     *
     *******************************************************************************************************
     */
    while (!glfwWindowShouldClose(window))
    {
        /*Clear the color and depth buffers of the OpenGL rendering context at the beginning of each frame*/
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /**
         *******************************************************************************************************
         *                                                                                                     *
         *           Handling, Processing Input Events and Updating Scene based on Input received              *
         *                                                                                                     *
         *******************************************************************************************************
         */

        /* It is used to process the user inputs nad modify the state of application based on the input*/
        processInput(window);

        /*Create GUI within our application*/
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        /**
         * Calculating  the time elapsed between frames in rendering loop
         *
         * It is crucial for controlling animation and updating object position
         * in a smooth and frame-rate-independant manner.
         */
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        /*Updating and manage animations of animator object*/
        animator.UpdateAnimation(deltaTime);

        /* Calculating diffuse, ambient and specular color for lighting in scene*/
        glm::vec3 diffuseColor = lightColor * glm::vec3(ambientIntensity);
        glm::vec3 ambientColor = lightColor * glm::vec3(diffuseIntensity);
        glm::vec3 specularColor = lightColor * glm::vec3(specularIntensity);

        /**
         *******************************************************************************************************
         *                                                                                                     *
         *                                          Lighting Shader                                            *
         *                                                                                                     *
         *******************************************************************************************************
         */
        /* Activating "lightingShader"*/
        lightingShader.use();

        /* Setting the sunlight position and direction of the light */
        lightingShader.setVec3("sunLight.position", lightPos);
        lightingShader.setVec3("sunLight.direction", lightDir);

        /*Setting the view position*/
        lightingShader.setVec3("viewPos", camera.Position);

        /*Seting the ambient, diffuse and specular lighting properties of light sources*/
        lightingShader.setVec3("sunLight.base.ambient", ambientColor);
        lightingShader.setVec3("sunLight.base.diffuse", diffuseColor);
        lightingShader.setVec3("sunLight.base.specular", specularColor);

        /* Calculating the projection and view matrices for camera in 3D scene*/
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        /*Setting the projection and view matrix in "lightingShader"*/
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        /*Manipulating the model matrix for an object in the scene*/
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));     // it's a bit too big for our scene, so scale it down

        /**
         * Setting the model matrix as a uniform in "lightingShader"
         *
         * It allows shader to apply the transformation defined by model matrix
         * to the vertices of the object we are rendering
         */
        lightingShader.setMat4("model", model);

        /*Binding the VAO associated with the skybox to the OpenGL context*/
        glBindVertexArray(skyboxVAO);

        /* Binding the previously created cubemap texture to the OpenGL context*/
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

        /*Rendering our model using "lightingShader","cubemapTexture" and applying lighting calculations during rendering*/
        ourModel.Draw(lightingShader, true, cubemapTexture);

        /**
         *******************************************************************************************************
         *                                                                                                     *
         *                                          Animation Shader                                           *
         *                                                                                                     *
         *******************************************************************************************************
         */

        /*Activating animationShader that we have defined above*/
        animationShader.use();

        /*Updating the "projection" and "view" matrix with new values*/
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();

        /*Setting various uniforms to control how the animation is being rendered*/
        animationShader.setMat4("projection", projection);
        animationShader.setMat4("view", view);
        animationShader.setVec3("girlColor", lightColor);

        /*Retrieving the final bone transformation matrices from animator objects*/
        auto transforms = animator.GetFinalBoneMatrices();

        /*looping through 'transforms' array which contain final bone transformations and setting the uniform*/
        for (int i = 0; i < transforms.size(); ++i)
        {
            animationShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        }

        /*Manipulating the 'model' matrix for specific object in scene*/
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(11.09f, 2.105f, 10.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.f, 1.f, 1.f));             // it's a bit too big for our scene, so scale it down
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.f, 1.f, 0.f));

        /*Set the "model" matrix as uniform in "animationShader"*/
        animationShader.setMat4("model", model);

        /*Render the animationModel using animationShader and cubermapTexture, but lighting calculation won't be applied during rendering*/
        animationModel.Draw(animationShader, false, cubemapTexture);

        /*Setting the depth comparision function, fragment will be visible if depth value is less than or equal to stored value */
        glDepthFunc(GL_LEQUAL);

        /**
         *******************************************************************************************************
         *                                                                                                     *
         *                                          Skybox Shader                                              *
         *                                                                                                     *
         *******************************************************************************************************
         */

        /*Activating skyboxShader*/
        skyboxShader.use();

        /*Sets a uniform named "skyColor" in the "skyboxShader" with the value of the lightColor vector*/
        skyboxShader.setVec3("skyColor", lightColor);

        /*Remove any translation components from the 'view' matrix*/
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));

        /*Setting 'projection' and 'view' matrix as uniform in skyShader program*/
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        /*Binding Vertex Array Object and cubemapTexture before rendering the skybox*/
        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

        /**
         * Render the vertices currently bounded in Vertex Array Object and the active shader program
         * * GL_TRIANGLES : We are telling OpenGL to treat vertices as individual triangles
         * * 0 : Starting index of the vertices with in VAO. Since we are using whole buffer,so we need to start from index 0.
         * * 36 : number of vertices to render. As each cube made of two triangle(Six Vertices) and there are six faces in total, so we have 36 vertices
         */
        glDrawArrays(GL_TRIANGLES, 0, 36);

        /*Unbound bounded vertex array object from OpenGL context*/
        glBindVertexArray(0);

        /*Restoring the deep comparision function back to default*/
        glDepthFunc(GL_LESS);

        /**
         *******************************************************************************************************
         *                                                                                                     *
         *                                          ImGUI Debug Bar Setup                                           *
         *                                                                                                     *
         *******************************************************************************************************
         */

        /**
         * Creating a user interface using ImGUI for controlling certain parameters
         * and displaying application statistics within our graphics appliction
         */
        {
            ImGui::SliderFloat3("LightPos", &lightPos.x, -400.f, 400.f);
            ImGui::SliderFloat3("LightColor", &lightColor.x, 0.0f, 1.0f);
            ImGui::SliderFloat("LightColor-ambientIntensity", &ambientIntensity, 0.0f, 1.0f);
            ImGui::SliderFloat("LightColor-diffuseIntensity", &diffuseIntensity, 0.0f, 1.0f);
            ImGui::SliderFloat("LightColor-specularIntensity", &specularIntensity, 0.0f, 1.0f);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        /*Swaps the front and back buffers of the window*/
        glfwSwapBuffers(window);

        /*Process events in the event queue*/
        glfwPollEvents();
    }

    /**
     *******************************************************************************************************
     *                                                                                                     *
     *                                          Cleanup                                                    *
     *                                                                                                     *
     *******************************************************************************************************
     */

    /*Shutting down and cleaning ImGUI*/
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();
    }

    /*Cleaning up and terminating GLFW library*/
    glfwTerminate();

    return 0;
}
