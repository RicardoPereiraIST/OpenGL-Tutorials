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

std::vector<glm::vec3> windows;
std::map<float, glm::vec3> sorted;

bool wireframe = false;
bool wireframeKeyPressed = false;
bool trueFps = false;
bool trueFpsKeyPressed = false;
bool blurScreen = false;
bool blurKeyPressed = false;

//sky and outline (stencil) don't work properly together in this scene
bool stencil = false;
bool blend = true;
bool face_cull = true;
bool framebuffer = true;
bool sky = true;

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
void setWindows();
void drawObjects();
void setFrameBuffers();