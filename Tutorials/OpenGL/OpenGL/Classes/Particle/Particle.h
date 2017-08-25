#pragma once

#ifndef PARTICLE_H
#define PARTICLE_H

#include <glad\glad.h>
#include <glm\glm.hpp>
#include <iostream>

class Particle {	
	public:
		glm::vec2 position, velocity;
		glm::vec4 color;
		float scale;
		float life;

		Particle() : position(0.0f), velocity(0.0f), color(1.0f), scale(10.0f), life(0.0f) {}
};

#endif