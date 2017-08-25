#pragma once

#ifndef PARTICLEGENERATOR_H
#define PARTICLEGENERATOR_H

#include "Particle.h"
#include "..\..\Programs\Breakout\GameObject\GameObject.h"
#include <vector>

class ParticleGenerator {
private:
	std::vector<Particle> particles;
	unsigned int amount;

	unsigned int lastUsedParticle = 0;

	std::string shader;
	std::string texture;
	std::string vertex;

	void init() {
		for (unsigned int i = 0; i < amount; ++i)
			this->particles.push_back(Particle());
	}

	unsigned int firstUnusedParticle() {
		// First search from last used particle, this will usually return almost instantly
		for (unsigned int i = lastUsedParticle; i < amount; ++i) {
			if (particles[i].life <= 0.0f) {
				lastUsedParticle = i;
				return i;
			}
		}
		// Otherwise, do a linear search
		for (unsigned int i = 0; i < lastUsedParticle; ++i) {
			if (particles[i].life <= 0.0f) {
				lastUsedParticle = i;
				return i;
			}
		}
		// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
		lastUsedParticle = 0;
		return 0;
	}

	void respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset = glm::vec2(0.0f, 0.0f)) {
		float random = ((rand() % 100) - 50) / 10.0f;
		float rColor = 0.5 + ((rand() % 100) / 100.0f);
		float scaleRandom = ((rand() % 10) + 1);
		particle.position = object.position + random + offset;
		particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
		particle.scale = scaleRandom;
		particle.life = 1.0f;
		particle.velocity = object.velocity * 0.1f;
	}

public:
	ParticleGenerator(std::string sh, std::string tex, std::string ver, unsigned int amt) : shader(sh), texture(tex), vertex(ver), amount(amt){
		init();
	}

	void update(float dt, GameObject &object, GLuint newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f)) {
		// Add new particles 
		for (unsigned int i = 0; i < newParticles; ++i){
			int unusedParticle = firstUnusedParticle();
			respawnParticle(particles[unusedParticle], object, offset);
		}

		// Update all particles
		for (unsigned int i = 0; i < amount; ++i){
			Particle &p = particles[i];
			p.life -= dt; // reduce life
			if (p.life > 0.0f){
				p.position -= p.velocity * dt;
				p.color.a -= dt * 2.5;
			}
		}
	}

	void draw() {
		// Use additive blending to give it a 'glow' effect
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		ShaderManager::instance()->get(shader)->use();
		for (Particle particle : particles)
		{
			if (particle.life > 0.0f)
			{
				ShaderManager::instance()->get(shader)->setUniform("offset", particle.position);
				ShaderManager::instance()->get(shader)->setUniform("color", particle.color);
				ShaderManager::instance()->get(shader)->setUniform("scale", particle.scale);
				TextureManager::instance()->get(texture)->bind(0);
				VertexManager::instance()->get(vertex)->draw();
			}
		}
		// Don't forget to reset to default blending mode
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
};

#endif