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
	delete particles;
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

	Shader *particleSh = new Shader("Shaders/Breakout/particle.vs", "Shaders/Breakout/particle.fs");
	particleSh->use();
	particleSh->setUniform("sprite", 0);
	particleSh->setUniform("projection", projection);
	ShaderManager::instance()->add("particle", particleSh);

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

	TextureLoader *particleTex = new TextureLoader();
	particleTex->load("Images/Breakout/particle.png");
	TextureManager::instance()->add("particle", particleTex);

	VertexBuffers *spriteQuad = new VertexBuffers(spriteVertices, false, true, 2, true);
	VertexManager::instance()->add("sprite", spriteQuad);

	VertexBuffers *particleQuad = new VertexBuffers(particle_quad, false, true, 2);
	VertexManager::instance()->add("particle", particleQuad);

	SpriteRenderer *sr = new SpriteRenderer("sprite", "sprite");
	SpriteRendererManager::instance()->add("sprite", sr);

	particles = new ParticleGenerator("particle", "particle", "particle", 500);

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
	particles->update(dt, *ball, 2, glm::vec2(ball->radius / 2));

	if (ball->position.y >= height)
	{
		resetLevel();
		resetPlayer();
	}
}

void BreakoutManager::render() {
	if (state == GAME_ACTIVE)
	{
		SpriteRendererManager::instance()->get("sprite")->draw("background", glm::vec2(0, 0), glm::vec2(width, height), 0.0f, glm::vec3(1.0f,1.0f,1.0f));
		levels[level].draw("sprite");
		player->draw("sprite");
		particles->draw();
		ball->draw("sprite");
	}
}

void BreakoutManager::doCollisions()
{
	for (GameObject &box : levels[level].bricks)
	{
		if (!box.destroyed)
		{
			collision collision = checkCollision(*ball, box);
			if (std::get<0>(collision))
			{
				if (!box.isSolid)
					box.destroyed = true;

				// Collision resolution
				Direction::Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (dir == Direction::LEFT || dir == Direction::RIGHT)
				{
					ball->velocity.x = -ball->velocity.x; // Reverse horizontal velocity
					// Relocate
					float penetration = ball->radius - std::abs(diff_vector.x);
					ball->position.x += dir == Direction::LEFT ? penetration : -penetration;
				}
				else
				{
					ball->velocity.y = -ball->velocity.y; // Reverse vertical velocity
					// Relocate
					float penetration = ball->radius - std::abs(diff_vector.y);
					ball->position.y += dir == Direction::UP ? -penetration : penetration;
				}
			}
		}
	}

	collision result = checkCollision(*ball, *player);
	if (!ball->stuck && std::get<0>(result))
	{
		// Check where it hit the board, and change velocity based on where it hit the board
		float centerBoard = player->position.x + player->size.x / 2;
		float distance = (ball->position.x + ball->radius) - centerBoard;
		float percentage = distance / (player->size.x / 2);
		// Then move accordingly
		float strength = 2.0f;
		glm::vec2 oldvelocity = ball->velocity;
		ball->velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		ball->velocity = glm::normalize(ball->velocity) * glm::length(oldvelocity); // Keep speed consistent over both axes (multiply by length of old velocity, so total strength is not changed)
		// Fix sticky paddle
		ball->velocity.y = -1 * abs(ball->velocity.y);
	}
}

bool BreakoutManager::checkCollision(GameObject &first, GameObject &second) {
	bool collisionX = first.position.x + first.size.x >= second.position.x &&
		second.position.x + second.size.x >= first.position.x;

	bool collisionY = first.position.y + first.size.y >= second.position.y &&
		second.position.y + second.size.y >= first.position.y;

	return collisionX && collisionY;
}

collision BreakoutManager::checkCollision(Ball &first, GameObject &second) {
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
	
	if (glm::length(difference) < first.radius) // not <= since in that case a collision also occurs when object one exactly touches object two, which they are at the end of each collision resolution stage.
		return std::make_tuple(true, vectorDirection(difference), difference);
	else
		return std::make_tuple(false, Direction::UP, glm::vec2(0,0));
}

void BreakoutManager::resetLevel()
{
	levels[level].load(("Programs/Breakout/GameLevel/Levels/"+std::to_string(level+1)+".lvl").c_str(), width, height * 0.5f);
}

void BreakoutManager::resetPlayer()
{
	player->size = PLAYER_SIZE;
	player->position = glm::vec2(width / 2 - PLAYER_SIZE.x / 2, height - PLAYER_SIZE.y);
	ball->reset(player->position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
}

Direction::Direction BreakoutManager::vectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),	// up
		glm::vec2(1.0f, 0.0f),	// right
		glm::vec2(0.0f, -1.0f),	// down
		glm::vec2(-1.0f, 0.0f)	// left
	};
	float max = 0.0f;
	unsigned int best_match = -1;
	for (unsigned int i = 0; i < 4; i++)
	{
		float dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction::Direction)best_match;
}