#pragma once

#include "BreakoutManager.h"

#include "..\..\..\Includes\SpriteRenderers.h"
#include "..\..\..\Random data\multiple_vertices.h"

BreakoutManager *BreakoutManager::manager = NULL;

BreakoutManager::BreakoutManager(unsigned int w, unsigned int h) : state(GAME_ACTIVE), keys(), width(w), height(h) {
	init();
}

BreakoutManager::~BreakoutManager() {}

void BreakoutManager::destroy() {
	delete manager;
	manager = NULL;
}

void BreakoutManager::init() {
	Shader *sprite = new Shader("Shaders/Breakout/sprite.vs", "Shaders/Breakout/sprite.fs");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(width), static_cast<GLfloat>(height), 0.0f, -1.0f, 1.0f);
	sprite->use();
	sprite->setUniform("image", 0);
	sprite->setUniform("projection", projection);
	ShaderManager::instance()->add("sprite", sprite);

	TextureLoader *face = new TextureLoader();
	face->load("Images/faces/awesomeface.png");
	TextureManager::instance()->add("face", face);

	VertexBuffers *spriteQuad = new VertexBuffers(spriteVertices, false, true, 2, true);
	VertexManager::instance()->add("sprite", spriteQuad);

	SpriteRenderer *sr = new SpriteRenderer("sprite", "sprite", "face");
	SpriteRendererManager::instance()->add("sprite", sr);
}

BreakoutManager* BreakoutManager::instance(unsigned int w, unsigned int h) {
	if(manager == NULL)
		manager = new BreakoutManager(w, h);
	return manager;
}

void BreakoutManager::processInput(float dt) {

}

void BreakoutManager::update(float dt) {

}

void BreakoutManager::render() {
	SpriteRendererManager::instance()->get("sprite")->draw(glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}