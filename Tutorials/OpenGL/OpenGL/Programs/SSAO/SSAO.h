#pragma once

#include "..\..\Includes\Engine.h"
#include <random>

GLFWwindow* window;
const unsigned int width = 800, height = 600;

//FPS
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//MOUSE
float lastX = width / 2, lastY = height / 2;
bool firstMouse = true;

glm::vec3 lightPos = glm::vec3(2.0, 4.0, -2.0);
glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);

std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between 0.0 - 1.0
std::default_random_engine generator;
std::vector<glm::vec3> ssaoKernel;
std::vector<glm::vec3> ssaoNoise;

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
void updatePointLights();

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}