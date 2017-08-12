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

const unsigned int NR_LIGHTS = 32;
std::vector<glm::vec3> lightPositions;
std::vector<glm::vec3> lightColors;
std::vector<glm::vec3> objectPositions;

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
void setPointLights();
void setFrameBuffers();
void setModels();