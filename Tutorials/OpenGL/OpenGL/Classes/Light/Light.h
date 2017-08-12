#pragma once

#ifndef LIGHT_H
#define LIGHT_H

#include <glm\glm.hpp>
#include <string>

class Light {
protected:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	//if wanted, to facilitate
	glm::vec3 color;

	std::string shader;

	void useShader() {
		ShaderManager::instance()->get(shader)->use();
	}

	virtual void setAmbient(glm::vec3 a) {
		ambient = a;
	}

	virtual void setDiffuse(glm::vec3 d) {
		diffuse = d;
	}

	virtual void setSpecular(glm::vec3 s) {
		specular = s;
	}

	virtual void setColor(glm::vec3 c) {
		color = c;
	}

public:
	Light(){}

	Light(glm::vec3 a, glm::vec3 d, glm::vec3 s) {
		ambient = a;
		diffuse = d;
		specular = s;
	}

	Light(glm::vec3 c) : color(c) {}
};

#endif