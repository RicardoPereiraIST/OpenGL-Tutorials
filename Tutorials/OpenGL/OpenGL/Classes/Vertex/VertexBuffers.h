#pragma once

#ifndef VERTEX_BUFFERS_H
#define VERTEX_BUFFERS_H

#include <iostream>
#include <string>
#include <vector>
#include "..\Texture\TextureManager.h"

class VertexBuffersException : public std::runtime_error {
public:
	VertexBuffersException(const std::string & msg) :
		std::runtime_error(msg) { }
};

class VertexBuffers {
	private:
		unsigned int vao, vbo, ebo;
		unsigned int instanceVBO;
		bool colors = false, texcoords = false, has_indices = false, has_instances = false, tangents = false, bitangents = false;
		int num_indices, num_vertices, sizeof_vector;
		std::vector<float> vertices;
		std::vector<unsigned int> indices;
		glm::vec2 *instances;
		unsigned int beginningStride = 0;

		const float PI = 3.14159265359;
		
		void create(bool reverse) {
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);

			if(has_indices)
				glGenBuffers(1, &ebo);

			if (has_instances) {
				glGenBuffers(1, &instanceVBO);
				glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vertices.size(), instances, GL_STATIC_DRAW);
			}

			glBindVertexArray(vao);

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

			if (has_indices) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
			}

			int c=0, t=0, ta = 0, bit = 0;
			if (colors) c = 3;
			if (texcoords) t = 2;
			if (tangents) ta = 3;
			if (bitangents) bit = 3;

			int attrib = 0;

			// position attribute
			if (!reverse) {
				glVertexAttribPointer(attrib, beginningStride == 0 ? sizeof_vector : beginningStride, GL_FLOAT, GL_FALSE, (sizeof_vector + c + t + ta + bit) * sizeof(float), (void*)0);
				glEnableVertexAttribArray(attrib++);
				// color attribute
				if (colors) {
					glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, (sizeof_vector + c + t + ta + bit) * sizeof(float), (void*)(sizeof_vector * sizeof(float)));
					glEnableVertexAttribArray(attrib++);
				}
				// texture coord attribute
				if (texcoords) {
					glVertexAttribPointer(attrib, 2, GL_FLOAT, GL_FALSE, (sizeof_vector + c + t + ta + bit) * sizeof(float), (void*)((sizeof_vector + c) * sizeof(float)));
					glEnableVertexAttribArray(attrib++);
				}
			}
			else {
				glVertexAttribPointer(attrib, beginningStride == 0 ? sizeof_vector : beginningStride, GL_FLOAT, GL_FALSE, (sizeof_vector + c + t + ta + bit) * sizeof(float), (void*)0);
				glEnableVertexAttribArray(attrib++);
				// texture coord attribute
				if (texcoords) {
					glVertexAttribPointer(attrib, 2, GL_FLOAT, GL_FALSE, (sizeof_vector + c + t + ta + bit) * sizeof(float), (void*)(sizeof_vector * sizeof(float)));
					glEnableVertexAttribArray(attrib++);
				}
				// color attribute
				if (colors) {
					glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, (sizeof_vector + c + t + ta + bit) * sizeof(float), (void*)((sizeof_vector + t) * sizeof(float)));
					glEnableVertexAttribArray(attrib++);
				}
			}
			if (tangents) {
				glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, (sizeof_vector + c + t + ta + bit) * sizeof(float), (void*)((sizeof_vector + c + t) * sizeof(float)));
				glEnableVertexAttribArray(attrib++);
			}
			if (bitangents) {
				glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, (sizeof_vector + c + t + ta + bit) * sizeof(float), (void*)((sizeof_vector + c + t + ta) * sizeof(float)));
				glEnableVertexAttribArray(attrib++);
			}

			if (has_instances) {
				glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer
				glVertexAttribPointer(attrib, 2, GL_FLOAT, GL_FALSE, sizeof_vector * sizeof(float), (void*)0);
				glEnableVertexAttribArray(attrib++);
				glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.
			}

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(0);
		}

		//Function to create spheres
		void createSphere(unsigned int X_SEGMENTS, unsigned int Y_SEGMENTS) {
			std::vector<glm::vec3> positions;
			std::vector<glm::vec2> uv;
			std::vector<glm::vec3> normals;
			indices.clear();

			for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
			{
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
				{
					float xSegment = (float)x / (float)X_SEGMENTS;
					float ySegment = (float)y / (float)Y_SEGMENTS;
					float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
					float yPos = std::cos(ySegment * PI);
					float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

					positions.push_back(glm::vec3(xPos, yPos, zPos));
					uv.push_back(glm::vec2(xSegment, ySegment));
					normals.push_back(glm::vec3(xPos, yPos, zPos));
				}
			}

			bool oddRow = false;
			for (int y = 0; y < Y_SEGMENTS; ++y)
			{
				if (!oddRow) // even rows: y == 0, y == 2; and so on
				{
					for (int x = 0; x <= X_SEGMENTS; ++x)
					{
						indices.push_back(y       * (X_SEGMENTS + 1) + x);
						indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					}
				}
				else
				{
					for (int x = X_SEGMENTS; x >= 0; --x)
					{
						indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
						indices.push_back(y       * (X_SEGMENTS + 1) + x);
					}
				}
				oddRow = !oddRow;
			}

			num_indices = indices.size();
			vertices.clear();
			for (int i = 0; i < positions.size(); ++i)
			{
				vertices.push_back(positions[i].x);
				vertices.push_back(positions[i].y);
				vertices.push_back(positions[i].z);
				if (uv.size() > 0)
				{
					if (!texcoords) texcoords = true;
					vertices.push_back(uv[i].x);
					vertices.push_back(uv[i].y);
				}
				if (normals.size() > 0)
				{
					if (!colors) colors = true;
					vertices.push_back(normals[i].x);
					vertices.push_back(normals[i].y);
					vertices.push_back(normals[i].z);
				}
			}

			create(true);
		}

		void initialize(std::vector<float> v, bool c, bool t, int size_vector, bool ta, bool bit, unsigned int begStride) {
			vertices = v;
			colors = c;
			texcoords = t;
			tangents = ta;
			bitangents = bit;
			sizeof_vector = size_vector;
			num_vertices = vertices.size() / (sizeof_vector + (c ? 3 : 0) + (t ? 2 : 0) + (ta ? 3 : 0) + (bit ? 3 : 0));
			beginningStride = begStride;
		}

		void initializeInstances(glm::vec2 * inst) {
			has_instances = true;
			instances = inst;
		}

		void initializeIndices(std::vector<unsigned int> i) {
			has_indices = true;
			indices = i;
			num_indices = indices.size();
		}

		void bindAndDraw(GLenum primitive, int instances = 0) {
			glBindVertexArray(vao);

			if (instances > 0) {
				if (has_indices) {
					glDrawElementsInstanced(primitive, num_indices, GL_UNSIGNED_INT, 0, instances);
				}
				else {
					glDrawArraysInstanced(primitive, 0, num_vertices, instances);
				}
			}
			else {
				if (has_indices) {
					glDrawElements(primitive, num_indices, GL_UNSIGNED_INT, 0);
				}
				else {
					glDrawArrays(primitive, 0, num_vertices);
				}
			}

			glBindVertexArray(0);
		}

	public:
		VertexBuffers(){}

		VertexBuffers(std::vector<float> v, bool c, bool t, int size_vector = 3, unsigned int begStride = 0, bool reverse = false) {
			initialize(v, c, t, size_vector, false, false, begStride);
			create(reverse);
		}

		VertexBuffers(std::vector<float> v, bool c, bool t, bool ta, bool bit, int size_vector = 3, unsigned int begStride = 0, bool reverse = false) {
			initialize(v, c, t, size_vector, ta, bit, begStride);
			create(reverse);
		}

		VertexBuffers(std::vector<float> v, bool c, bool t, glm::vec2 * inst, int size_vector = 3, unsigned int begStride = 0, bool reverse = false) {
			initialize(v, c, t, size_vector, false, false, begStride);
			initializeInstances(inst);
			create(reverse);
		}

		VertexBuffers(std::vector<float> v, std::vector<unsigned int> i, bool c, bool t, int size_vector = 3, unsigned int begStride = 0, bool reverse = false) {
			initialize(v, c, t, size_vector, false, false, begStride);
			initializeIndices(i);
			create(reverse);
		}

		VertexBuffers(std::vector<float> v, std::vector<unsigned int> i, bool c, bool t, glm::vec2 * inst, int size_vector = 3, unsigned int begStride = 0, bool reverse = false) {
			initialize(v, c, t, size_vector, false, false, begStride);
			initializeInstances(inst);
			initializeIndices(i);
			create(reverse);
		}

		void initializeSphere(unsigned int X_SEGMENTS = 64, unsigned int Y_SEGMENTS = 64) {
			has_indices = true;
			sizeof_vector = 3;
			createSphere(X_SEGMENTS, Y_SEGMENTS);
		}

		void createGliphQuad() {
			num_vertices = 6;

			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			//The 2D quad requires 6 vertices of 4 floats each so we reserve 6 * 4 floats of memory.
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		void updateGliphQuad(GLfloat v[6][4]) {
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 6 * 4, v); // Be sure to use glBufferSubData and not glBufferData
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		~VertexBuffers() {
			glDeleteVertexArrays(1, &vao);
			glDeleteVertexArrays(1, &vbo);
			glDeleteVertexArrays(1, &ebo);
			glDeleteVertexArrays(1, &instanceVBO);
		}

		void draw(GLenum primitive = GL_TRIANGLES) {
			bindAndDraw(primitive);
		}

		void draw(std::string textureName, GLenum primitive = GL_TRIANGLES, bool cube = false, int active = 0) {
			TextureManager::instance()->get(textureName)->bind(active, -1, cube);
			bindAndDraw(primitive);
		}

		void drawInst(int instances, GLenum primitive = GL_TRIANGLES) {
			bindAndDraw(primitive, instances);
		}

		void drawInst(std::string textureName, int instances, GLenum primitive = GL_TRIANGLES, int active = 0) {
			TextureManager::instance()->get(textureName)->bind(active);
			bindAndDraw(primitive, instances);
		}
};

#endif