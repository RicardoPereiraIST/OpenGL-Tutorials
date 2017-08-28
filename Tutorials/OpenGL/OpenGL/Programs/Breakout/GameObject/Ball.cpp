#include "Ball.h"

Ball::Ball()
	: GameObject(), radius(12.5f), stuck(true) { }

Ball::Ball(glm::vec2 pos, float radius, glm::vec2 velocity, std::string sprite)
	: GameObject(pos, glm::vec2(radius * 2, radius * 2), sprite, glm::vec3(1.0f), velocity), radius(radius), stuck(true), sticky(false), passThrough(false) {}

glm::vec2 Ball::move(float dt, unsigned int window_width)
{
	if (!stuck)
	{
		position += velocity * dt;
		if (position.x <= 0.0f)
		{
			velocity.x = -velocity.x;
			position.x = 0.0f;
		}
		else if (position.x + size.x >= window_width)
		{
			velocity.x = -velocity.x;
			position.x = window_width - size.x;
		}
		if (position.y <= 0.0f)
		{
			velocity.y = -velocity.y;
			position.y = 0.0f;
		}
	}
	return position;
}

void Ball::reset(glm::vec2 p, glm::vec2 v)
{
	position = p;
	velocity = v;
	stuck = true;
	sticky = false;
	passThrough = false;
}