#pragma once

#include "..\..\Includes\Engine.h"

const unsigned int WIDTH = 800, HEIGHT = 600;
Init init = Init();

float exposure = 1.0f;

bool hdr = true;
bool hdrKeyPressed = false;

void gameLoop();
void processInput(GLFWwindow*);
void setShader();
void setTextures();
void setVertices();
void drawObjects();
void setFrameBuffers();
void setPointLights();