#pragma once

#include "..\..\Includes\Engine.h"

const unsigned int WIDTH = 800, HEIGHT = 600;
Init init = Init();

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

void gameLoop();
void processInput(GLFWwindow*);
void setShader();
void setTextures();
void setVertices();
void drawObjects();
void setLights();
void setFrameBuffers();
void updateSpotLight();