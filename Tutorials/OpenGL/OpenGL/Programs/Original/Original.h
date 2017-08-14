#include "..\..\Includes\Engine.h"

const unsigned int WIDTH = 800, HEIGHT = 600;
Init init = Init();

bool glass = false;
bool glassKeyPressed = false;

bool refract = false;
bool refractKeyPressed = false;

void gameLoop();
void processInput(GLFWwindow*);
void setShader();
void setTextures();
void setModels();
void setVertices();
void setLights();
void updateSpotLight();
void setAndDrawObjects();