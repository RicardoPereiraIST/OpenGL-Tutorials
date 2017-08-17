#pragma once

#ifndef BREAKOUTMANAGER_H
#define BREAKOUTMANAGER_H

#include "..\GameLevel\GameLevel.h"
#include <glm\glm.hpp>
#include "..\GameObject\Ball.h"

enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

class BreakoutManager {
private:
	static BreakoutManager *manager;

	const glm::vec2 PLAYER_SIZE = glm::vec2(100, 20);
	const float PLAYER_VELOCITY = 500.0f;
	const glm::vec2 INITIAL_BALL_VELOCITY = glm::vec2(100.0f, -350.0f);
	const float BALL_RADIUS = 12.5f;

	GameObject *player;
	Ball *ball;

	std::vector<GameLevel> levels;
	unsigned int level = 0;

	BreakoutManager(unsigned int w, unsigned int h);
	~BreakoutManager();

	void init();
	void doCollisions();
	bool checkCollision(GameObject &first, GameObject &second);
	bool checkCollision(Ball &first, GameObject &second);

public:
	GameState state;
	bool keys[1024];
	unsigned int width, height;

	static BreakoutManager* instance(unsigned int w = 800, unsigned int h = 600);

	static void destroy();

	void processInput(float dt);

	void update(float dt);

	void render();
};

#endif