#pragma once

#ifndef BALL_H
#define BALL_H

#include "GameObject.h"

class Ball : public GameObject
{
public:
	float radius;
	bool stuck;
	bool sticky, passThrough;

	Ball();
	Ball(glm::vec2 pos, float radius, glm::vec2 velocity, std::string sprite);

	glm::vec2 move(float dt, unsigned int window_width);
	void reset(glm::vec2 position, glm::vec2 velocity);
};

#endif