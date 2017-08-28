#pragma once

#include "BreakoutManager.h"

#include <glfw3.h>
#include "..\..\..\Includes\SpriteRenderers.h"
#include "..\..\..\Random data\multiple_vertices.h"
#include "..\..\..\Includes\FrameBuffers.h"
#include "..\..\..\Includes\TextRenderers.h"
#include <algorithm>

BreakoutManager *BreakoutManager::manager = NULL;

BreakoutManager::BreakoutManager(unsigned int w, unsigned int h) : state(GAME_MENU), keys(), width(w), height(h) {
	init();
}

BreakoutManager::~BreakoutManager() {
	delete player;
	delete ball;
	delete particles;
	soundEngine->drop();
}

void BreakoutManager::destroy() {
	delete manager;
	manager = NULL;
}

void BreakoutManager::init() {
	Shader *sprite = new Shader("Shaders/Breakout/sprite.vs", "Shaders/Breakout/sprite.fs");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
	sprite->use();
	sprite->setUniform("image", 0);
	sprite->setUniform("projection", projection);
	ShaderManager::instance()->add("sprite", sprite);

	Shader *particleSh = new Shader("Shaders/Breakout/particle.vs", "Shaders/Breakout/particle.fs");
	particleSh->use();
	particleSh->setUniform("sprite", 0);
	particleSh->setUniform("projection", projection);
	ShaderManager::instance()->add("particle", particleSh);

	Shader *effectsSh = new Shader("Shaders/Breakout/effects.vs", "Shaders/Breakout/effects.fs");
	effectsSh->use();
	effectsSh->setUniform("scene", 0);
	effectsSh->setUniform("offsets", 9, offsets);
	effectsSh->setUniform("edge_kernel", 9, edge_kernel);
	effectsSh->setUniform("blur_kernel", 9, blur_kernel);
	ShaderManager::instance()->add("effects", effectsSh);

	Shader* text = new Shader("Shaders/Text/text.vs", "Shaders/Text/text.fs");
	text->use();
	text->setUniform("projection", projection);
	ShaderManager::instance()->add("text", text);

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

	TextureLoader *tex_speed = new TextureLoader();
	tex_speed->load("Images/Breakout/powerup_speed.png");
	TextureManager::instance()->add("tex_speed", tex_speed);

	TextureLoader *tex_sticky = new TextureLoader();
	tex_sticky->load("Images/Breakout/powerup_sticky.png");
	TextureManager::instance()->add("tex_sticky", tex_sticky);

	TextureLoader *tex_pass = new TextureLoader();
	tex_pass->load("Images/Breakout/powerup_passthrough.png");
	TextureManager::instance()->add("tex_pass", tex_pass);

	TextureLoader *tex_size = new TextureLoader();
	tex_size->load("Images/Breakout/powerup_increase.png");
	TextureManager::instance()->add("tex_size", tex_size);

	TextureLoader *tex_confuse = new TextureLoader();
	tex_confuse->load("Images/Breakout/powerup_confuse.png");
	TextureManager::instance()->add("tex_confuse", tex_confuse);

	TextureLoader *tex_chaos = new TextureLoader();
	tex_chaos->load("Images/Breakout/powerup_chaos.png");
	TextureManager::instance()->add("tex_chaos", tex_chaos);

	VertexBuffers *spriteQuad = new VertexBuffers(spriteVertices, false, true, 2, true);
	VertexManager::instance()->add("sprite", spriteQuad);

	VertexBuffers *particleQuad = new VertexBuffers(particle_quad, false, true, 2);
	VertexManager::instance()->add("particle", particleQuad);

	SpriteRenderer *sr = new SpriteRenderer("sprite", "sprite");
	SpriteRendererManager::instance()->add("sprite", sr);

	particles = new ParticleGenerator("particle", "particle", "particle", 500);

	VertexBuffers *effectsQuad = new VertexBuffers();
	effectsQuad->createFrameQuad(effects_quad, false, true);
	VertexManager::instance()->add("effects", effectsQuad);
	TextureLoader *effectsTex = new TextureLoader();
	effectsTex->createTexture(width, height, false, false, GL_UNSIGNED_BYTE, GL_LINEAR, GL_REPEAT);
	TextureManager::instance()->add("effects", effectsTex);
	FrameBuffer *f1 = new FrameBuffer("effects", width, height, 8, false, true);
	FrameBufferManager::instance()->add("effects", f1);

	TextRenderer *arial = new TextRenderer("text", "Fonts/Arial/arial.ttf");
	TextRendererManager::instance()->add("arial", arial);

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

	soundEngine->play2D("Music/Tracks/breakout.mp3", true);
}

BreakoutManager* BreakoutManager::instance(unsigned int w, unsigned int h) {
	if(manager == NULL)
		manager = new BreakoutManager(w, h);
	return manager;
}

void BreakoutManager::processInput(float dt) {
	if (state == GAME_MENU)
	{
		if (keys[GLFW_KEY_ENTER] && !keysProcessed[GLFW_KEY_ENTER]) {
			state = GAME_ACTIVE;
			keysProcessed[GLFW_KEY_ENTER] = true;
		}
		if (keys[GLFW_KEY_W] && !keysProcessed[GLFW_KEY_W]) {
			level = (level + 1) % levels.size();
			keysProcessed[GLFW_KEY_W] = true;
		}
		if (keys[GLFW_KEY_S] && !keysProcessed[GLFW_KEY_S])
		{
			if (level > 0)
				--level;
			else
				level = 3;
			keysProcessed[GLFW_KEY_S] = true;
		}
	}

	if (state == GAME_WIN)
	{
		if (keys[GLFW_KEY_ENTER])
		{
			keysProcessed[GLFW_KEY_ENTER] = true;
			chaos = false;
			state = GAME_MENU;
		}
	}

	if (state == GAME_ACTIVE)
	{
		float velocity = PLAYER_VELOCITY * dt;

		if (keys[GLFW_KEY_A])
		{
			if (player->position.x >= 0) {
				player->position.x -= velocity;
				if (ball->stuck)
					ball->position.x -= velocity;
			}
		}
		if (keys[GLFW_KEY_D])
		{
			if (player->position.x <= width - player->size.x) {
				player->position.x += velocity;
				if (ball->stuck)
					ball->position.x += velocity;
			}
		}
		if (keys[GLFW_KEY_SPACE])
			ball->stuck = false;
	}
}

void BreakoutManager::update(float dt) {
	ball->move(dt, width);
	doCollisions();
	particles->update(dt, *ball, 2, glm::vec2(ball->radius / 2));
	updatePowerUps(dt);

	if (shakeTime > 0.0f)
	{
		shakeTime -= dt;
		if (shakeTime <= 0.0f)
			shake = false;
	}

	if (ball->position.y >= height)
	{
		lives--;
		if (lives <= 0)
		{
			resetLevel();
			state = GAME_MENU;
		}
		resetPlayer();
	}

	if (state == GAME_ACTIVE && levels[level].isCompleted())
	{
		resetLevel();
		resetPlayer();
		chaos = true;
		state = GAME_WIN;
	}
}

void BreakoutManager::render() {
	if (state == GAME_ACTIVE || state == GAME_MENU || state == GAME_WIN)
	{
		//draw to frame
		FrameBufferManager::instance()->get("effects")->bindMultisampled();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		SpriteRendererManager::instance()->get("sprite")->draw("background", glm::vec2(0, 0), glm::vec2(width, height), 0.0f, glm::vec3(1.0f,1.0f,1.0f));
		levels[level].draw("sprite");
		player->draw("sprite");
		for (PowerUp &powerUp : PowerUps)
			if (!powerUp.destroyed)
				powerUp.draw("sprite");
		particles->draw();
		ball->draw("sprite");

		//end render (blit frame)
		FrameBufferManager::instance()->blitMultisampled("effects", width, height);

		//render quad
		ShaderManager::instance()->get("effects")->use();
		ShaderManager::instance()->get("effects")->setUniform("time", (float)glfwGetTime());
		ShaderManager::instance()->get("effects")->setUniform("confuse", confuse);
		ShaderManager::instance()->get("effects")->setUniform("chaos", chaos);
		ShaderManager::instance()->get("effects")->setUniform("shake", shake);

		TextureManager::instance()->get("effects")->bind(0);
		VertexManager::instance()->get("effects")->draw();

		TextRendererManager::instance()->get("arial")->renderText("Lives: " + std::to_string(lives), 5.0f, 5.0f, 0.5f, glm::vec3(1.0, 1.0f, 1.0f), true);
	}

	if (state == GAME_MENU)
	{
		TextRendererManager::instance()->get("arial")->renderText("Press ENTER to start", 150.0f, height / 2, 1.0f, glm::vec3(1.0, 1.0f, 1.0f), true);
		TextRendererManager::instance()->get("arial")->renderText("Press W or S to select level", 157.5f, height / 2 + 50.0f, 0.75f, glm::vec3(1.0, 1.0f, 1.0f), true);
	}

	if (state == GAME_WIN)
	{
		TextRendererManager::instance()->get("arial")->renderText("You WON!!!", 250.0, height / 2 - 20.0, 1.0, glm::vec3(0.0, 1.0, 0.0), true);
		TextRendererManager::instance()->get("arial")->renderText("Press ENTER to retry or ESC to quit", 70.0, height / 2 + 40, 0.8f, glm::vec3(1.0, 1.0, 0.0), true);
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
				if (!box.isSolid) {
					box.destroyed = true;
					levels[level].destroyedBricks++;
					spawnPowerUps(box);
					soundEngine->play2D("Music/Sounds/bleep.mp3", false);
				}
				else
				{   // if block is solid, enable shake effect
					shakeTime = 0.05f;
					shake = true;
					soundEngine->play2D("Music/Sounds/solid.wav", false);
				}
				// Collision resolution
				Direction::Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (!(ball->passThrough && !box.isSolid))
				{
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
	}

	for (PowerUp &powerUp : PowerUps)
	{
		if (!powerUp.destroyed)
		{
			if (powerUp.position.y >= height)
				powerUp.destroyed = true;
			if (checkCollision(*player, powerUp))
			{	// Collided with player, now activate powerup
				activatePowerUp(powerUp);
				powerUp.destroyed = true;
				powerUp.activated = true;
				soundEngine->play2D("Music/Sounds/powerup.wav", false);
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

		ball->velocity.x = BOOST * percentage * strength;
		ball->velocity = glm::normalize(ball->velocity) * glm::length(oldvelocity); // Keep speed consistent over both axes (multiply by length of old velocity, so total strength is not changed)
		// Fix sticky paddle
		ball->velocity.y = -1 * abs(ball->velocity.y);

		ball->stuck = ball->sticky;

		soundEngine->play2D("Music/Sounds/bleep.wav", false);
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
	shake = false;
	confuse = false;
	chaos = false;
	shakeTime = 0.0f;
	player->color = glm::vec3(1.0f);
	ball->color = glm::vec3(1.0f);
	PowerUps.clear();
	lives = 3;
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

void BreakoutManager::spawnPowerUps(GameObject &block)
{
	if (shouldSpawn(75)) // 1 in 75 chance
		PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.position, "tex_speed"));
	if (shouldSpawn(75))
		PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.position, "tex_sticky"));
	if (shouldSpawn(75))
		PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.position, "tex_pass"));
	if (shouldSpawn(75))
		PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.position, "tex_size"));
	if (shouldSpawn(15)) // Negative powerups should spawn more often
		PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.position, "tex_confuse"));
	if (shouldSpawn(15))
		PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.position, "tex_chaos"));
}

void BreakoutManager::activatePowerUp(PowerUp &powerUp)
{
	// Initiate a powerup based type of powerup
	if (powerUp.type == "speed")
	{
		ball->velocity *= 1.2;
	}
	else if (powerUp.type == "sticky")
	{
		ball->sticky = true;
		player->color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.type == "pass-through")
	{
		ball->passThrough = true;
		ball->color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.type == "pad-size-increase")
	{
		player->size.x += 50;
	}
	else if (powerUp.type == "confuse")
	{
		if (!chaos)
			confuse = true; // Only activate if chaos wasn't already active
	}
	else if (powerUp.type == "chaos")
	{
		if (!confuse)
			chaos = true;
	}
}

void BreakoutManager::updatePowerUps(float dt)
{
	for (PowerUp &powerUp : PowerUps)
	{
		powerUp.position += powerUp.velocity * dt;
		if (powerUp.activated)
		{
			powerUp.duration -= dt;

			if (powerUp.duration <= 0.0f)
			{
				// Remove powerup from list (will later be removed)
				powerUp.activated = false;
				// Deactivate effects
				if (powerUp.type == "sticky")
				{
					if (!isOtherPowerUpActive(PowerUps, "sticky"))
					{	// Only reset if no other PowerUp of type sticky is active
						ball->sticky = false;
						player->color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.type == "pass-through")
				{
					if (!isOtherPowerUpActive(PowerUps, "pass-through"))
					{	// Only reset if no other PowerUp of type pass-through is active
						ball->passThrough = false;
						ball->color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.type == "confuse")
				{
					if (!isOtherPowerUpActive(PowerUps, "confuse"))
					{	// Only reset if no other PowerUp of type confuse is active
						confuse = false;
					}
				}
				else if (powerUp.type == "chaos")
				{
					if (!isOtherPowerUpActive(PowerUps, "chaos"))
					{	// Only reset if no other PowerUp of type chaos is active
						chaos = false;
					}
				}
			}
		}
	}

	PowerUps.erase(std::remove_if(PowerUps.begin(), PowerUps.end(),
		[](const PowerUp &powerUp) { return powerUp.destroyed && !powerUp.activated; }
	), PowerUps.end());
}

bool BreakoutManager::shouldSpawn(unsigned int chance)
{
	unsigned int random = rand() % chance;
	return random == 0;
}

bool BreakoutManager::isOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type)
{
	for (const PowerUp &powerUp : powerUps)
	{
		if (powerUp.activated)
			if (powerUp.type == type)
				return true;
	}
	return false;
}