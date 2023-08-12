#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <shader.h>
#include <camera.h>
#include <model.h>
#include <Animator.h>


#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, -35.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
 
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//paths

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



int main()
{
    // initialize glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // window creation
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "House Modeling", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Making the context of our window the main context of the current thread
    glfwMakeContextCurrent(window);


    // configure resize callback function to framebuffer_size_callback
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // load the address of the OpenGL function pointers which is OS-Specific
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }



    // configure global opengl state
    glEnable(GL_DEPTH_TEST);
    //configures blend function
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // build and compile shaders
    Shader lightingShader(lightingShadervPath, lightingShaderfPath);
    Shader animationShader(animationShadervPath, animationShaderfPath);
    Shader skyboxShader( skyboxShadervPath, skyboxShaderfPath ); // skybox shaders



    // load models
    Model ourModel(objFilePath);

	Model animationModel( animationFilePath );
    Animation danceAnimation(animationFilePath,&animationModel);
	Animator animator(&danceAnimation);



    glm::vec3 lightPos(0.0f, 200.0f, 100.0f);
    glm::vec3 lightDir(1.0f, 1.0f, 1.0f);
    // light properties
    glm::vec3 lightColor;
    lightColor.x = static_cast<float>(1.0f);
    lightColor.y = static_cast<float>(1.0f);
    lightColor.z = static_cast<float>(1.0f);


    // stbi_set_flip_vertically_on_load(true);

    // imgui
    const char *glsl_version = "#version 130";
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // skybox ------------------------------------------
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

    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
    glBindVertexArray(0);

    vector<std::string> faces;
    // faces.push_back(skyboxFilePath + "/right.png");
    // faces.push_back(skyboxFilePath + "/left.png");
    // faces.push_back(skyboxFilePath + "/top.png");
    // faces.push_back(skyboxFilePath + "/bottom.png");
    // faces.push_back(skyboxFilePath + "/front.png");
    // faces.push_back(skyboxFilePath + "/back.png");

    faces.push_back(skyboxFilePath + "/right.jpg");
    faces.push_back(skyboxFilePath + "/left.jpg");
    faces.push_back(skyboxFilePath + "/top.jpg");
    faces.push_back(skyboxFilePath + "/bottom.jpg");
    faces.push_back(skyboxFilePath + "/front.jpg");
    faces.push_back(skyboxFilePath + "/back.jpg");

    GLuint cubemapTexture;
    glGenTextures(1, &cubemapTexture);

    int imageWidth, imageHeight, nrComponents;
    unsigned char *image;

    glBindTexture(GL_TEXTURE_2D, cubemapTexture);

    // for getting skybox textures
    for (GLuint i = 0; i < faces.size(); i++)
    {
        image = stbi_load(const_cast<char *>(faces[i].c_str()), &imageWidth, &imageHeight, &nrComponents, 0);
        if (image)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, imageWidth, imageHeight, 0, format, GL_UNSIGNED_BYTE, image);
            stbi_image_free(image);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << skyboxFilePath << std::endl;
            stbi_image_free(image);
        }

        // Parameters
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    // glfwSwapInterval(8);
    // main render loop
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    float ambientIntensity = 0.55f;
    float diffuseIntensity = 0.25f;
    float specularIntensity = 0.f;


    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);
       

        //-----------------------------

        // imgui
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        animator.UpdateAnimation(deltaTime);

        // render
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //====================================================================================================================================
        // enable shader before setting uniforms
        glm::vec3 diffuseColor = lightColor   * glm::vec3(ambientIntensity); // decrease the influence
        glm::vec3 ambientColor = lightColor * glm::vec3(diffuseIntensity); // low influence
        glm::vec3 specularColor = lightColor * glm::vec3(specularIntensity); // low influence
        lightingShader.use();
        lightingShader.setVec3("sunLight.position", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setVec3("sunLight.direction",lightDir);

        lightingShader.setVec3("sunLight.base.ambient", ambientColor);
        lightingShader.setVec3("sunLight.base.diffuse", diffuseColor);
        lightingShader.setVec3("sunLight.base.specular", specularColor);


        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));     // it's a bit too big for our scene, so scale it down
        lightingShader.setMat4("model", model);


        // ourModel.Draw(ourShader);
        glBindVertexArray(skyboxVAO);
        // glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        ourModel.Draw( lightingShader, true, cubemapTexture );




        //===================================================================================================================================
        //animation part 
        animationShader.use();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        animationShader.setMat4("projection", projection);
        animationShader.setMat4("view", view);
        animationShader.setVec3("girlColor",lightColor);
        auto transforms = animator.GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i)
		animationShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

        // render the loaded model
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(11.09f, 2.105f, 10.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.f,1.f,1.f));	// it's a bit too big for our scene, so scale it down
        model = glm::rotate(model,glm::radians(90.0f),glm::vec3(0.f,1.f,0.f));
        animationShader.setMat4("model", model);
        animationModel.Draw(animationShader, false, cubemapTexture);



        //============================================================================================================================================
        // Skybox part
        glDepthFunc(GL_LEQUAL); // Change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        skyboxShader.setVec3("skyColor",lightColor);
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // Remove any translation component of the view matrix

        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Skybox cube
        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // Set depth function back to default

        // imgui
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

        // swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // imgui
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();
    }

    // terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessKeyboard(CRIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessKeyboard(CLEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
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

    // camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
