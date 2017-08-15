#pragma once

#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <unordered_map>
#include "TextureLoader.h"

class TextureManager {
private:
	typedef std::unordered_map<std::string, TextureLoader*> Map;

	Map map;

protected:
	TextureManager() {}

	~TextureManager() {
		Map::iterator it;
		for (it = map.begin(); it != map.end(); it++) {
			delete it->second;
		}
	}

public:
	static TextureManager* instance() {
		static TextureManager manager;
		return &manager;
	}

	TextureLoader* get(std::string name) throw(TextureException) {
		Map::iterator it;
		it = map.find(name);
		if (it == map.end()) {
			throw(TextureException("No texture called " + name + " found."));
		}

		return map[name];
	}

	void add(std::string name, TextureLoader* program) {
		Map::iterator it;
		it = map.find(name);
		if (it != map.end()) {
			std::cout << "Texture already exists, replacing it..." << std::endl;
			delete it->second;
		}

		map[name] = program;
	}

	void unbind() {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
};

#endif