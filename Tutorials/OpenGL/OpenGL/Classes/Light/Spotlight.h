#pragma once

#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include <glm\glm.hpp>
#include "Light.h"
#include "..\Shader\ShaderManager.h"

class Spotlight : public Light {
	private:
		float constant;
		float linear;
		float quadratic;

		void updatePosition() {
			useShader();
			ShaderManager::instance()->get(shader)->setUniform("spotLight.position", position);
		}

		void updateDirection() {
			useShader();
			ShaderManager::instance()->get(shader)->setUniform("spotLight.direction", direction);
		}

		void updateConstant() {
			useShader();
			ShaderManager::instance()->get(shader)->setUniform("spotLight.constant", constant);
		}

		void updateLinear() {
			useShader();
			ShaderManager::instance()->get(shader)->setUniform("spotLight.linear", linear);
		}

		void updateQuadratic() {
			useShader();
			ShaderManager::instance()->get(shader)->setUniform("spotLight.quadratic", quadratic);
		}

		void updateAmbient() {
			useShader();
			ShaderManager::instance()->get(shader)->setUniform("spotLight.ambient", ambient);
		}

		void updateDiffuse() {
			useShader();
			ShaderManager::instance()->get(shader)->setUniform("spotLight.diffuse", diffuse);
		}

		void updateSpecular() {
			useShader();
			ShaderManager::instance()->get(shader)->setUniform("spotLight.specular", specular);
		}

		void updateCutoff() {
			useShader();
			ShaderManager::instance()->get(shader)->setUniform("spotLight.cutOff", cutOff);
		}

		void updateOuterCutoff() {
			useShader();
			ShaderManager::instance()->get(shader)->setUniform("spotLight.outerCutOff", outerCutOff);
		}

		void updateColor() {
			useShader();
			ShaderManager::instance()->get(shader)->setUniform("spotLight.color", color);
		}

	public:
		float cutOff;
		float outerCutOff;
		glm::vec3 position;
		glm::vec3 direction;

		Spotlight(){}

		Spotlight(std::string sh, glm::vec3 a, glm::vec3 d, glm::vec3 s, glm::vec3 pos, glm::vec3 dir, float c, float l, float q, float co, float oco){
			shader = sh;
			setPosition(pos);
			setAmbient(a);
			setDiffuse(d);
			setSpecular(s);
			setConstant(c);
			setLinear(l);
			setQuadratic(q);
			setDirection(dir);
			setCutoff(co);
			setOuterCutoff(oco);
		}

		Spotlight(std::string sh, glm::vec3 pos, glm::vec3 dir, glm::vec3 c, float co, float oco) {
			shader = sh;
			setPosition(pos);
			setDirection(dir);
			setCutoff(co);
			setOuterCutoff(oco);
		}

		void setPosition(glm::vec3 p) {
			position = p;
			updatePosition();
		}

		void setDirection(glm::vec3 d) {
			direction = d;
			updateDirection();
		}

		void setConstant(float c) {
			constant = c;
			updateConstant();
		}

		void setLinear(float l) {
			linear = l;
			updateLinear();
		}

		void setQuadratic(float q) {
			quadratic = q;
			updateQuadratic();
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

		void setCutoff(float co) {
			cutOff = co;
			updateCutoff();
		}

		void setOuterCutoff(float oco) {
			outerCutOff = oco;
			updateOuterCutoff();
		}

		void setColor(glm::vec3 c) {
			Light::setColor(c);
			updateColor();
		}
};

#endif