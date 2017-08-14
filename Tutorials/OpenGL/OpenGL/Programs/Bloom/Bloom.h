#pragma once

#include "..\..\Includes\Engine.h"

const unsigned int WIDTH = 800, HEIGHT = 600;
Init init = Init();

std::vector<glm::vec3> lightPositions;

// colors
std::vector<glm::vec3> lightColors;

bool bloom = true;
bool bloomKeyPressed = false;
float exposure = 1.0f;

void gameLoop();
void processInput(GLFWwindow*);
void setShader();
void setTextures();
void setVertices();
void drawObjects();
void setFrameBuffers();
void setPointLights();