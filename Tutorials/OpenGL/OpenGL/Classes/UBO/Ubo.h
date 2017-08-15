#pragma once

#ifndef UBO_H
#define UBO_H

#include <iostream>

class UboException : public std::runtime_error {
public:
	UboException(const std::string & msg) :
		std::runtime_error(msg) { }
};

class Ubo {
	private:
		unsigned int ubo;

	public:
		Ubo(unsigned int size, GLenum draw = GL_STATIC_DRAW) {
			glGenBuffers(1, &ubo);
			glBindBuffer(GL_UNIFORM_BUFFER, ubo);
			glBufferData(GL_UNIFORM_BUFFER, size, NULL, draw);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		//only for glm::mat4, for now
		void add(glm::mat4 matrix, unsigned int entry, unsigned int size) {
			glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, size);

			glBindBuffer(GL_UNIFORM_BUFFER, ubo);
			glBufferSubData(GL_UNIFORM_BUFFER, entry, sizeof(glm::mat4), glm::value_ptr(matrix));
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		~Ubo() {
			glDeleteFramebuffers(1, &ubo);
		}

		unsigned int getID() {
			return ubo;
		}
};

#endif