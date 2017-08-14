#pragma once

#include "..\..\Includes\Engine.h"

const unsigned int WIDTH = 800, HEIGHT = 600;
Init init = Init();

bool toy = false;
bool toyKeyPressed = false;

void gameLoop();
void processInput(GLFWwindow*);
void setShader();
void setTextures();
void setVertices();
void drawObjects();
void setPointLights();
void setFrameBuffers();