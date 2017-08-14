#pragma once

#include "..\..\Includes\Engine.h"

const unsigned int WIDTH = 800, HEIGHT = 600;
Init init = Init();

unsigned int amount = 1000;
glm::vec2 translations[100];
glm::mat4 *modelMatrices;

int createWindow();
void gameLoop();
void processInput(GLFWwindow*);
void setShader();
void setVertices();
void drawObjects();
void setModels();
void setModelMatrices();