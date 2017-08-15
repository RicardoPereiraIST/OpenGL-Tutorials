#pragma once

#ifndef BREAKOUTMANAGER_H
#define BREAKOUTMANAGER_H

enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

class BreakoutManager {
private:
	static BreakoutManager *manager;

	BreakoutManager(unsigned int w, unsigned int h);
	~BreakoutManager();

	void init();

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