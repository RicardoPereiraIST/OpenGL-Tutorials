#pragma once

#ifndef POWERUP_H
#define POWERUP_H

#include <string>
#include <glm/glm.hpp>
#include "GameObject.h"

const glm::vec2 POWERUP_SIZE = glm::vec2(60, 20);
const glm::vec2 POWERUP_VELOCITY = glm::vec2(0.0f, 150.0f);

class PowerUp : public GameObject
{
	public:
		std::string type;
		float duration;
		bool activated;

		PowerUp(std::string type, glm::vec3 color, GLfloat duration, glm::vec2 position, std::string texture)
			: GameObject(position, POWERUP_SIZE, texture, color, POWERUP_VELOCITY), type(type), duration(duration), activated() { }
};

#endif