#include <ctime>
#include <string>
#include <iostream>
#include <Windows.h>

// GLEW
// #define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
// AntTweakBar
#include <AntTweakBar.h>

#include "UI.h"
#include "lighting.h"
#include "renderer.h"
#include "diffuseObject.h"
#include "generalObject.h"
#include "resource_manager.h"

// #define FULL_SCREEN

// Window size.
int WIDTH, HEIGHT;
GLuint width = 1920, height = 1080;

// Keyboard.
bool keys[1024];

// Mouse.
GLfloat lastX = WIDTH / 2.0f, lastY = HEIGHT / 2.0f;
bool firstMouse = true;

const int LightNumber = 5;
const int ObjectNumber = 2;
const int GeneralNumber = 2;
const int BandNumber = 4;

std::string objects[] = {"buddha", "maxplanck"};
std::string gobjects[] = {"buddha", "maxplanck"};
std::string lightings[] = {"galileo", "grace", "rnl", "stpeters", "uffizi"};
std::string bands[] = {"linear", "quadratic", "cubic", "quartic"};

glm::vec3 albedo(0.15f, 0.15f, 0.15f);

int objectIndex = -1;
int lightingIndex = -1;
int transferFIndex = -1;
int materialIndex = 0;
int bandIndex = 3;

int lastObject = -1;
int lastLighting = -1;
int lastTransfer = -1;
int lastMaterial = 0; // Diffuse
int lastBand = 3; // Quartic

DiffuseObject** diffObject;
GeneralObject* genObject;
Lighting** lighting;
Lighting* simpleL;
Renderer renderer;

// Cubemap & Simple Light.
bool drawCubemap = false;
bool simpleLight = true;
bool lastSimple = true;

// Camera.
float camera_dis = 3.0f;
glm::vec3 camera_pos(0.0f, 0.0f, 1.0f);
glm::vec3 last_camera_pos(0.0f, 0.0f, 1.0f);
glm::vec3 camera_dir(0.0f, 0.0f, 0.0f);
glm::vec3 camera_up(0.0f, 1.0f, 0.0f);

//Simple lighting.
glm::vec3 light_dir(0.0f, 0.0f, 1.0f);
float simpleLightTheta = 43.3f;
float simpleLightPhi = 225.0f;

// Rotation.
float g_Rotation[] = {0.0f, 0.0f, 0.0f, 1.0f};
int g_AutoRotate = 0;
int g_RotateTime = 0;
float g_RotateStart[] = {0.0f, 0.0f, 0.0f, 1.0f};
float rotateMatrix[4 * 4]; // Rotation matrix

// FPS.
double currTime;
double lastTime;
int frames = 0;
int fps;

// GLFW
GLFWwindow* window;

/*
 *  Function prototypes.
 */
// Callback functions.
int key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
int button_calback(GLFWwindow* window, int button, int action, int mods);
int mouse_callback(GLFWwindow* window, double xpos, double ypos);
int scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
// Data processing function.
void dataProcessing(int argc, char** argv);
// Initialization functions.
void dataLoading();
void shaderLoading();
// Miscellaneous.
void calculateFPS();
void checkUIStatus();
void changeLight(int index);
void changeObject(int index);
void changeTransfer(int index);
void changeBand(int index);
void changeMaterial(int index);

int main(int argc, char** argv)
{
    if (argc >= 2)
    {
        dataProcessing(argc, argv);
    }
    else
    {
        // Init GLFW.
        glfwInit();
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);
#endif
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef FULL_SCREEN
        // Create a "Windowed full screen" window in the primary monitor.
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        window = glfwCreateWindow(mode->width, mode->height, "prt-SH", monitor, nullptr);
#else
        window = glfwCreateWindow(width, height, "prt-SH", nullptr, nullptr);
#endif
        glfwMakeContextCurrent(window);

        // Note: The required callback functions are set with AntTweakBar UI.

        // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions.
        glewExperimental = GL_TRUE;
        // Initialize GLEW to setup the OpenGL Function pointers.
        glewInit();

        // Define the viewport dimensions.
        glfwGetFramebufferSize(window, &WIDTH, &HEIGHT);
        glViewport(0, 0, WIDTH, HEIGHT);

        width = WIDTH;
        height = HEIGHT;

        std::cout << "GLFW version                : " << glfwGetVersionString() << std::endl;
        std::cout << "GL_VERSION                  : " << glGetString(GL_VERSION) << std::endl;
        std::cout << "GL_VENDOR                   : " << glGetString(GL_VENDOR) << std::endl;
        std::cout << "GL_RENDERER                 : " << glGetString(GL_RENDERER) << std::endl;
        std::cout << "GL_SHADING_LANGUAGE_VERSION : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
        std::cout << "WINDOW WIDTH                : " << WIDTH << std::endl;
        std::cout << "WINDOW HEIGHT               : " << HEIGHT << std::endl;

        // Setup some OpenGL options.
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glDepthFunc(GL_LESS);
        // Anti-aliasing.
        glEnable(GL_MULTISAMPLE);
        glfwSwapInterval(1);

        // Do some initialization (including loading data, shaders, models, etc.)
        dataLoading();
        shaderLoading();
        renderer.Init(LightNumber);
        UIInit();
        lastTime = glfwGetTime();

        // Loop.
        while (!glfwWindowShouldClose(window))
        {
            // Calculate FPS.
            calculateFPS();

            // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions.
            glfwPollEvents();
            // Check if any thing changed by AntTweakBar UI (like object, lighting, etc.)
            checkUIStatus();

            // Clear the color buffer.
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Render.
            renderer.Render();
            // Render AntTweakBar UI.
            TwDraw();

            // Swap the screen buffers.
            glfwSwapBuffers(window);
        }
        // Terminate GLFW, clearing any resources allocated by GLFW.
        glfwTerminate();
    }

    return 0;
}

void calculateFPS()
{
    // Return current time in seconds.
    currTime = glfwGetTime();
    frames++;

    if ((currTime - lastTime) >= 1.0f)
    {
        fps = frames / (currTime - lastTime);
        // std::cout << "FPS: " << fps << std::endl;
        frames = 0;
        lastTime = currTime;
    }
}

void dataLoading()
{
    std::cout << "Loading data ................ " << std::endl;

    diffObject = new DiffuseObject*[ObjectNumber];
    genObject = new GeneralObject[GeneralNumber];
    lighting = new Lighting*[LightNumber];

    for (size_t i = 0; i < ObjectNumber; i++)
    {
        diffObject[i] = new DiffuseObject[BandNumber];
        std::string objFile = "objects/" + objects[i] + ".obj";
        for (size_t j = 0; j < BandNumber; j++)
        {
            std::string dataFile = "processedData/objects/" + bands[j] + "/" + objects[i];
            diffObject[i][j].init(objFile, albedo);
            diffObject[i][j].readFDiskbin(dataFile);
        }
    }

    for (size_t i = 0; i < GeneralNumber; i++)
    {
        std::string objFile = "objects/" + gobjects[i] + ".obj";
        std::string dataFile = "processedData/objects/quartic/" + gobjects[i];
        genObject[i].init(objFile, albedo);
        genObject[i].readFDiskbin(dataFile);
    }

    glm::vec3 hdrEffect[] = {
        glm::vec3(1.2f, 1.2f, 1.2f),
        glm::vec3(2.2f, 2.2f, 2.2f),
        glm::vec3(1.2f, 1.2f, 1.2f),
        glm::vec3(1.8f, 1.8f, 1.8f),
        glm::vec3(1.8f, 1.8f, 1.8f)
    };
    glm::vec3 glossyEffect[] = {
        glm::vec3(1.2f, 1.2f, 1.2f),
        glm::vec3(2.2f, 2.2f, 2.2f),
        glm::vec3(1.2f, 1.2f, 1.2f),
        glm::vec3(1.8f, 1.8f, 1.8f),
        glm::vec3(1.8f, 1.8f, 1.8f)
    };
    for (size_t i = 0; i < LightNumber; i++)
    {
        lighting[i] = new Lighting[BandNumber];
        for (size_t j = 0; j < BandNumber; j++)
        {
            std::string lightPattern = "processedData/lightings/" + bands[j] + "/" + lightings[i] +
                "_probe.dat";
            lighting[i][j].init(lightPattern, hdrEffect[i], glossyEffect[i]);
        }
    }

    simpleL = new Lighting[BandNumber];
    for (size_t i = 0; i < BandNumber; i++)
    {
        simpleL[i].init("processedData/lightings/" + bands[i] + "/simple_probe.dat", glm::vec3(1.0f, 1.0f, 1.0f),
                        glm::vec3(1.0f, 1.0f, 1.0f));
    }

    std::cout << "Done" << std::endl;

    // Initialize indices.
    transferFIndex = 0;
    objectIndex = 0;
    lightingIndex = 0;
    bandIndex = 3;

    if (materialIndex == 0)
    {
        // Diffuse Rendering.
        renderer.Setup(&diffObject[objectIndex][bandIndex], &lighting[lightingIndex][bandIndex]);
        renderer.SetupColorBuffer(0, glm::vec3(0.0f, 0.0f, 1.0f), true);
    }
    else
    {
        // General Rendering.
        renderer.Setup(&genObject[objectIndex], &lighting[lightingIndex][bandIndex]);
        renderer.SetupColorBuffer(0, camera_dis * camera_pos, false);
    }

    // Data initialization.
    lastObject = objectIndex;
    lastLighting = lightingIndex;
    lastTransfer = transferFIndex;
}

void shaderLoading()
{
    std::cout << "Loading shaders ............. ";
    ResourceManager::LoadShader(_SHADER_PREFIX_"/cubemap.vert", _SHADER_PREFIX_"/cubemap.frag", "", "cubemap");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/prt.vert", _SHADER_PREFIX_"/prt.frag", "", "prt");
    std::cout << "Done" << std::endl;
}

void dataProcessing(int argc, char** argv)
{
    std::string ptype(argv[1]);
    std::string diffGeneal(argv[2]);

    int sampleNumber = 4096;
    int band = 5;

    if (ptype == "-l")
    {
        if (argc > 4)
        {
            band = atoi(argv[4]);
        }
        if (argc > 5)
        {
            sampleNumber = atoi(argv[5]);
        }

        if (diffGeneal == "-s")
        {
            // Processing simple light.
            // .\PRT.exe -l -s simple_probe.dat [band] [sample number]
            Lighting simplePattern("", PROBE, band);
            simplePattern.process(sampleNumber, false);
            simplePattern.write2Diskbin(argv[3]);
        }
        else
        {
            // .\PRT.exe -l xxx_probe.hdr xxx_probe.dat [band] [sample number]
            Lighting pattern(argv[2], PROBE, band);
            pattern.process(sampleNumber, true);
            pattern.write2Diskbin(argv[3]);
        }
    }
    else if (ptype == "-o")
    {
        // .\PRT.exe -o -d 1 xxx.obj xxx.dat [band] [sample number]
        // .\PRT.exe -o -g 1 xxx.obj xxx.dat [band] [sample number]
        int transferType = atoi(argv[3]);
        if (argc > 6)
        {
            band = atoi(argv[6]);
        }
        if (argc > 7)
        {
            sampleNumber = atoi(argv[7]);
        }

        const DWORD start = GetTickCount();
        if (diffGeneal == "-d")
        {
            DiffuseObject diffObj;
            diffObj.init(argv[4], albedo);
            diffObj.project2SH(transferType, band, sampleNumber, 1);
            diffObj.write2Diskbin(argv[5]);
        }
        else if (diffGeneal == "-g")
        {
            GeneralObject genObj;
            genObj.init(argv[4], albedo);
            genObj.project2SH(transferType, band, sampleNumber, 1);
            genObj.write2Diskbin(argv[5]);
        }
        const DWORD end = GetTickCount();

        std::cout << "Processing time:" << (end - start) / 1000.0 << " seconds" << std::endl;
    }
}

// Is called whenever a key is pressed/released via GLFW.
int key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }

    return TwEventKeyGLFW(key, action);
}

int mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    // currentcamera->ProcessMouseMovement(xoffset, yoffset, xpos, ypos);
    if (simpleLight)
    {
        simpleLightTheta += 0.1f * (yoffset);
        if (simpleLightTheta < 0.0f)
            simpleLightTheta = 0.0f;
        if (simpleLightTheta > 180.0f)
            simpleLightTheta = 180.0f;

        simpleLightPhi += 0.1f * (xoffset);
    }

    return TwMouseMotion((int)xpos, (int)ypos);
}

// Is called whenever MOUSE_LEFT or MOUSE_RIGHT is pressed/released via GLFW.
int button_calback(GLFWwindow* window, int button, int action, int mods)
{
    return TwEventMouseButtonGLFW(button, action);
}


int scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset > 0 && camera_dis >= 1.0f)
    {
        camera_dis -= 0.2f;
    }
    else
    {
        camera_dis += 0.2f;
    }
    return TwEventMouseWheelGLFW(yoffset);
}

void checkUIStatus()
{
    if (lastLighting != lightingIndex)
    {
        std::cout << "Console UI: Light change" << std::endl;

        changeLight(lightingIndex);
        lastLighting = lightingIndex;
    }
    if (lastObject != objectIndex)
    {
        std::cout << "Console UI: Object change" << std::endl;
        changeObject(objectIndex);
        lastObject = objectIndex;
    }
    if (lastTransfer != transferFIndex)
    {
        std::cout << "Console UI: Transfer Type change" << std::endl;
        changeTransfer(transferFIndex);
        lastTransfer = transferFIndex;
    }
    if (lastSimple != simpleLight)
    {
        std::cout << "Console UI: Simple Light" << std::endl;
        if (materialIndex == 0)
        {
            renderer.SetupColorBuffer(transferFIndex, glm::vec3(0.0f, 0.0f, 0.0f), true);
        }
        else
        {
            renderer.SetupColorBuffer(transferFIndex, glm::vec3(0.0f, 0.0f, 0.0f), false);
        }
        lastSimple = simpleLight;
        drawCubemap = !simpleLight;
    }
    if (lastMaterial != materialIndex)
    {
        std::cout << "Console UI: Material change" << std::endl;
        changeMaterial(materialIndex);
        lastMaterial = materialIndex;
    }
    if (lastBand != bandIndex)
    {
        std::cout << "Console UI: SH Order change" << std::endl;
        changeBand(bandIndex);
        lastBand = bandIndex;
    }
}

void changeLight(int index)
{
    if (materialIndex == 0)
    {
        renderer.Setup(&diffObject[objectIndex][bandIndex], &lighting[index][bandIndex]);
        renderer.SetupColorBuffer(transferFIndex, glm::vec3(0.0f, 0.0f, 0.0f), true);
    }
    else
    {
        renderer.Setup(&genObject[objectIndex], &lighting[index][bandIndex]);
        renderer.SetupColorBuffer(transferFIndex, camera_dis * camera_pos, false);
    }
}

void changeObject(int index)
{
    if (materialIndex == 0)
    {
        renderer.Setup(&diffObject[index][bandIndex], &lighting[lightingIndex][bandIndex]);
        renderer.SetupColorBuffer(transferFIndex, glm::vec3(0.0f, 0.0f, 0.0f), true);
    }
    else
    {
        renderer.Setup(&genObject[index], &lighting[lightingIndex][bandIndex]);
        renderer.SetupColorBuffer(transferFIndex, camera_dis * camera_pos, false);
    }
}

void changeTransfer(int index)
{
    if (materialIndex == 0)
    {
        renderer.Setup(&diffObject[objectIndex][bandIndex], &lighting[lightingIndex][bandIndex]);
        renderer.SetupColorBuffer(index, glm::vec3(0.0f, 0.0f, 0.0f), true);
    }
    else
    {
        renderer.Setup(&genObject[objectIndex], &lighting[lightingIndex][bandIndex]);
        renderer.SetupColorBuffer(index, camera_dis * camera_pos, false);
    }
}

void changeBand(int index)
{
    // We only generate data for diffuse transfer.
    if (materialIndex == 0)
    {
        renderer.Setup(&diffObject[objectIndex][index], &lighting[lightingIndex][index]);
        renderer.SetupColorBuffer(transferFIndex, glm::vec3(0.0f, 0.0f, 0.0f), true);
    }
}

void changeMaterial(int index)
{
    if (index == 0)
    {
        renderer.Setup(&diffObject[objectIndex][bandIndex], &lighting[lightingIndex][bandIndex]);
        renderer.SetupColorBuffer(transferFIndex, glm::vec3(0.0f, 0.0f, 0.0f), true);
    }
    else
    {
        renderer.Setup(&genObject[objectIndex], &lighting[lightingIndex][bandIndex]);
        renderer.SetupColorBuffer(transferFIndex, camera_dis * camera_pos, false);
    }
}
