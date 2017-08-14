#pragma once

#include "..\..\Includes\Engine.h"

const unsigned int WIDTH = 800, HEIGHT = 600;
Init init = Init();

std::vector<glm::vec3> windows;
std::map<float, glm::vec3> sorted;

bool wireframe = false;
bool wireframeKeyPressed = false;
bool trueFps = false;
bool trueFpsKeyPressed = false;
bool blurScreen = false;
bool blurKeyPressed = false;

//sky and outline (stencil) don't work properly together in this scene
bool stencil = false;
bool blend = true;
bool face_cull = true;
bool framebuffer = true;
bool sky = true;

void gameLoop();
void processInput(GLFWwindow*);
void setShader();
void setTextures();
void setVertices();
void setWindows();
void drawObjects();
void setFrameBuffers();