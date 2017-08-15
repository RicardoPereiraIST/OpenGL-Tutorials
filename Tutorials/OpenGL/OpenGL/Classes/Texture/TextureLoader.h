#pragma once

#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <glad\glad.h>
#include <iostream>
#include <vector>
#include <string>
#include <glm\glm.hpp>

class TextureException : public std::runtime_error {
public:
	TextureException(const std::string & msg) :
		std::runtime_error(msg) { }
};

class TextureLoader {
	private:
		friend class FrameBuffer;

		unsigned int textureID;
		unsigned int *bufferIDs;
		unsigned int n_textures = 1;

		unsigned int texturesAdded = 0;
		bool initialized = false;

		void loadTextures(std::string filename, bool flip = false, GLenum wrap = GL_REPEAT, bool gamma = false, GLenum min_mag = GL_LINEAR, GLenum map = GL_LINEAR_MIPMAP_LINEAR);

		void initializeTextures(bool cube);

		void setMinMag(GLenum min_mag, bool cube = false, bool mipmap = false, GLenum map = GL_LINEAR_MIPMAP_LINEAR);

		void setSTR(GLenum s_t_r, bool r = false, bool cube = false);

		void setTexParameters(GLenum min_mag, GLenum s_t_r, bool cube = false, bool mipmap = false, bool r = false, GLenum map = GL_LINEAR_MIPMAP_LINEAR);

		unsigned int getID();

		unsigned int *getBufferIDs();

		unsigned int getSize();

	public:
		TextureLoader(int size = 1, bool cube = false);
		~TextureLoader();

		void loadHDR(std::string filename, bool flip = true, GLenum min_mag = GL_LINEAR, GLenum s_t = GL_CLAMP_TO_EDGE);

		void load(std::string file, bool gamma = false, GLenum wrap = GL_REPEAT, bool flip = false, GLenum min_mag = GL_LINEAR, GLenum map = GL_LINEAR_MIPMAP_LINEAR) throw(TextureException);

		void createTexture(int width, int height, bool floating = true, bool alpha = false, GLenum format = GL_FLOAT, GLenum min_mag = GL_LINEAR, GLenum s_t = GL_CLAMP_TO_EDGE);

		void addTexture(bool single, int width, int height, GLenum floating = GL_RGB16F, GLenum alpha = GL_RGB, GLenum format = GL_FLOAT, GLenum min_mag = GL_NEAREST, GLenum s_t = GL_CLAMP_TO_EDGE);

		void addTextureWithVector(bool single, int width, int height, std::vector<glm::vec3> vector, GLenum floating = GL_RGB16F, GLenum alpha = GL_RGB, GLenum format = GL_FLOAT, GLenum min_mag = GL_NEAREST, GLenum s_t = GL_REPEAT);

		void addTextureWithString(bool single, int width, int height, unsigned char * vector, GLenum floating = GL_RGB16F, GLenum alpha = GL_RGB, GLenum format = GL_FLOAT, GLenum min_mag = GL_NEAREST, GLenum s_t = GL_REPEAT);

		void loadFrame(int width, int height, GLenum internalFormat = GL_RGB, GLenum format = GL_RGB, GLenum type = GL_UNSIGNED_BYTE, GLenum min_mag = GL_LINEAR);

		void loadShadowMap(int width, int height, GLenum min_mag = GL_NEAREST, GLenum s_t = GL_CLAMP_TO_BORDER);

		void loadCubeMap(std::vector<std::string> faces, GLenum min_mag = GL_LINEAR, GLenum s_t_r = GL_CLAMP_TO_EDGE);

		void loadShadowCube(int width, int height, GLenum min_mag = GL_NEAREST, GLenum s_t_r = GL_CLAMP_TO_EDGE);

		void setCubeMap(int width, int height, bool mipmap = false, bool generateMipMap = false, GLenum min_mag = GL_LINEAR, GLenum s_t_r = GL_CLAMP_TO_EDGE, GLenum map = GL_LINEAR_MIPMAP_LINEAR);

		unsigned int loadWithDir(const char *path, const std::string &directory, bool gamma = false) throw(TextureException);

		void bind(int n, int index = -1, bool cube = false);

};

#endif