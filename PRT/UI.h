#ifndef UI_H_
#define UI_H_

#include <AntTweakBar.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/quaternion.hpp>

extern glm::vec3 camera_pos;
extern glm::vec3 light_dir;

extern bool simpleLight;

// Rotation.
extern glm::fquat g_Rotation;
extern glm::fquat last_Rotation;
extern glm::fquat g_RotateStart;
extern int g_AutoRotate;
extern int g_RotateTime;

// Mesh Information.
extern int fps;
extern int vertices;
extern int faces;

extern std::string objects[];
extern std::string lightings[];
extern int objectIndex;
extern int lightingIndex;
extern int transferFIndex;
extern int materialIndex;
extern int bandIndex;
extern int BRDFIndex;

// Window.
extern int WIDTH;
extern int HEIGHT;

// GLFW
extern GLFWwindow* window;

extern int key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
extern int button_calback(GLFWwindow* window, int button, int action, int mods);
extern int mouse_callback(GLFWwindow* window, double xpos, double ypos);
extern int scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

typedef enum { UNSHADOW, SHADOW, INTERREFLECT } TransferFEUM;

typedef enum { BUDDHA, MAXPLANCK } ObjectEUM;

typedef enum { LIGHT1, LIGHT2, LIGHT3, LIGHT4, LIGHT5 } LightingEUM;

typedef enum { DIFFUSE, GLOSSY } MaterialEUM;

typedef enum { LINEAR, QUADRATIC, CUBIC, QUARTIC } BandENUM;

typedef enum { PHONG, WARD_ISOTROPIC, WARD_ANISOTROPIC } BRDFENUM;

inline void Terminate()
{
    TwTerminate();
}

inline void TW_CALL SetAutoRotateCB(const void* value, void* clientData)
{
    (void)clientData;

    // Copy value to g_AutoRotate.
    g_AutoRotate = *(const int *)value;
    if (g_AutoRotate != 0)
    {
        // Initialize rotation.
        g_RotateTime = glfwGetTime();
        g_RotateStart = g_Rotation;

        TwDefine(" Console/ObjRotation readonly ");
    }
    else
    {
        TwDefine(" Console/ObjRotation readwrite ");
    }
}

inline void TW_CALL GetAutoRotateCB(void* value, void* clientData)
{
    (void)clientData; // unused
    *(int *)value = g_AutoRotate; // copy g_AutoRotate to value
}

inline void UIInit()
{
    glm::vec3 axis(0.0f, 1.0f, 0.0f);
    float angle = 0.0f;

    TwInit(TW_OPENGL_CORE, NULL);
    TwWindowSize(WIDTH, HEIGHT);

    TwBar* bar = TwNewBar("Console");
    TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.' ");
    TwDefine(" Console size='250 440' color='40 40 40' position='0 0' ");

    {
        TwEnumVal MaterialEV[2] = {{DIFFUSE, "diffuse"}, {GLOSSY, "glossy"}};
        TwType MaterialType = TwDefineEnum("Material Type", MaterialEV, 2);
        // Add 'Material Type' to 'bar': it is a modifable variable of type MaterialType.
        TwAddVarRW(bar, "Material Type", MaterialType, &materialIndex, " help='Change object.' ");
    }

    {
        TwEnumVal BandEV[4] = {{LINEAR, "linear"}, {QUADRATIC, "quadratic"}, {CUBIC, "cubic"}, {QUARTIC, "quartic"}};
        TwType BandType = TwDefineEnum("SH Order", BandEV, 4);
        TwAddVarRW(bar, "SH Order", BandType, &bandIndex, " help='Change SH order.' ");
    }

    {
        TwEnumVal ObjectEV[2] = {{BUDDHA, "buddha"}, {MAXPLANCK, "maxplanck"}};
        TwType ObjectType = TwDefineEnum("ObjectType", ObjectEV, 2);
        TwAddVarRW(bar, "Object Type", ObjectType, &objectIndex, " help='Change object.' ");
    }

    {
        TwEnumVal tranFEV[3] = {{UNSHADOW, "unshadowed"}, {SHADOW, "shadowed"}, {INTERREFLECT, "interreflected"}};
        TwType tranFType = TwDefineEnum("tranFType", tranFEV, 3);
        TwAddVarRW(bar, "Transfer Type", tranFType, &transferFIndex, " help='Change transfer function.' ");
    }

    {
        TwEnumVal LightingEV[5] = {
            {LIGHT1, "galileo"}, {LIGHT2, "grace"}, {LIGHT3, "rnl"}, {LIGHT4, "stpeters"}, {LIGHT5, "uffizi"}
        };
        TwType LightType = TwDefineEnum("Lighting", LightingEV, 5);
        TwAddVarRW(bar, "Environment Lighting", LightType, &lightingIndex, " help='Change lighting.' ");
    }

    {
        TwEnumVal brdfEV[3] = {
            {PHONG, "Phong"}, {WARD_ISOTROPIC, "Ward(isotropic)"}, {WARD_ANISOTROPIC, "Ward(anisotropic)"}
        };
        TwType brdfType = TwDefineEnum("brdf", brdfEV, 3);
        TwAddVarRW(bar, "BRDF", brdfType, &BRDFIndex, " help='Change BRDF.' ");
    }

    TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4F, &g_Rotation,
               " label='Object Orientation' opened=false help='Change the object orientation.' ");

    TwAddVarCB(bar, "AutoRotate", TW_TYPE_BOOL32, SetAutoRotateCB, GetAutoRotateCB, NULL,
               " label='Auto-Rotate' help='Toggle auto-rotate mode.' ");

    TwAddVarRW(bar, "CameraRotation", TW_TYPE_DIR3F, &camera_pos,
               " label='Camera Direction' opened=true help='Change the camera direction.'");

    TwAddVarRW(bar, "LightRotation", TW_TYPE_DIR3F, &light_dir,
               " label='Light Direction' opened=true help='Change the light direction.'");

    TwAddVarRW(bar, "Simple Light", TW_TYPE_BOOLCPP, &simpleLight,
               " label='Simple Light' help='Change the simple light'");

    TwBar* info = TwNewBar("Mesh");
    TwDefine("Mesh size='250 100' text=light  color='40 40 40' position='0 440' valueswidth=100");

    TwAddVarRO(info, "FPS", TW_TYPE_UINT32, &fps, " label='FPS: ' ");

    // std::string mesh = objects[objectIndex];
    // std::string lighting = lightings[lightingIndex];
    //
    // std::cout << "Mesh: " << mesh << std::endl;
    // std::cout << "Lighting: " << lighting << std::endl;

    // @FIXME: String will cause memory leak.
    // TwAddVarRO(info, "Mesh", TW_TYPE_STDSTRING, &mesh, " label='Mesh: ' ");
    // TwAddVarRO(info, "Lighting", TW_TYPE_STDSTRING, &lighting, " label='Lighting: ' ");
    TwAddVarRO(info, "nbV", TW_TYPE_UINT32, &vertices, " label='Vertices: ' ");
    TwAddVarRO(info, "nbF", TW_TYPE_UINT32, &faces, " label='Triangles: ' ");

    g_RotateTime = glfwGetTime();
    g_RotateStart = glm::angleAxis(glm::radians(angle), axis);
    g_Rotation = glm::angleAxis(glm::radians(angle), axis);
    last_Rotation = g_Rotation;

    // Set GLFW event callbacks.
    glfwSetCursorPosCallback(window, (GLFWcursorposfun)mouse_callback);
    glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)button_calback);
    glfwSetKeyCallback(window, (GLFWkeyfun)key_callback);
    glfwSetScrollCallback(window, (GLFWscrollfun)scroll_callback);
}

#endif
