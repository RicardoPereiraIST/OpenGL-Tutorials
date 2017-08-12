#pragma once

#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

#include <glm\glm.hpp>
#include "Light.h"
#include "..\Shader\ShaderManager.h"

class Directional_Light : public Light {
private:
	glm::vec3 direction;

	void updateDirection() {
		useShader();
		ShaderManager::instance()->get(shader)->setUniform("dirLight.direction", direction);
	}

	void updateAmbient() {
		useShader();
		ShaderManager::instance()->get(shader)->setUniform("dirLight.ambient", ambient);
	}

	void updateDiffuse() {
		useShader();
		ShaderManager::instance()->get(shader)->setUniform("dirLight.diffuse", diffuse);
	}

	void updateSpecular() {
		useShader();
		ShaderManager::instance()->get(shader)->setUniform("dirLight.specular", specular);
	}

	void updateColor() {
		useShader();
		ShaderManager::instance()->get(shader)->setUniform("dirLight.color", color);
	}

public:
	Directional_Light(){}

	Directional_Light(std::string sh, glm::vec3 a, glm::vec3 d, glm::vec3 s, glm::vec3 dir) {
		shader = sh;
		setDirection(dir);
		setAmbient(a);
		setDiffuse(d);
		setSpecular(s);
	}

	Directional_Light(std::string sh, glm::vec3 dir, glm::vec3 c) {
		shader = sh;
		setDirection(dir);
		setColor(c);
	}

	void setDirection(glm::vec3 d) {
		direction = d;
		updateDirection();
	}

	void setAmbient(glm::vec3 a) {
		Light::setAmbient(a);
		updateAmbient();
	}

	void setDiffuse(glm::vec3 d) {
		Light::setDiffuse(d);
		updateDiffuse();
	}

	void setSpecular(glm::vec3 s) {
		Light::setSpecular(s);
		updateSpecular();
	}

	void setColor(glm::vec3 c) {
		Light::setColor(c);
		updateColor();
	}

};

#endif