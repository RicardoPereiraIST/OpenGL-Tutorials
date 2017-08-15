#pragma once

#ifndef MESH_H
#define MESH_H

#include <assimp\Importer.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "..\Shader\ShaderManager.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
};

struct Texture {
	unsigned int id;
	std::string type;
	aiString path;
};

class Mesh {
	private:
		/*  Render data  */
		unsigned int VAO, VBO, EBO;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		/*  Functions    */
		void setupMesh() {
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);

			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
				&indices[0], GL_STATIC_DRAW);

			int attrib = 0;

			// vertex positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			// vertex normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
			// vertex texture coords
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

			// vertex tangent
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
			// vertex bitangent
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

			glBindVertexArray(0);
		}

	public:
		/*  Functions  */
		Mesh(std::vector<Vertex> v, std::vector<unsigned int> i, std::vector<Texture> t, bool inst = false) {
			vertices = v;
			indices = i;
			textures = t;
			setupMesh();
		}

		void setInstances(int attrib) {
			glBindVertexArray(VAO);
			// set attribute pointers for matrix (4 times vec4)
			glEnableVertexAttribArray(attrib);
			glVertexAttribPointer(attrib, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glVertexAttribDivisor(attrib++, 1);
			glEnableVertexAttribArray(attrib);
			glVertexAttribPointer(attrib, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glVertexAttribDivisor(attrib++, 1);
			glEnableVertexAttribArray(attrib);
			glVertexAttribPointer(attrib, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glVertexAttribDivisor(attrib++, 1);
			glEnableVertexAttribArray(attrib);
			glVertexAttribPointer(attrib, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
			glVertexAttribDivisor(attrib++, 1);

			glBindVertexArray(0);
		}

		void Draw(std::string n, bool inst, int amount) {
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int normalNr = 1;
			unsigned int heightNr = 1;
			unsigned int reflectNr = 1;
			for (unsigned int i = 0; i < textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
												  // retrieve texture number (the N in diffuse_textureN)
				std::stringstream ss;
				std::string number;
				std::string name = textures[i].type;
				if (name == "texture_diffuse")
					ss << diffuseNr++; // transfer unsigned int to stream
				else if (name == "texture_specular")
					ss << specularNr++; // transfer unsigned int to stream
				else if (name == "texture_normal")
					ss << normalNr++; // transfer unsigned int to stream
				else if (name == "texture_height")
					ss << heightNr++; // transfer unsigned int to stream
				else if (name == "texture_reflection")
					ss << reflectNr++; // transfer unsigned int to stream
				number = ss.str();

				ShaderManager::instance()->get(n)->setUniform((name + number).c_str(), i);
				glBindTexture(GL_TEXTURE_2D, textures[i].id);
			}

			// draw mesh
			glBindVertexArray(VAO);
			if(inst)
				glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, amount);
			else
				glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

			glBindVertexArray(0);
			glActiveTexture(GL_TEXTURE0);
		}
};

#endif;