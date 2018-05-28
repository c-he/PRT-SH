#ifndef UI_H_
#define UI_H_

#include <AntTweakBar.h>
#include <GLFW/glfw3.h>

extern glm::vec3 camera_pos;
extern glm::vec3 light_dir;

extern bool simpleLight;

// Rotation.
extern float g_Rotation[4];
extern int g_AutoRotate;
extern int g_RotateTime;
extern float g_RotateStart[4];

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

typedef enum { LIGHT1, LIGHT2, LIGHT3 } LightingEUM;

typedef enum { DIFFUSE, GLOSSY } MaterialEUM;

// Axis,angle convert to quat.
inline void AxisAngletoQuat(float* q, const float* axis, float angle)
{
    float sinxita, length;

    sinxita = (float)sin(0.5f * angle);
    length = (float)sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);

    q[0] = sinxita * axis[0] / length;
    q[1] = sinxita * axis[1] / length;
    q[2] = sinxita * axis[2] / length;
    q[3] = (float)cos(0.5f * angle);
}

// quat to rotate matrix
inline void QuattoMatrix(const float* q, float* mat)
{
    float yy2 = 2.0f * q[1] * q[1];
    float xy2 = 2.0f * q[0] * q[1];
    float xz2 = 2.0f * q[0] * q[2];
    float yz2 = 2.0f * q[1] * q[2];
    float zz2 = 2.0f * q[2] * q[2];
    float wz2 = 2.0f * q[3] * q[2];
    float wy2 = 2.0f * q[3] * q[1];
    float wx2 = 2.0f * q[3] * q[0];
    float xx2 = 2.0f * q[0] * q[0];
    mat[0] = -yy2 - zz2 + 1.0f;
    mat[1] = xy2 + wz2;
    mat[2] = xz2 - wy2;
    mat[4] = xy2 - wz2;
    mat[5] = -xx2 - zz2 + 1.0f;
    mat[6] = yz2 + wx2;
    mat[8] = xz2 + wy2;
    mat[9] = yz2 - wx2;
    mat[10] = -xx2 - yy2 + 1.0f;
    mat[3] = mat[7] = mat[11] = mat[12] = mat[13] = mat[14] = 0;
    mat[15] = 1;
}

// Multiply quaternion.
inline void Multi(const float* q1, const float* q2, float* result)
{
    result[0] = q1[3] * q2[0] + q1[0] * q2[3] + q1[1] * q2[2] - q1[2] * q2[1];
    result[1] = q1[3] * q2[1] + q1[1] * q2[3] + q1[2] * q2[0] - q1[0] * q2[2];
    result[2] = q1[3] * q2[2] + q1[2] * q2[3] + q1[0] * q2[1] - q1[1] * q2[0];
    result[3] = q1[3] * q2[3] - (q1[0] * q2[0] + q1[1] * q2[1] + q1[2] * q2[2]);
}

inline void Terminate()
{
    TwTerminate();
}

inline void TW_CALL SetAutoRotateCB(const void* value, void* clientData)
{
    (void)clientData; // unused

    g_AutoRotate = *(const int *)value; // copy value to g_AutoRotate
    if (g_AutoRotate != 0)
    {
        // init rotation
        g_RotateTime = glfwGetTime();
        g_RotateStart[0] = g_Rotation[0];
        g_RotateStart[1] = g_Rotation[1];
        g_RotateStart[2] = g_Rotation[2];
        g_RotateStart[3] = g_Rotation[3];

        TwDefine(" Console/ObjRotation readonly ");
    }
    else
        TwDefine(" Console/ObjRotation readwrite ");
}

inline void TW_CALL GetAutoRotateCB(void* value, void* clientData)
{
    (void)clientData; // unused
    *(int *)value = g_AutoRotate; // copy g_AutoRotate to value
}

inline void UIInit()
{
    TwBar *bar, *info; // Pointer to a tweak bar
    float axis[] = {0.0f, 1.0f, 0.0f};
    float angle = 0.0f;

    TwInit(TW_OPENGL_CORE, NULL);
    TwWindowSize(WIDTH, HEIGHT);

    bar = TwNewBar("Console");
    TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.' ");
    TwDefine(" Console size='250 440' color='96 216 224' position='3 10' ");

    {
        TwEnumVal MaterialEV[2] = {{DIFFUSE, "diffuse"}, {GLOSSY, "glossy"}};
        TwType MaterialType = TwDefineEnum("Material Type", MaterialEV, 2);
        // Add 'Material Type' to 'bar': it is a modifable variable of type MaterialType.
        TwAddVarRW(bar, "Material Type", MaterialType, &materialIndex,
                   " keyIncr='<' keyDecr='>' help='Change object.' ");
    }

    TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4F, &g_Rotation,
               " label='Object Rotation' opened=false help='Change the object orientation.' ");

    TwAddVarCB(bar, "AutoRotate", TW_TYPE_BOOL32, SetAutoRotateCB, GetAutoRotateCB, NULL,
               " label='Auto-Rotate' key=space help='Toggle auto-rotate mode.' ");

    TwAddVarRW(bar, "CameraRotation", TW_TYPE_DIR3F, &camera_pos,
               " label='Camera Rotation' opened=true help='Change the camera direction.'");

    TwAddVarRW(bar, "Simple Light", TW_TYPE_BOOLCPP, &simpleLight,
               " label='Simple Light' help='Change the simple light'");

    TwAddVarRW(bar, "LightRotation", TW_TYPE_DIR3F, &light_dir,
               " label='Light Rotation' opened=true help='Change the camera direction.'");

    {
        TwEnumVal ObjectEV[2] = {{BUDDHA, "buddha"}, {MAXPLANCK, "maxplanck"}};
        TwType ObjectType = TwDefineEnum("ObjectType", ObjectEV, 2);
        TwAddVarRW(bar, "Object Type", ObjectType, &objectIndex, " keyIncr='<' keyDecr='>' help='Change object.' ");
    }

    {
        TwEnumVal tranFEV[3] = {{UNSHADOW, "unshadowed"}, {SHADOW, "shadowed"}, {INTERREFLECT, "interreflected"}};
        TwType tranFType = TwDefineEnum("tranFType", tranFEV, 3);
        TwAddVarRW(bar, "Transfer Type", tranFType, &transferFIndex,
                   " keyIncr='<' keyDecr='>' help='Change transfer function.' ");
    }

    {
        TwEnumVal LightingEV[3] = {{LIGHT1, "grace"}, {LIGHT2, "stpeters"}, {LIGHT3, "campus"}};
        TwType LightType = TwDefineEnum("Lighting", LightingEV, 3);
        TwAddVarRW(bar, "Environment Lighting", LightType, &lightingIndex,
                   " keyIncr='<' keyDecr='>' help='Change object.' ");
    }
    // TwAddVarRW(bar, "Multi Sampling", TW_TYPE_BOOL32, &b_multiSampling, NULL);

    info = TwNewBar("Mesh");
    TwDefine("Mesh size='250 320' text=light  color='40 40 40' position='3 450' valueswidth=100");

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

    AxisAngletoQuat(g_Rotation, axis, angle);
    AxisAngletoQuat(g_RotateStart, axis, angle);

    // Set GLFW event callbacks.
    glfwSetCursorPosCallback(window, (GLFWcursorposfun)mouse_callback);
    glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)button_calback);
    glfwSetKeyCallback(window, (GLFWkeyfun)key_callback);
    glfwSetScrollCallback(window, (GLFWscrollfun)scroll_callback);
}

#endif
