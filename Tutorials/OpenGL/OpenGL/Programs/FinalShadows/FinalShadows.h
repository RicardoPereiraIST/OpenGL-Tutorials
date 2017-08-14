#pragma once

#include "..\..\Includes\Engine.h"

const unsigned int WIDTH = 800, HEIGHT = 600;
Init init = Init();

bool blinn = false;
bool blinnKeyPressed = false;

bool pre_sampler = false;
bool pre_samplerKeyPressed = false;

bool debug = false;
bool debugKeyPressed = false;

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

glm::mat4 lightSpaceMatrix;
glm::mat4 spotSpaceMatrix;

std::vector<glm::mat4> shadowTransforms;

//point and spot
float f = 25.0f, n = 1.0f;;
//dir
float near_plane = 1.0f, far_plane = 7.5f;

float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;

//spot
glm::vec3 spotPos = glm::vec3(6.5f, 1.0f, 1.0f);
glm::vec3 spotDir = glm::vec3(3.0f, -1.0f, 0.0f);
//dir
glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
//point
glm::vec3 pointLightPos(0.0f, 0.0f, 0.0f);

glm::vec3 sampleOffsetDirections[20] = {
	glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, 1.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, 1.0f, -1.0f),
	glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(-1.0f, 1.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, -1.0f), glm::vec3(-1.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 1.0f), glm::vec3(0.0f, -1.0f, -1.0f), glm::vec3(0.0f, 1.0f, -1.0f)
};


void gameLoop();
void processInput(GLFWwindow*);
void setShader();
void setTextures();
void setVertices();
void drawObjects();
void setLights();
void updatePointLights();
void updateSpotLight();
void setFrameBuffers();
void setShadows();
void updateShadows();
void renderScene(std::string shaderName, std::string textureName, bool depth = false);
void draw(std::string vertexBuffer, std::string textureName);