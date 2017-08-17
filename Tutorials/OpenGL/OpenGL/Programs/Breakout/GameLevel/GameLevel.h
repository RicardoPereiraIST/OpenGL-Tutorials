#pragma once

#ifndef GAMELEVEL_H
#define GAMELEVEL_H

#include "..\GameObject\GameObject.h"
#include <vector>

class GameLevel
{
public:
	std::vector<GameObject> bricks;
	GameLevel();
	void load(const char *file, unsigned int levelWidth, unsigned int levelHeight);
	void draw(std::string renderer);
	bool isCompleted();
private:
	static std::map<unsigned int, glm::vec3> colors;

	void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
};

#endif