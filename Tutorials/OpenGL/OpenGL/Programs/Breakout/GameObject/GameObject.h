#pragma once

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glm\glm.hpp>
#include "..\..\..\Classes\Texture\TextureManager.h"
#include "..\..\..\Classes\SpriteRenderer\SpriteRendererManager.h"

class GameObject
{
public:
	glm::vec2 position, size, velocity;
	glm::vec3 color;
	float rotation;
	bool isSolid;
	bool destroyed;
	std::string sprite;

	GameObject();
	GameObject(glm::vec2 pos, glm::vec2 size, std::string sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	virtual void draw(std::string renderer);
};

#endif