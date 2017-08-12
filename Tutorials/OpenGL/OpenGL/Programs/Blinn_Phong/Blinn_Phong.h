#pragma once

#include "..\..\Includes\Engine.h"

GLFWwindow* window;
const unsigned int width = 800, height = 600;

//FPS
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//MOUSE
float lastX = width / 2, lastY = height / 2;
bool firstMouse = true;

bool blinn = false;
bool blinnKeyPressed = false;

bool point = true;
bool pointKeyPressed = false;
bool dir = true;
bool dirKeyPressed = false;
bool spot = true;
bool spotKeyPressed = false;

bool gamma = false;
bool gammaKeyPressed = false;

bool framebuffer = false;
bool framebufferKeyPressed = false;

void initializeGLFW();
int initializeGLAD();
void initializeDebug();
void setCallbacks();
int createWindow();
void gameLoop();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow*);
void setShader();
void setTextures();
void setVertices();
void drawObjects();
void setLights();
void setFrameBuffers();
void updateSpotLight();