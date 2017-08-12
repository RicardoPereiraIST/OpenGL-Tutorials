#pragma once

#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include <glm\glm.hpp>
#include "Light.h"
#include "..\Shader\ShaderManager.h"

class Point_Light : public Light {
	private:
		glm::vec3 position;
		float constant;
		float linear;
		float quadratic;
		float radius;

		int index;

		void updatePosition() {
			useShader();
			ShaderManager::instance()->get(shader)->setUniform(("pointLights[" + std::to_string(index) + "].position").c_str(), position);
		}

		void updateConstant() {
			useShader();
			ShaderManager::instance()->get(shader)->setUniform(("pointLights[" + std::to_string(index) + "].constant").c_str(), constant);
		}

		void updateLinear() {
			useShader();
			ShaderManager::instance()->get(shader)->setUniform(("pointLights[" + std::to_string(index) + "].linear").c_str(), linear);
		}

		void updateQuadratic() {
			useShader();
			ShaderManager::instance()->get(shader)->setUniform(("pointLights[" + std::to_string(index) + "].quadratic").c_str(), quadratic);
		}

		void updateRadius(int inverted_epsilon) {
			useShader();
			//fmaxf of the color, which in this case is the specular value - could be other. find other solution
			const float maxBrightness = std::fmaxf(std::fmaxf(specular.r, specular.g), specular.b);
			float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (inverted_epsilon)* maxBrightness))) / (2.0f * quadratic);
			ShaderManager::instance()->get(shader)->setUniform(("pointLights[" + std::to_string(index) + "].radius").c_str(), radius);
		}

		void updateAmbient() {
			useShader();
			ShaderManager::instance()->get(shader)->setUniform(("pointLights[" + std::to_string(index) + "].ambient").c_str(), ambient);
		}

		void updateDiffuse() {
			useShader();
			ShaderManager::instance()->get(shader)->setUniform(("pointLights[" + std::to_string(index) + "].diffuse").c_str(), diffuse);
		}

		void updateSpecular() {
			useShader();
			ShaderManager::instance()->get(shader)->setUniform(("pointLights[" + std::to_string(index) + "].specular").c_str(), specular);
		}

		void updateColor() {
			useShader();
			ShaderManager::instance()->get(shader)->setUniform(("pointLights[" + std::to_string(index) + "].color").c_str(), color);
		}

	public:
		Point_Light(){}

		Point_Light(std::string sh, glm::vec3 a, glm::vec3 d, glm::vec3 s, glm::vec3 pos, float c, float l, float q, int i, float inverted_epsilon = 0){
			shader = sh;
			index = i;
			setPosition(pos);
			setAmbient(a);
			setDiffuse(d);
			setSpecular(s);
			setConstant(c);
			setLinear(l);
			setQuadratic(q);

			if (inverted_epsilon > 0)
				setRadius(inverted_epsilon);
		}

		Point_Light(std::string sh, glm::vec3 pos, glm::vec3 c, int i){
			shader = sh;
			index = i;
			setPosition(pos);
			setColor(c);
		}

		void setPosition(glm::vec3 p) {
			position = p;
			updatePosition();
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

		void setRadius(float r) {
			radius = r;
			updateRadius(r);
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