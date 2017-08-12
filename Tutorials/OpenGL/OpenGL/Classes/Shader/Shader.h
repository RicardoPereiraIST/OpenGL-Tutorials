#pragma once

#include <iostream>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glfw3.h>

#ifndef SHADER_H
#define SHADER_H

namespace ShaderType {
	enum Type {
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER,
		GEOMETRY = GL_GEOMETRY_SHADER
	};
};

class ShaderProgramException : public std::runtime_error {
public:
	ShaderProgramException(const std::string & msg) :
		std::runtime_error(msg) { }
};

class Shader {
private:
	GLuint programID;
	bool linked;
	std::unordered_map<std::string, GLint> uniformIDs;

public:
	Shader() {
		programID = 0;
		linked = false;
	}

	Shader(const char* vertex, const char* fragment, const char* geometry = "") : Shader() {
		compileShaderFromFile(vertex, ShaderType::VERTEX);
		if (geometry != "") {
			compileShaderFromFile(geometry, ShaderType::GEOMETRY);
		}
		compileShaderFromFile(fragment, ShaderType::FRAGMENT);
		link();
	}

	~Shader() {
		GLint numShaders = 0;
		glGetProgramiv(programID, GL_ATTACHED_SHADERS, &numShaders);

		GLuint *shaderNames = new GLuint[numShaders];
		glGetAttachedShaders(programID, numShaders, NULL, shaderNames);

		for (int i = 0; i < numShaders; i++) {
			glDeleteShader(shaderNames[i]);
		}

		glDeleteProgram(programID);
		delete[] shaderNames;
	}

	GLint getUniformLocation(const char* name) {
		std::unordered_map<std::string, GLint>::iterator it;
		it = uniformIDs.find(name);

		if (it == uniformIDs.end()) {
			uniformIDs[name] = glGetUniformLocation(programID, name);
		}

		return uniformIDs[name];
	}

	void compileShaderFromFile(const char* fileName, ShaderType::Type type) throw(ShaderProgramException) {
		std::ifstream inFile(fileName, std::ios::in);
		if (!inFile) {
			std::string message = std::string("Unable to open: ") + fileName;
			throw ShaderProgramException(message);
		}

		std::stringstream code;
		code << inFile.rdbuf();
		inFile.close();

		std::string source = code.str();

		compileShader(source, type, fileName);
	}

	void compileShader(std::string source, ShaderType::Type type, const char* fileName) {
		if (programID <= 0) {
			programID = glCreateProgram();
			if (programID == (GLuint)0) {
				throw ShaderProgramException("Unable to create shader program.");
			}
		}

		GLuint shaderID = glCreateShader(type);

		const char *c_code = source.c_str();
		glShaderSource(shaderID, 1, &c_code, NULL);

		glCompileShader(shaderID);

		int result;

		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
		if (GL_FALSE == result) {
			int length = 0;
			std::string logString;
			glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);

			if (length > 0) {
				char *c_log = new char[length];
				int written = 0;
				glGetShaderInfoLog(shaderID, length, &written, c_log);
				logString = c_log;
				delete[] c_log;
			}

			std::string msg;
			if (fileName) {
				msg = std::string(fileName) + ": shader compilation failed.\n";
			}
			else {
				msg = "Shader compilation failed.\n";
			}
			msg += logString;

			throw ShaderProgramException(msg);
		}
		else {
			glAttachShader(programID, shaderID);
		}
	}

	void bindAttribLocation(GLuint location, const char* name) {
		glBindAttribLocation(programID, location, name);
	}

	void bindFragDataLocation(GLuint location, const char* name) {
		glBindFragDataLocation(programID, location, name);
	}

	int getProgramID() {
		return programID;
	}

	bool isLinked() {
		return linked;
	}

	void link() throw(ShaderProgramException) {
		if (linked)
			return;

		if (programID <= 0) {
			throw ShaderProgramException("Program has not been compiled.");
		}

		glLinkProgram(programID);

		int status = 0;
		glGetProgramiv(programID, GL_LINK_STATUS, &status);
		if (status == GL_FALSE) {
			int length = 0;
			std::string logString;

			glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);

			if (length > 0) {
				char *c_log = new char[length];
				int written = 0;
				glGetProgramInfoLog(programID, length, &written, c_log);
				logString = c_log;
				delete[] c_log;
			}

			throw ShaderProgramException(std::string("Program link failed:\n") + logString);
		}
		else {
			linked = true;
		}
	}

	void use() throw(ShaderProgramException) {
		if (programID <= 0 || !linked) {
			throw ShaderProgramException("Shader has not been linked");
		}
		glUseProgram(programID);
	}

	void setUniform(const char* name, float x) {
		GLint uniformID = getUniformLocation(name);
		glUniform1f(uniformID, (GLfloat)x);
	}

	void setUniform(const char* name, bool b) {
		GLint uniformID = getUniformLocation(name);
		glUniform1i(uniformID, (int)b);
	}

	void setUniform(const char* name, int i) {
		GLint uniformID = getUniformLocation(name);
		glUniform1i(uniformID, i);
	}

	void setUniform(const char* name, unsigned int i) {
		GLint uniformID = getUniformLocation(name);
		glUniform1i(uniformID, i);
	}

	void setUniform(const char* name, float x, float y, float z, float w) {
		GLint uniformID = getUniformLocation(name);
		glUniform4f(uniformID, (GLfloat)x, (GLfloat)y, (GLfloat)z, (GLfloat)w);
	}

	void setUniform(const char* name, float x, float y, float z) {
		GLint uniformID = getUniformLocation(name);
		glUniform3f(uniformID, (GLfloat)x, (GLfloat)y, (GLfloat)z);
	}

	void setUniform(const char* name, glm::vec2 v) {
		GLint uniformID = getUniformLocation(name);
		glUniform2fv(uniformID, 1, glm::value_ptr(v));
	}

	void setUniform(const char* name, glm::vec3 v) {
		GLint uniformID = getUniformLocation(name);
		glUniform3fv(uniformID, 1, glm::value_ptr(v));
	}

	void setUniform(const char* name, GLfloat* m) {
		GLint uniformID = getUniformLocation(name);
		glUniformMatrix4fv(uniformID, 1, GL_FALSE, m);
	}

	void setUniform(const char* name, glm::mat4 m) {
		GLint uniformID = getUniformLocation(name);
		glUniformMatrix4fv(uniformID, 1, GL_FALSE, glm::value_ptr(m));
	}

	void setUniformTexture(const char* name, int n) {
		GLint uniformID = getUniformLocation(name);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, n);
		glUniform1i(uniformID, GL_TEXTURE0);
	}

	void addUniformBlock(const char* name, GLuint location) {
		GLint uboId = glGetUniformBlockIndex(programID, name);
		glUniformBlockBinding(programID, uboId, location);
	}
};

#endif