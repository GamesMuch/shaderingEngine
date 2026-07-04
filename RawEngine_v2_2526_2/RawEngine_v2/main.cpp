#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
// #include <glm/detail/func_geometric.inl>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

#include "core/mesh.h"
#include "core/assimpLoader.h"
#include "core/texture.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp> // to extract columns from matrices

#include "../../../Documents/ShaderEngine1/vcpkg/installed/x64-windows/include/glm/vec3.hpp"
#include "../../../Documents/ShaderEngine1/vcpkg/installed/x64-windows/include/glm/detail/setup.hpp"

//#define MAC_CLION
#define VSTUDIO

#ifdef MAC_CLION
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#endif

#ifdef VSTUDIO
// Note: install imgui with:
//     ./vcpkg.exe install imgui[glfw-binding,opengl3-binding]
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif

#include "core/ray.h"
#include "core/color.h"

int g_width = 800;
int g_height = 600;
glm::vec4 Offset = glm::vec4(0.0f,0.0f,0.0f,0.0f);
glm::vec2 MouseAngle = glm::vec2(0.0f,0.0f);
glm::vec3 LightDirection = glm::vec3(3.0f,2.0f,0.0f);


float cameraSpeed = 1.0f;
float cameraMultiplier = 1.0f;
bool canPressAgain;
bool FirstScene = true;

bool PostProcessing = false;

std::vector<core::Model*>* CurrentScene;
std::string ActiveScene;

std::vector<core::Model*> Scene1;
std::vector<core::Model*> Scene2;

std::vector<glm::vec4> pixels(g_width * g_height);

struct Shader {
    std::string name;
    bool active;
};

Shader shaderList[5]{
    Shader( "Edges", false),
    Shader("Color", false),
    Shader("Pixelate", false),
    Shader("Inverse Colors", false),
    Shader("Grayscale", false),
};
float pixelSize = 0.02f;
glm::vec3 colorShaderColor = glm::vec3(1,1,1);
float edgeIntensity = 0.3f;
glm::vec3 edgeColor = glm::vec3(1,1,1);

bool NeedResize = false;

// DONE: Pass offset as reference!
void processInput(GLFWwindow *window) {
    //, glm::vec3& offset) {

#pragma region Inputs
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        glfwSetWindowAspectRatio(window,200, 150);
#pragma endregion
#pragma region Camera Speed
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraSpeed = 2.5f * cameraMultiplier;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == !GLFW_PRESS)
        cameraSpeed = 1.0f * cameraMultiplier;
#pragma endregion
#pragma region Camera Movement
    // this is reversed!
    // No it is not anymore!
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Offset.z = -cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Offset.z = cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Offset.x = -cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Offset.x = cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        Offset.y = -cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        Offset.y = cameraSpeed;
#pragma endregion
}
core::Model CreateObject(std::string name) {
    return core::AssimpLoader::loadModel(name);
}
void OnResize() {
    pixels.resize(g_width * g_height);
    NeedResize = true;
}
void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    g_width = width;
    g_height = height;
    glViewport(0, 0, width, height);
    OnResize();
}

 void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    MouseAngle.x = xpos; //std::fmod(xpos, 360);

    MouseAngle.y = ypos; //td::fmod(ypos, 360);
    // printf("MousePosition is: %f,%f\n", MouseAngle.x, MouseAngle.y);
}


std::string readFileToString(const std::string &filePath) {
    std::ifstream fileStream(filePath, std::ios::in);
    if (!fileStream.is_open()) {
        printf("Could not open file: %s\n", filePath.c_str());
        return "";
    }
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    return buffer.str();
}

GLuint generateShader(const std::string &shaderPath, GLuint shaderType) {
    printf("Loading shader: %s\n", shaderPath.c_str());
    const std::string shaderText = readFileToString(shaderPath);
    const GLuint shader = glCreateShader(shaderType);
    const char *s_str = shaderText.c_str();
    glShaderSource(shader, 1, &s_str, nullptr);
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Error! Shader issue [%s]: %s\n", shaderPath.c_str(), infoLog);
    }
    return shader;
}

double hasHitSphere(const point3& center, double radius, const ray& r) {
    vec3 oc = center - r.origin();
    auto a = dot(r.direction(), r.direction());
    auto b = -2.0 * dot(r.direction(), oc);
    auto c = dot(oc, oc) - radius*radius;
    auto discriminant = b*b - 4*a*c;
    if (discriminant < 0) {
        return -1;
    }
    return (-b - std::sqrt(discriminant) ) / (2.0*a);
}

glm::vec4 sendRay(ray r, vec3 point, double size) {
    auto t = hasHitSphere(point, size, r);
    if (t > 0.0) {
        vec3 N = glm::normalize(r.at(t) - vec3(0,0,-1));
        return glm::vec4(N * 0.5f + 0.5f,1);
        return glm::vec4(0.5f*color(N.x + 1, N.y + 1, N.z + 1),1);
    }
    vec3 unit_direction = glm::normalize(r.direction());
    float a = 0.5*(unit_direction.y + 1.0);

    vec3 returnColor = vec3((1.0f-a)*vec3(1)) + a*color(0.5,0.7,1.0);

    return glm::vec4(returnColor,1);
}

int main() {
    /*
    {
        int a=2;
        int& b=a;
        b=3;
        printf("a: %d b:%d\n",a,b);
    }
    */


    for (int j = 0; j < g_height; j++) {
        for (int i = 0; i < g_width; i++) {
            auto r = double(i) / (g_width-1);
            auto g = double(j) / (g_height-1);
            auto b = 0.0;

            int ir = int(255.999 * r);
            int ig = int(255.999 * g);
            int ib = int(255.999 * b);

            pixels[j * g_width + i] = glm::vec4(ir, ig, ib, 1.0f);
            // pixels[j * g_width + i] = glm::vec4(1,0,0,1);
        }
    }

    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, true);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // if (glfwRawMouseInputSupported()) {
    //     glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    // }

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(g_width, g_height, "RenderingEngine", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }


    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    glfwSetCursorPosCallback(window, cursor_position_callback);


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    //Setup platforms
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 400");

    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const GLuint modelVertexShader = generateShader("shaders/modelVertex.vs", GL_VERTEX_SHADER);
    const GLuint fragmentShader = generateShader("shaders/fragment.fs", GL_FRAGMENT_SHADER);
    const GLuint textureShader = generateShader("shaders/texture.fs", GL_FRAGMENT_SHADER);
    const GLuint lightShader = generateShader("shaders/light.fragshader", GL_FRAGMENT_SHADER);

    const GLuint postPFrag = generateShader("shaders/fullscreen.fs", GL_FRAGMENT_SHADER);
    const GLuint postPVert = generateShader("shaders/fullscreen.vs", GL_VERTEX_SHADER);

    int success;
    char infoLog[512];
    const unsigned int modelShaderProgram = glCreateProgram();
    glAttachShader(modelShaderProgram, modelVertexShader);
    glAttachShader(modelShaderProgram, fragmentShader);
    glLinkProgram(modelShaderProgram);
    glGetProgramiv(modelShaderProgram, GL_LINK_STATUS, &success);

    const unsigned int modelLightShaderProgram = glCreateProgram();
    glAttachShader(modelLightShaderProgram, modelVertexShader);
    glAttachShader(modelLightShaderProgram, lightShader);
    glLinkProgram(modelLightShaderProgram);
    glGetProgramiv(modelLightShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(modelShaderProgram, 512, NULL, infoLog);
        printf("Error! Making Shader Program: %s\n", infoLog);
    }
    const unsigned int textureShaderProgram = glCreateProgram();
    glAttachShader(textureShaderProgram, modelVertexShader);
    glAttachShader(textureShaderProgram, textureShader);
    glLinkProgram(textureShaderProgram);
    glGetProgramiv(textureShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
    glGetProgramInfoLog(textureShaderProgram, 512, NULL, infoLog);
    printf("Error! Making Shader Program: %s\n", infoLog);
    }

    const unsigned int postProcessingShaderProgram = glCreateProgram();

    glAttachShader(postProcessingShaderProgram, postPVert);
    glAttachShader(postProcessingShaderProgram, postPFrag);

    glLinkProgram(postProcessingShaderProgram);

    glDeleteShader(postPVert);
    glDeleteShader(postPFrag);
    // glDeleteShader(vertexShader);
    glDeleteShader(modelVertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(textureShader);
    //FirstScene
#pragma region FirstScene

    // core::Mesh otherQuad = core::Mesh::generateQuad();
    // core::Model quad2Model({otherQuad});
    // quad2Model.translate(glm::vec3(0, 0, 0));
    // quad2Model.scale(glm::vec3(5, 5, 1));
    // quad2Model.type = core::Model::ModelType::Object2d;
    // quad2Model.ModelName = "CageQuad";
    // Scene1.emplace_back(&quad2Model);
    //
    // core::Model suzanne = CreateObject("models/nonormalmonkey.obj");
    // suzanne.translate(glm::vec3(-2,0,3));
    // suzanne.type = core::Model::ModelType::Object3d;
    // suzanne.ModelName = "monkey1";
    // Scene1.emplace_back(&suzanne);

    core::Model orb = CreateObject("models/sphere.fbx");
    orb.translate(glm::vec3(0,0,-7));
    orb.scale(glm::vec3(5,5,5));
    orb.type = core::Model::ModelType::Object3d;
    orb.ModelName = "lightOrb";
    Scene1.emplace_back(&orb);

    // core::Model fish = CreateObject("models/fish.obj");
    // fish.translate(glm::vec3(-2,0,3));
    // fish.scale(glm::vec3(1.0f,5.0f,1.0f));
    // fish.type = core::Model::ModelType::Object3d;
    // fish.ModelName = "fish";
    // Scene1.emplace_back(&fish);



#pragma endregion FirstScene

#pragma region SecondScene
    // core::Model money2 = CreateObject("models/nonormalmonkey.obj");
    // money2.translate(glm::vec3(0,1,3));
    // money2.rotate(glm::vec3(0.53f,0.935f,0.235f),240);
    // money2.type = core::Model::ModelType::Object3d;
    // money2.ModelName = "money2";
    // Scene2.emplace_back(&money2);
    //
    // core::Model money3 = CreateObject("models/nonormalmonkey.obj");
    // money3.translate(glm::vec3(0,5,3));
    // money3.type = core::Model::ModelType::Object3d;
    // money3.ModelName = "money3";
    // Scene2.emplace_back(&money3);
    //
    // core::Model money4 = CreateObject("models/nonormalmonkey.obj");
    // money4.translate(glm::vec3(0,1,7));
    // money4.type = core::Model::ModelType::Object3d;
    // money4.ModelName = "money4";
    // Scene2.emplace_back(&money4);
    // core::Model money5 = CreateObject("models/nonormalmonkey.obj");
    // money5.translate(glm::vec3(2,1,3));
    // money5.type = core::Model::ModelType::Object3d;
    // money5.ModelName = "money5";
    // Scene2.emplace_back(&money5);
    //
    // //Add more monkey
    //
    // core::Model fih = CreateObject("models/fish.obj");
    // fih.translate(glm::vec3(0,0.0,0));
    // fih.rotate(glm::vec3(0,0,1), 90);
    // fih.scale(glm::vec3(1.0,5.0,1.0));
    // fih.type = core::Model::ModelType::Object3d;
    // fih.ModelName = "Fih";
    // Scene2.emplace_back(&fih);
    //
    // Scene2.emplace_back(&light);

// #pragma endregion SecondScene

    // core::Model lightOrb = CreateObject("models/sphere.fbx");
    // lightOrb.translate(LightDirection);
    // lightOrb.scale(glm::vec3(0.1,0.1,0.1));
    // lightOrb.ModelName = "LightOrb";

    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));



    core::Texture cmgtGatoTexture("textures/CMGaTo_crop.png");
    core::Texture getCaged("textures/caged.png");
    core::Texture lightCircle("textures/LightBulb.png");

    glm::vec4 clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    glClearColor(clearColor.r,
                 clearColor.g, clearColor.b, clearColor.a);


    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 10.0f);
    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
    glm::vec3 camPos = glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = normalize(cross(cameraDirection, up));
    glm::vec3 cameraUp    = normalize(cross(cameraRight, cameraDirection));

    //VP




    GLint mvpMatrixUniform = glGetUniformLocation(modelShaderProgram, "mvpMatrix");
    GLint matrixUniformLight = glGetUniformLocation(modelLightShaderProgram, "mvpMatrix");
    GLint textureModelUniform = glGetUniformLocation(textureShaderProgram, "mvpMatrix");
    GLint textureUniform = glGetUniformLocation(textureShaderProgram, "text");



    double currentTime = glfwGetTime();
    double finishFrameTime = 0.0;
    float deltaTime = 0.0f;
    float rotationStrength = 100.0f;

    float camHeading=0;
    float camPitch=0;
    float prevMouseX=0;
    float prevMouseY=0;

    bool check = false;
    int LightType = 0;
    glm::vec3 LightColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
    glm::vec3 AmbientColor = glm::vec3(0.0, 0.0,0.15);

    /* TODO!!!: Make the creating items modular!
     * Use std::string path = "/path/to/directory"; to get the path, then a foreach loop to load the options
     * Use buttons(?) to summon the items, and make them move around!
     * This is only if you got more time, first just, ya know, do the minimum requirements!
    */

    CurrentScene = &Scene1;
    ActiveScene = "Scene1";

    //Todo: Figure out how to properly bind the PostP vs and fs so that you can start using colors!
    // new FBOobject = FBOobject.Create(g_width, g_height);
    //

    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    unsigned int texBuffer;
    glGenTextures(1, &texBuffer);
    glBindTexture(GL_TEXTURE_2D, texBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, g_width, g_height, 0, GL_RGBA, GL_FLOAT, pixels.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texBuffer, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, g_width, g_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    printf("FBO NOT COMPLETE\n");

glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    while (!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &g_width, &g_height);
        glfwSetWindowAspectRatio(window,g_width, g_height);

        if (NeedResize) {
            glBindTexture(GL_TEXTURE_2D, texBuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, g_width, g_height, 0, GL_RGBA, GL_FLOAT, pixels.data());
            glBindTexture(GL_TEXTURE_2D, 0);
            NeedResize = false;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, fbo); // all subsequent render is done on the framebuffer, instead of on screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);





        float mouseDx = prevMouseX - MouseAngle.x;
        prevMouseX = MouseAngle.x;

        float mouseDy = prevMouseY - MouseAngle.y;
        prevMouseY = MouseAngle.y;


        glm::vec2 Vec = glm::vec2(mouseDx, mouseDy);
        //printf("Mouse Delta: (%f,%f)\n", mouseDx, mouseDy);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Peak Engine 1.1");
        ImGui::Checkbox("Start Editing", &check);

        if  (check) {
            if (ImGui::Button("Swap Scenes!"))
            {
                printf("Flipping Scene!\n");
                if (ActiveScene == "Scene1") {
                    ActiveScene = "Scene2";
                    CurrentScene = &Scene2;
                }
                else if (ActiveScene == "Scene2") {
                    ActiveScene = "Scene1";
                    CurrentScene = &Scene1;
                }
            }
            ImGui::SliderInt("LightType", &LightType, 0,1);
            // ImGui::SliderFloat("Monkey Spin Speed", &rotationStrength, 0.0f, 360.0f);
            ImGui::SliderFloat3("LightDirection", &LightDirection.x, -10,10);
            ImGui::SliderFloat3("AmbientColor", &AmbientColor.x, 0,0.2);
            ImGui::SliderFloat3("LightColor", &LightColor.x, 0,1);

            ImGui::Checkbox("Enable Post Processing", &PostProcessing);
            if (PostProcessing) {
                for (int i = 0; i < 5; i++) {
                    ImGui::Checkbox(shaderList[i].name.c_str(), &shaderList[i].active);

                    if (shaderList[i].name == "Pixelate" and shaderList[i].active == true) {
                        ImGui::SliderFloat("Pixel Size", &pixelSize, 0,0.2);
                    }
                    if (shaderList[i].name == "Color" and shaderList[i].active == true) {
                        ImGui::SliderFloat3("Color", &colorShaderColor.x, 0,1);
                    }
                    if (shaderList[i].name == "Edges" and shaderList[i].active == true) {
                        ImGui::SliderFloat3("Edge Color", &edgeColor.x, 0,1);
                        ImGui::SliderFloat("Edge Calmness", &edgeIntensity, 0, 0.5);
                    }
                }
            }

            // ImGui::Button("Wireframe Mode");

            // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //GL_Fill for normal, GL_Line for polygonnn
        }

        // ImGui::Text("Flying Speed: %f\n", cameraSpeed);
        ImGui::Text("It is i, the engine, courtesy of the programmer i suppose :)");
        ImGui::End();

        processInput(window);



        //camHeading += 0.001f;
        if (glfwGetMouseButton  (window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
            camHeading += mouseDx * 0.005f;
            camPitch += mouseDy * 0.005f;
        }

        // here we calculate the camera-to-world matrix:
        glm::mat4 M(1); // identity matrix
        M = glm::translate(M, cameraPos);
        M = glm::rotate(M, camHeading, glm::vec3(0,1,0));
        cameraDirection += camHeading * glm::vec3(0,1,0);
        M = glm::rotate(M, camPitch, glm::vec3(1.0,0.0,0.0));
        cameraDirection += camPitch * glm::vec3(1.0,0.0,0.0);



        // glm::vec4 col1 = glm::column(M,2);
        // printf("Matrix column: (%f,%f,%f,%f)\n",col1.x,col1.y,col1.z,col1.w);


        glm::vec3 LocalOffset = (M * (Offset * 0.015f));
        cameraPos += LocalOffset;
        Offset = glm::vec4(0.0);

        // .. the view matrix is the inverse of that:
        glm::mat4 view=glm::inverse(M);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(g_width) / static_cast<float>(g_height), 0.1f, 100.0f);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, g_width, g_height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");

        //Light
        // lightOrb.translate(LightDirection);

        // glUseProgram(modelLightShaderProgram);
        // glBindVertexArray(0);
        // glActiveTexture(GL_TEXTURE0);
        // glUniformMatrix4fv(matrixUniformLight, 1, GL_FALSE, glm::value_ptr(projection * view * lightOrb.getModelMatrix()));
        // lightOrb.render();
        // glBindVertexArray(0);

        for (core::Model* mod : *CurrentScene) {

            if (mod->type == core::Model::ModelType::Object3d) {

                glUseProgram(modelLightShaderProgram);
                glBindVertexArray(0);
                glActiveTexture(GL_TEXTURE0);
                glUniformMatrix4fv(matrixUniformLight, 1, GL_FALSE, glm::value_ptr(projection * view * mod->getModelMatrix()));

                glUniform3f(glGetUniformLocation(modelLightShaderProgram,"lightDirection"),LightDirection.x, LightDirection.y, LightDirection.z);
                glUniform1i(glGetUniformLocation(modelLightShaderProgram,"lightType"),LightType);
                glUniform3f(glGetUniformLocation(modelLightShaderProgram, "lightColor"), LightColor.x,LightColor.y,LightColor.z);
                glUniform3f(glGetUniformLocation(modelLightShaderProgram,"ambientColor"), AmbientColor.x, AmbientColor.y, AmbientColor.z);
                glUniform3f(glGetUniformLocation(modelLightShaderProgram, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);


                mod->render();

                glBindVertexArray(0);

            }

            else if (mod->type == core::Model::ModelType::Object2d) {

                glUseProgram(textureShaderProgram);
                glUniformMatrix4fv(textureModelUniform, 1, GL_FALSE, glm::value_ptr(projection * view * mod->getModelMatrix()));
                glActiveTexture(GL_TEXTURE0); // upcoming OpenGL texture configuration calls refer to the first (of 8?) textures
                glUniform1i(textureUniform, 0); // For the "text" uniform in the currently bound shader (which includes texture.fs), use the texture that GL_TEXTURE0 points to
                glBindTexture(GL_TEXTURE_2D, getCaged.getId()); // Make the current texture (GL_TEXTURE0) point to the loaded texture
                mod->render();
                // typically do this to prevent confusion:
                glBindTexture(GL_TEXTURE_2D, 0); // unbind
                glBindVertexArray(0);

            }
            else {
                return 1;
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        // vec3 pos = cameraPos - orb.getPos();
        // std::cout << pos.z << std::endl;


            float ratio = float(g_width) / float(g_height);

            for (int j = 0; j < g_height; j++) {
                for (int i = 0; i < g_width; i++) {
                    float u = (2.0f * i / g_width  - 1.0f) * ratio;
                    float v = (1.0f - 2.0f * j / g_height);

                    vec3 rayDir = glm::normalize(cameraDirection + u * cameraRight +v * cameraUp);
                    ray r(camPos, rayDir);
                    pixels[j * g_width +i] = sendRay(r, orb.getPos(), orb.getRadius());


                    // pixels[j * g_width + i] = glm::vec4(ir, ig, ib, 1.0f);
                    // pixels[j * g_width + i] = glm::vec4(1,0,0,1);

                    // pixels[j * g_width + i] = write_color(i,j,g_width,g_height);
                }
            }


        glUseProgram(postProcessingShaderProgram);
        //glUniform1i(PixelSize, PixelCount);
        //glUniform2f(Screensize, (float)g_width, (float)g_height);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texBuffer);
        glTexSubImage2D(GL_TEXTURE_2D, 0,0,0,g_width,g_height,GL_RGBA, GL_FLOAT, pixels.data());

        GLint boolList[5];
        for (int i = 0; i < 5; i++) {
            boolList[i] = shaderList[i].active;
        }
        GLint location = glGetUniformLocation(postProcessingShaderProgram, "shaderBoolList");
        glUniform1iv(location, 5, boolList);

        glUniform1f(glGetUniformLocation(postProcessingShaderProgram ,"pixelSize"), pixelSize);
        glUniform1f(glGetUniformLocation(postProcessingShaderProgram , "edgeIntensity"), edgeIntensity);

        glUniform3f(glGetUniformLocation(postProcessingShaderProgram, "edgeColor"), edgeColor.x, edgeColor.y, edgeColor.z);
        glUniform3f(glGetUniformLocation(postProcessingShaderProgram, "colorShader"),colorShaderColor.x, colorShaderColor.y, colorShaderColor.z);
        // glUniform1i(glGetUniformLocation(postProcessingShaderProgram, "horizontal"), check);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        //printf("Before poll\n");
        glfwPollEvents(); // this is where callbacks are called
        //printf("After poll\n");
        finishFrameTime = glfwGetTime();
        deltaTime = static_cast<float>(finishFrameTime - currentTime);
        currentTime = finishFrameTime;
    }

    glDeleteProgram(modelShaderProgram);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}