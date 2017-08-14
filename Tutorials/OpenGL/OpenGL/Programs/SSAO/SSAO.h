#pragma once

#include "..\..\Includes\Engine.h"
#include <random>

const unsigned int WIDTH = 800, HEIGHT = 600;
Init init = Init();

glm::vec3 lightPos = glm::vec3(2.0, 4.0, -2.0);
glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);

std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between 0.0 - 1.0
std::default_random_engine generator;
std::vector<glm::vec3> ssaoKernel;
std::vector<glm::vec3> ssaoNoise;

void gameLoop();
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