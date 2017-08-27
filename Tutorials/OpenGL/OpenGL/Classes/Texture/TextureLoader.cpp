#pragma once

#include "TextureLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "..\..\Dependencies\stb_image.h"


void TextureLoader::loadTextures(std::string filename, bool flip, GLenum wrap, bool gamma, GLenum min_mag, GLenum map) {
	int texW, texH, twxNrChannels;

	stbi_set_flip_vertically_on_load(flip);

	unsigned char *data = stbi_load(filename.c_str(), &texW, &texH, &twxNrChannels, 0);

	if (data) {
		GLenum internalFormat, format;
		if (twxNrChannels == 1)
			internalFormat = format = GL_RED;
		else if (twxNrChannels == 3) {
			internalFormat = gamma ? GL_SRGB : GL_RGB;
			format = GL_RGB;
		}
		else if (twxNrChannels == 4) {
			internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
			format = GL_RGBA;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, texW, texH, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		setTexParameters(min_mag, wrap, false, true, map);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
		throw TextureException("Failed to load data");
	}

	stbi_image_free(data);
}

void TextureLoader::initializeTextures(bool cube) {

	if (!initialized) {
		if (n_textures > 1) {
			bufferIDs = new unsigned int[n_textures];
			glGenTextures(n_textures, bufferIDs);
		}
		else {
			glGenTextures(1, &textureID);
			GLenum format = cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
			glBindTexture(format, textureID);
		}
		initialized = true;
	}
}

void TextureLoader::setMinMag(GLenum min_mag, bool cube, bool mipmap, GLenum map) {
	GLenum tex = cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;

	if (mipmap)
		glTexParameteri(tex, GL_TEXTURE_MIN_FILTER, map);
	else
		glTexParameteri(tex, GL_TEXTURE_MIN_FILTER, min_mag);
	glTexParameteri(tex, GL_TEXTURE_MAG_FILTER, min_mag);
}

void TextureLoader::setSTR(GLenum s_t_r, bool r, bool cube) {
	GLenum tex = cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;

	glTexParameteri(tex, GL_TEXTURE_WRAP_S, s_t_r);
	glTexParameteri(tex, GL_TEXTURE_WRAP_T, s_t_r);
	if (r)
		glTexParameteri(tex, GL_TEXTURE_WRAP_R, s_t_r);
}

void TextureLoader::setTexParameters(GLenum min_mag, GLenum s_t_r, bool cube, bool mipmap, bool r, GLenum map) {
	setSTR(s_t_r, r, cube);
	setMinMag(min_mag, cube, mipmap, map);
}

unsigned int TextureLoader::getID() {
	return textureID;
}

unsigned int *TextureLoader::getBufferIDs() {
	return bufferIDs;
}

unsigned int TextureLoader::getSize() {
	return n_textures;
}

TextureLoader::TextureLoader(int size, bool cube) {
	n_textures = size;
	initializeTextures(cube);
}
TextureLoader::~TextureLoader() {
	glDeleteTextures(1, &textureID);
	if (n_textures > 1) {
		glDeleteTextures(n_textures, bufferIDs);
		delete bufferIDs;
	}
}

void TextureLoader::loadHDR(std::string filename, bool flip, GLenum min_mag, GLenum s_t) {
	stbi_set_flip_vertically_on_load(flip);
	int width, height, nrComponents;
	float *data = stbi_loadf(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

		setTexParameters(min_mag, s_t, false, false, false);

		stbi_image_free(data);
	}
	else
	{
		throw TextureException("Failed to load HDR image.");
	}
}

void TextureLoader::load(std::string file, bool gamma, GLenum wrap, bool flip, GLenum min_mag, GLenum map) throw(TextureException) {
	loadTextures(file, flip, wrap, gamma, min_mag, map);
}

void TextureLoader::createTexture(int width, int height, bool floating, bool alpha, GLenum format, GLenum min_mag, GLenum s_t) {
	GLenum type;
	if (floating) {
		if (alpha)
			type = GL_RGBA16F;
		else
			type = GL_RGB16F;
	}
	else {
		if (alpha)
			type = GL_RGBA;
		else
			type = GL_RGB;
	}

	if (n_textures == 1) {
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, alpha ? GL_RGBA : GL_RGB, format, NULL);
		setTexParameters(min_mag, s_t, false, false, false);
	}
	else {
		for (int i = 0; i < n_textures; i++) {
			glBindTexture(GL_TEXTURE_2D, bufferIDs[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, alpha ? GL_RGBA : GL_RGB, format, NULL);

			setTexParameters(min_mag, s_t, false, false, false);
		}
	}
}

void TextureLoader::addTexture(bool single, int width, int height, GLenum floating, GLenum alpha, GLenum format, GLenum min_mag, GLenum s_t) {
	if (single) {
		glTexImage2D(GL_TEXTURE_2D, 0, floating, width, height, 0, alpha, format, NULL);
		setTexParameters(min_mag, s_t, false, false, false);
	}
	else {
		if (texturesAdded < n_textures) {
			glBindTexture(GL_TEXTURE_2D, bufferIDs[texturesAdded]);
			glTexImage2D(GL_TEXTURE_2D, 0, floating, width, height, 0, alpha, format, NULL);
			setTexParameters(min_mag, s_t, false, false, false);
			texturesAdded++;
		}
	}
}

void TextureLoader::addTextureWithVector(bool single, int width, int height, std::vector<glm::vec3> vector, GLenum floating, GLenum alpha, GLenum format, GLenum min_mag, GLenum s_t) {
	if (single) {
		glTexImage2D(GL_TEXTURE_2D, 0, floating, width, height, 0, alpha, format, &vector);
		setTexParameters(min_mag, s_t, false, false, false);
	}
	else {
		if (texturesAdded < n_textures) {
			glBindTexture(GL_TEXTURE_2D, bufferIDs[texturesAdded]);
			glTexImage2D(GL_TEXTURE_2D, 0, floating, width, height, 0, alpha, format, &vector);
			setTexParameters(min_mag, s_t, false, false, false);
			texturesAdded++;
		}
	}
}

void TextureLoader::addTextureWithString(bool single, int width, int height, unsigned char * vector, GLenum floating, GLenum alpha, GLenum format, GLenum min_mag, GLenum s_t) {
	if (single) {
		glTexImage2D(GL_TEXTURE_2D, 0, floating, width, height, 0, alpha, format, vector);
		setTexParameters(min_mag, s_t, false, false, false);
	}
	else {
		if (texturesAdded < n_textures) {
			glBindTexture(GL_TEXTURE_2D, bufferIDs[texturesAdded]);
			glTexImage2D(GL_TEXTURE_2D, 0, floating, width, height, 0, alpha, format, vector);
			setTexParameters(min_mag, s_t, false, false, false);
			texturesAdded++;
		}
	}
}

void TextureLoader::loadFrame(int width, int height, GLenum internalFormat, GLenum format, GLenum type, GLenum min_mag) {
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);

	setMinMag(min_mag);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureLoader::loadShadowMap(int width, int height, GLenum min_mag, GLenum s_t) {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	setTexParameters(min_mag, s_t, false, false, false);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureLoader::loadCubeMap(std::vector<std::string> faces, GLenum min_mag, GLenum s_t_r) {
	int width, height, nrChannels;
	unsigned char *data;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
		}
		stbi_image_free(data);
	}

	setTexParameters(min_mag, s_t_r, true, false, true);
}

void TextureLoader::loadShadowCube(int width, int height, GLenum min_mag, GLenum s_t_r) {
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	setTexParameters(min_mag, s_t_r, true, false, true);
}

void TextureLoader::setCubeMap(int width, int height, bool mipmap, bool generateMipMap, GLenum min_mag, GLenum s_t_r, GLenum map) {
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	}

	setTexParameters(min_mag, s_t_r, true, mipmap, true, map);

	if (generateMipMap)
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

unsigned int TextureLoader::loadWithDir(const char *path, const std::string &directory, bool gamma) throw(TextureException) {

	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	loadTextures(filename);

	return textureID;
}

void TextureLoader::bind(int n, int index, bool cube) {
	GLenum tex = cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;

	if (n > -1)
		glActiveTexture(GL_TEXTURE0 + n);

	if (index > -1) {
		glBindTexture(tex, bufferIDs[index]);
	}
	else {
		glBindTexture(tex, textureID);
	}
}