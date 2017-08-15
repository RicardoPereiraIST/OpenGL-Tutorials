#pragma once

#include "Init.h"
#include "..\Debug\Debug.h"

unsigned int Init::width, Init::height;

float Init::deltaTime = 0.0f;
float Init::lastFrame = 0.0f;

//MOUSE
float Init::lastX = width / 2, Init::lastY = height / 2;
bool Init::firstMouse = true;
bool Init::setCamera;

Init::Init(unsigned int w, unsigned int h, bool d, int s, bool c, std::string t, bool setC) {
	width = w;
	height = h;
	debugLibrary = d;
	samples = s;
	cursor = c;
	title = t;
	setCamera = setC;

	initializeGLFW();
	createWindow();
	initializeGLAD();
	if (debugLibrary)
		initializeDebug();

	glViewport(0, 0, width, height);
	setCallbacks();

	if (setCamera) {
		//Camera
		Camera *camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
		CameraManager::instance()->add("camera", camera);
	}
}

Init::~Init() {
	glfwTerminate();
	glUseProgram(0);
	glfwDestroyWindow(window);
}

void Init::initializeGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if (samples > 0)
		glfwWindowHint(GLFW_SAMPLES, samples);
	if (debugLibrary)
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // comment this line in a release build!
}

int Init::initializeGLAD() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	return 0;
}

void Init::initializeDebug() {
	GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
}

int Init::createWindow() {
	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	if (cursor)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return 0;
}

void Init::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void Init::mouse_callback(GLFWwindow* window, double xpos, double ypos) {

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	CameraManager::instance()->get("camera")->ProcessMouseMovement(xoffset, yoffset);
}

void Init::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	CameraManager::instance()->get("camera")->ProcessMouseScroll(yoffset);
}

void Init::setCallbacks() {
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
}