#pragma once

#include "..\..\Includes\Engine.h"
#include "BreakoutManager\BreakoutManager.h"

const unsigned int WIDTH = 800, HEIGHT = 600;
Init init = Init(WIDTH, HEIGHT, true, 8, true, "Breakout", false);
BreakoutManager *manager;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void gameLoop();