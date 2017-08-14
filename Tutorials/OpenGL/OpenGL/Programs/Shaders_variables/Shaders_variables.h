#pragma once

#include "..\..\Includes\Engine.h"

const unsigned int WIDTH = 800, HEIGHT = 600;
Init init = Init();

void gameLoop();
void processInput(GLFWwindow*);
void setShader();
void setVertices();
void drawObjects();
void setUbos();