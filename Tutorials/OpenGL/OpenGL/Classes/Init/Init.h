#pragma once

#ifndef INIT_H
#define INIT_H

#include <glad\glad.h>
#include <glfw3.h>
#include "..\Camera\CameraManager.h"

class Init
{
public:
	GLFWwindow* window;
	//FPS
	static float deltaTime;
	static float lastFrame;
	static unsigned int width, height;

	Init(unsigned int w = 800, unsigned int h = 600, bool d = true, int s = 0, bool c = true, std::string t = "LearnOpenGL");
	~Init();

private:
	//MOUSE
	static float lastX, lastY;
	static bool firstMouse;

	bool debugLibrary = true;
	bool cursor = true;
	int samples = 0;
	std::string title = "LearnOpenGL";

	void initializeGLFW();
	int initializeGLAD();
	void initializeDebug();
	int createWindow();
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void setCallbacks();
};

#endif