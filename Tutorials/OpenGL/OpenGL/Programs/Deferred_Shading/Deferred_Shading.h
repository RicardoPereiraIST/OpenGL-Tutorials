#pragma once

#include "..\..\Includes\Engine.h"

const unsigned int WIDTH = 800, HEIGHT = 600;
Init init = Init();

const unsigned int NR_LIGHTS = 32;
std::vector<glm::vec3> lightPositions;
std::vector<glm::vec3> lightColors;
std::vector<glm::vec3> objectPositions;

void gameLoop();
void processInput(GLFWwindow*);
void setShader();
void setTextures();
void setVertices();
void drawObjects();
void setPointLights();
void setFrameBuffers();
void setModels();