#include <ctime>
#include <string>
#include <iostream>
#include <Windows.h>

#define _USE_MATH_DEFINES
#include <cmath>

// GLEW
// #define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
#include "GL/freeglut.h"
// GLM
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <AntTweakBar.h>

#include "UI.h"
#include "Scene.h"
#include "Object.h"
#include "Lighting.h"
#include "Renderer.h"
#include "GlutFunction.h"
#include "DiffuseObject.h"
#include "GeneralObject.h"
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

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

/*some process commends

prt -l xxxx.jpg output.txt bandnumber samplenumber(4096)
prt -o -d 1 xxxx.obj output.txt bandnumber samplenumber
prt -o -g 1 xxxx.obj output.txt bandnumber samplenumber

@Todo: binary IO

*/

using namespace std;
using glm::vec3;
using glm::vec4;
using glm::mat4;

int lightNumber = 3;
int ObjectNumber = 2;
int GeneralNumber = 2;

std::string objects[] = {"buddha", "maxplanck", "Gargoyle2"};
std::string gobjects[] = {"buddha", "maxplanck", "Gargoyle2"};
std::string lightings[] = {"grace", "stpeters", "campus"};
std::string transferF[] = {"D", "DS", "DSI"};
std::string albedos[] = {"01", "03", "05"};

glm::vec3 HDRaffect[] = {glm::vec3(2.2f, 2.2f, 2.2f), glm::vec3(1.8f, 1.8f, 1.8f), glm::vec3(0.5f, 0.55f, 0.5f)};
glm::vec3 Glossyaffect[] = {glm::vec3(1.2f, 1.2f, 1.2f), glm::vec3(1.5f, 1.5f, 1.5f), glm::vec3(0.3f, 0.32f, 0.3f)};

int objectIndex = -1;
int lightingIndex = -1;
int transferFIndex = -1;
int materialIndex = 0;
int albedosIndex = -1;

int lastMaterial = 0;
int lastObject = -1;
int lastLighting = -1;
int lastTransfer = -1;

DiffuseObject* diffObject;
GeneralObject* genObject;
Lighting* lighting;
Lighting simpleL;
Renderer renderer;

glm::vec3 albedo(0.15f, 0.15f, 0.15f);
////////////////////////
bool b_multiSampling = false;
bool b_lastmulti = false;
//////////////////cubemap
bool drawCubemap = false;
bool simpleLight = false;
bool lastSimple = false;
mat4 proMatrix;
/////////////////camera 
float camera_dis = 3;
float last_camera_pos[] = {0, 0, 1};
float camera_pos[] = {0, 0, 1};
float camera_dir[] = {0, 0, 0};
float camera_up[] = {0, 1, 0};
/////////////////simple lighting
float light_dir[] = {0, 0, 1};
float simpleLightTheta = 43.3f;
float simpleLightPhi = 225.0f;
////////////////mouse
int mouseX, mouseY;
int lastMouseX, lastMouseY;
////////////////rotation
float g_Rotation[] = {0.0f, 0.0f, 0.0f, 1.0f};
int g_AutoRotate = 0;
int g_RotateTime = 0;
float g_RotateStart[] = {0.0f, 0.0f, 0.0f, 1.0f};

float g_MatDiffuse[] = {1.0f, 1.0f, 0.0f, 1.0f};
float lastQuat[4];

float rotateMatrix[4 * 4]; // rotation matrix

bool b_rotate = false;
//////////////////info
/////fps
double currTime, lastTime;
int frames, fps;
/////mesh
int vertices;
int faces;

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
void synchronize();

int main(int argc, char** argv)
{
	// Preprocess light.
	/*argc = 6;
	argv[0] = "prt";
	argv[1] = "-l";

	argv[2] = "Lighting/cubemaptest.bmp";
	argv[3] = "ProcessedData/simple_probe.dat";

	argv[4] = "4";
	argv[5] = "4096";*/

	// Preprocess model.
	/*argc = 7;
	argv[0] = "prt";
	argv[1] = "-o";
	argv[2] = "-g";
	argv[3] = "1";
	argv[4] = "Scene/buddha.obj";
	argv[5] = "Time/buddhaG01.dat";
	argv[6] = "4";
	argv[7] = "4096";*/

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
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *mode = glfwGetVideoMode(monitor);
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
		// glfwSwapInterval(1);

		// Do some initialization (including loading data, shaders, models, etc.)
		dataLoading();
		shaderLoading();
        renderer.init();
		UIInit();
		lastTime = glfwGetTime();

		// Loop.
		while (!glfwWindowShouldClose(window))
		{
			// Calculate FPS.
			calculateFPS();

			// Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions.
			glfwPollEvents();
			// do_movement();

			// Clear the color buffer.
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Render.
            renderer.render();
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
	currTime = glfwGetTime();
	frames++;

	if ((currTime - lastTime) >= 1000.0f)
	{
		fps = static_cast<int>((frames * 1000.0f) / (currTime - lastTime));
		frames = 0;
		lastTime = currTime;
	}
}

void dataLoading()
{
	std::cout << "Loading data ................ " << std::endl;

	diffObject = new DiffuseObject[ObjectNumber];
	genObject = new GeneralObject[GeneralNumber];

	lighting = new Lighting[lightNumber];
	// cubeMap = new Cubemap[lightNumber];

	for (size_t i = 0; i < ObjectNumber; i++)
	{
		std::string objFile = "Scene/" + objects[i] + ".obj";
		std::string dataFile = "ProcessedData/" + objects[i];
		diffObject[i].init(objFile, albedo);
		diffObject[i].readFDiskbin(dataFile);
	}

	// for (size_t i = 0; i < GeneralNumber; i++)
	// {
	// 	std::string objFile = "Scene/" + gobjects[i] + ".obj";
	// 	std::string dataFile = "ProcessedData/" + gobjects[i];
	// 	genObject[i].init(objFile, albedo);
	// 	genObject[i].readFDisk(dataFile);
	// }

	for (size_t i = 0; i < lightNumber; i++)
	{
		std::string lightPattern = "ProcessedData/" + lightings[i] + "_probe.dat";
		lighting[i].init(lightPattern, HDRaffect[i], Glossyaffect[i]);
	}

	std::cout << "Done" << std::endl;

	albedosIndex = 0;
	transferFIndex = 0;
	objectIndex = 0;
	lightingIndex = 0;
	// changeScene(1,0);
	// changeLight(0);
	// changeObject(1);

	simpleL.init("ProcessedData/simple_probe.dat", glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	if (materialIndex == 0)
	{
		// Diffuse Rendering
		renderer.init(&diffObject[0], &lighting[0]);
		renderer.initColorBuffer(0, glm::vec3(0.0f, 0.0f, 1.0f), true);
	}
	else
	{
		// General Rendering
		renderer.init(&genObject[0], &lighting[0]);
		renderer.initColorBuffer(0, camera_dis * glm::vec3(camera_pos[0], camera_pos[1], camera_pos[2]), false);
	}

	synchronize();
}

void shaderLoading()
{
	std::cout << "Loading shaders ............. ";
	ResourceManager::LoadShader(_SHADER_PREFIX_"/cubemap.vert", _SHADER_PREFIX_"/cubemap.frag", "", "cubemap");
	std::cout << "Done" << std::endl;
}

void dataProcessing(int argc, char** argv)
{
	std::string ptype(argv[1]);
	std::string diffGeneal(argv[2]);

	int samplenumber = 4096;
	int band = 4;

	int type = atoi(argv[3]);

	if (argc > 6)
	{
		band = atoi(argv[6]);
	}
	if (argc > 7)
	{
		samplenumber = atoi(argv[7]);
	}
	if (ptype == "-l")
	{
		Lighting pattern(argv[2], PROBE, band);
		//Lighting ltemp;
		pattern.process(samplenumber, true);
		pattern.write2Disk(argv[3]);
	}
	else if (ptype == "-o")
	{
		const DWORD start = GetTickCount();
		if (diffGeneal == "-d")
		{
			DiffuseObject otemp;
			otemp.init(argv[4], albedo);
			otemp.project2SH(type, band, samplenumber, 2);
			otemp.write2Diskbin(argv[5]);
		}
		else
		{
			GeneralObject otemp;
			otemp.init(argv[4], albedo);
			otemp.project2SH(type, band, samplenumber, 1);
			otemp.write2Disk(argv[5]);
		}
		const DWORD end = GetTickCount();

		std::cout << "Training time:" << (end - start) / 1000.0f << std::endl;
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
		mouseX = xpos;
		mouseY = ypos;

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
	if (button == 3)
	{
		camera_dis -= 0.2f;
	}
	else if (button == 4)
	{
		camera_dis += 0.2f;
	}

	return TwEventMouseButtonGLFW(button, action);
}


int scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	return TwEventMouseWheelGLFW(yoffset);
}

void Display()
{
	b_rotate = false;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// if (drawCubemap)
	// {
	// 	glm::mat4 pvm = proMatrix *
	// 		glm::lookAt(
	// 			vec3(camera_dis * camera_pos[0],
	// 			     camera_dis * camera_pos[1],
	// 			     camera_dis * camera_pos[2]),
	// 			vec3(camera_dir[0], camera_dir[1], camera_dir[2]),
	// 			vec3(camera_up[0], camera_up[1], camera_up[2])
	// 		);
 //
	// 	cubeMap[lightingIndex].render(pvm);
	// }

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(
		camera_dis * camera_pos[0],
		camera_dis * camera_pos[1],
		camera_dis * camera_pos[2],
		camera_dir[0], camera_dir[1], camera_dir[2],
		camera_up[0], camera_up[1], camera_up[2]
	);


	//std::cout << camera_pos[0] << ' ' << camera_pos[1] << ' ' << camera_pos[2] <<std::endl;

	if (g_AutoRotate)
	{
		float axis[3] = {0, 1, 0};
		float angle = (glfwGetTime() - g_RotateTime) / 1000.0f;
		float quat[4];
		AxisAngletoQuat(quat, axis, angle);
		Multi(g_RotateStart, quat, g_Rotation);
		b_rotate = true;
	}

	QuattoMatrix(g_Rotation, rotateMatrix);
	glMultMatrixf(rotateMatrix);

	//for(int s = 0; s < 4; ++s)
	//std::cout << "quan" << g_Rotation[0] << ' '<< g_Rotation[1] << ' '<< g_Rotation[2] << ' '<< g_Rotation[3] << std::endl;
	vec3 rotateVector;
	bool b_rotateLight = false;
	float thetatemp;
	//float phitemp;
	if (b_rotate)
	{
		mat4 rM = glm::make_mat4(rotateMatrix);
		vec4 dir = rM * vec4(0.0f, 0.0f, 1.0f, 0.0f);
		rotateVector = vec3(dir.x, dir.y, dir.z);
		rotateVector = glm::clamp(rotateVector, -1.0f, 1.0f);
		thetatemp = acos(rotateVector.z);
		if (dir.x < 0)
			thetatemp = 2 * (float)M_PI - thetatemp;

		b_rotateLight = true;
	}
	if (simpleLight)
	{
		//rotateVector = vec3(light_dir[0],light_dir[1],light_dir[2]);
		rotateVector = vec3(light_dir[2], light_dir[0], light_dir[1]);

		b_rotateLight = true;
	}

	if (b_rotateLight)
	{
		rotateVector = glm::normalize(rotateVector);
		float theta, phi;

		rotateVector[2] = glm::clamp(rotateVector[2], -1.0f, 1.0f);

		theta = acos(rotateVector[2]);
		float sintheta = sin(theta);
		if (fabs(sintheta) < M_ZERO)
		{
			phi = 0.0f;
		}
		else
		{
			float cosphi = rotateVector[0] / sintheta;
			float sinphi = rotateVector[1] / sintheta;
			phi = inverseSC(sinphi, cosphi);
		}
		vector<vec2> rotatePara;
		rotatePara.clear();

		if (simpleLight)
		{
			rotatePara.push_back(vec2(theta, phi));
			simpleL.rotateZYZ(rotatePara);
		}
		if (b_rotate)
		{
			rotatePara.push_back(vec2(0.0f, 2.0f * M_PI - thetatemp));
			//rotatePara.push_back(vec2(3.0f*M_PI/2.0f,M_PI/2.0f));
			lighting[lightingIndex].rotateZYZ(rotatePara);
		}
		if (materialIndex == 0)
		{
			renderer.init(&diffObject[objectIndex], &lighting[lightingIndex]);
			renderer.initColorBuffer(transferFIndex, vec3(0.0f, 0.0f, 0.0f), true);
		}
		else
		{
			renderer.init(&genObject[objectIndex], &lighting[lightingIndex]);
			renderer.initColorBuffer(transferFIndex, camera_dis * vec3(camera_pos[0], camera_pos[1], camera_pos[2]), false);
		}
	}

	if (materialIndex == 1)
	{
		if ((last_camera_pos[0] != camera_pos[0]) || (last_camera_pos[1] != camera_pos[1]) || (last_camera_pos[2] !=
			camera_pos[2]))
		{
			renderer.init(&genObject[objectIndex], &lighting[lightingIndex]);
			renderer.initColorBuffer(transferFIndex, camera_dis * vec3(camera_pos[0], camera_pos[1], camera_pos[2]), false);
			last_camera_pos[0] = camera_pos[0];
			last_camera_pos[1] = camera_pos[1];
			last_camera_pos[2] = camera_pos[2];
		}
	}

	renderer.naiveObjRender();

	calculateFPS();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	TwDraw();
	glutSwapBuffers();
	glutPostRedisplay();
}

void synchronize()
{
	lastLighting = lightingIndex;
	lastObject = objectIndex;
	lastTransfer = transferFIndex;

	b_lastmulti = b_multiSampling;

	lastMouseX = mouseX;
	lastMouseY = mouseY;
}
