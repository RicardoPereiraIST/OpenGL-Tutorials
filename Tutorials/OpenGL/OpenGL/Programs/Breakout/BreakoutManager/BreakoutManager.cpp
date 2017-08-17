#pragma once

#include "BreakoutManager.h"

#include <glfw3.h>
#include "..\..\..\Includes\SpriteRenderers.h"
#include "..\..\..\Random data\multiple_vertices.h"

BreakoutManager *BreakoutManager::manager = NULL;

BreakoutManager::BreakoutManager(unsigned int w, unsigned int h) : state(GAME_ACTIVE), keys(), width(w), height(h) {
	init();
}

BreakoutManager::~BreakoutManager() {
	delete player;
	delete ball;
}

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

	TextureLoader *background = new TextureLoader();
	background->load("Images/Breakout/background.jpg");
	TextureManager::instance()->add("background", background);

	TextureLoader *block = new TextureLoader();
	block->load("Images/Breakout/block.png");
	TextureManager::instance()->add("block", block);

	TextureLoader *block_solid = new TextureLoader();
	block_solid->load("Images/Breakout/block_solid.png");
	TextureManager::instance()->add("block_solid", block_solid);

	TextureLoader *paddle = new TextureLoader();
	paddle->load("Images/Breakout/paddle.png");
	TextureManager::instance()->add("paddle", paddle);

	VertexBuffers *spriteQuad = new VertexBuffers(spriteVertices, false, true, 2, true);
	VertexManager::instance()->add("sprite", spriteQuad);

	SpriteRenderer *sr = new SpriteRenderer("sprite", "sprite");
	SpriteRendererManager::instance()->add("sprite", sr);

	GameLevel one, two, three, four;
	one.load("Programs/Breakout/GameLevel/Levels/1.lvl", width, height * 0.5f);
	two.load("Programs/Breakout/GameLevel/Levels/2.lvl", width, height * 0.5f);
	three.load("Programs/Breakout/GameLevel/Levels/3.lvl", width, height * 0.5f);
	four.load("Programs/Breakout/GameLevel/Levels/4.lvl", width, height * 0.5f);
	levels.push_back(one);
	levels.push_back(two);
	levels.push_back(three);
	levels.push_back(four);

	glm::vec2 playerPos = glm::vec2(width / 2 - PLAYER_SIZE.x / 2, height - PLAYER_SIZE.y);
	player = new GameObject(playerPos, PLAYER_SIZE, "paddle");

	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	ball = new Ball(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, "face");
}

BreakoutManager* BreakoutManager::instance(unsigned int w, unsigned int h) {
	if(manager == NULL)
		manager = new BreakoutManager(w, h);
	return manager;
}

void BreakoutManager::processInput(float dt) {
	if (state == GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * dt;

		if (keys[GLFW_KEY_A])
		{
			if (player->position.x >= 0)
				player->position.x -= velocity;
			if (ball->stuck)
				ball->position.x -= velocity;
		}
		if (keys[GLFW_KEY_D])
		{
			if (player->position.x <= width - player->size.x)
				player->position.x += velocity;
			if (ball->stuck)
				ball->position.x += velocity;
		}
		if (keys[GLFW_KEY_SPACE])
			ball->stuck = false;
	}
}

void BreakoutManager::update(float dt) {
	ball->move(dt, width);
	doCollisions();
}

void BreakoutManager::render() {
	if (state == GAME_ACTIVE)
	{
		SpriteRendererManager::instance()->get("sprite")->draw("background", glm::vec2(0, 0), glm::vec2(width, height), 0.0f, glm::vec3(1.0f,1.0f,1.0f));
		levels[level].draw("sprite");
		player->draw("sprite");
		ball->draw("sprite");
	}
}

void BreakoutManager::doCollisions()
{
	for (GameObject &box : levels[level].bricks)
	{
		if (!box.destroyed)
		{
			if (checkCollision(*ball, box))
			{
				if (!box.isSolid)
					box.destroyed = true;
			}
		}
	}
}

bool BreakoutManager::checkCollision(GameObject &first, GameObject &second) {
	bool collisionX = first.position.x + first.size.x >= second.position.x &&
		second.position.x + second.size.x >= first.position.x;

	bool collisionY = first.position.y + first.size.y >= second.position.y &&
		second.position.y + second.size.y >= first.position.y;

	return collisionX && collisionY;
}

bool BreakoutManager::checkCollision(Ball &first, GameObject &second) {
	// Get center point circle first 
	glm::vec2 center(first.position + first.radius);

	// Calculate AABB info (center, half-extents)
	glm::vec2 aabb_half_extents(second.size.x / 2, second.size.y / 2);
	glm::vec2 aabb_center(
		second.position.x + aabb_half_extents.x,
		second.position.y + aabb_half_extents.y
	);

	// Get difference vector between both centers
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

	// Add clamped value to AABB_center and we get the value of box closest to circle
	glm::vec2 closest = aabb_center + clamped;

	// Retrieve vector between center circle and closest point AABB and check if length <= radius
	difference = closest - center;
	return glm::length(difference) < first.radius;
}