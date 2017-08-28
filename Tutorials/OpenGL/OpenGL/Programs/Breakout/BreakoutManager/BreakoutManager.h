#pragma once

#ifndef BREAKOUTMANAGER_H
#define BREAKOUTMANAGER_H

#include "..\GameLevel\GameLevel.h"
#include <glm\glm.hpp>
#include "..\GameObject\Ball.h"
#include "..\..\..\Classes\Particle\ParticleGenerator.h"
#include "..\GameObject\PowerUp.h"
#include <irrKlang\irrKlang.h>

enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

namespace Direction {
	enum Direction {
		UP,
		RIGHT,
		DOWN,
		LEFT
	};
}

typedef std::tuple<bool, Direction::Direction, glm::vec2> collision; // <collision?, what direction?, difference vector center - closest point>

class BreakoutManager {
private:
	static BreakoutManager *manager;

	GameState state;
	unsigned int width, height;

	const glm::vec2 PLAYER_SIZE = glm::vec2(100, 20);
	const float PLAYER_VELOCITY = 500.0f;
	const glm::vec2 INITIAL_BALL_VELOCITY = glm::vec2(0.0f, -360.0f);
	const float BALL_RADIUS = 12.5f;
	const float BOOST = 100.0f;

	GameObject *player;
	Ball *ball;
	ParticleGenerator *particles;

	std::vector<GameLevel> levels;
	unsigned int level = 0;

	std::vector<PowerUp>  PowerUps;

	unsigned int lives = 3;

	bool shake = false;
	float shakeTime = 0.0f;
	bool confuse = false;
	bool chaos = false;

	irrklang::ISoundEngine *soundEngine = irrklang::createIrrKlangDevice();

	BreakoutManager(unsigned int w, unsigned int h);
	~BreakoutManager();

	void init();
	void doCollisions();
	bool checkCollision(GameObject &first, GameObject &second);
	collision checkCollision(Ball &first, GameObject &second);
	Direction::Direction vectorDirection(glm::vec2 closest);

	void resetLevel();
	void resetPlayer();

	void spawnPowerUps(GameObject &block);
	void activatePowerUp(PowerUp &powerUp);
	void updatePowerUps(float dt);
	bool shouldSpawn(unsigned int chance);
	bool isOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type);

public:
	bool keys[1024];
	bool keysProcessed[1024];

	static BreakoutManager* instance(unsigned int w = 800, unsigned int h = 600);

	static void destroy();

	void processInput(float dt);

	void update(float dt);

	void render();
};

#endif