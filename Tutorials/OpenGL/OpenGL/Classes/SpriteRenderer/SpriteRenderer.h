#pragma once

#ifndef SPRITERENDERER_H
#define SPRITERENDERER_H

#include "..\Shader\ShaderManager.h"
#include "..\Vertex\VertexManager.h"
#include "..\Text\TextRendererManager.h"

class SpriteRendererException : public std::runtime_error {
public:
	SpriteRendererException(const std::string & msg) :
		std::runtime_error(msg) { }
};

class SpriteRenderer {
	private:
		std::string shader;
		std::string vertex;
		std::string texture;

	public:
		SpriteRenderer(std::string sh, std::string v, std::string tex) : shader(sh), vertex(v), texture(tex) {}
		~SpriteRenderer(){}

		void draw(glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color) {
			ShaderManager::instance()->get(shader)->use();

			glm::mat4 model;
			model = glm::translate(model, glm::vec3(position, 0.0f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)
			model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // Move origin of rotation to center of quad
			model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
			model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back
			model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale

			ShaderManager::instance()->get(shader)->setUniform("model", model);
			ShaderManager::instance()->get(shader)->setUniform("spriteColor", color);
			TextureManager::instance()->get(texture)->bind(0);
			VertexManager::instance()->get(vertex)->draw();
		}
};

#endif