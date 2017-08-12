#pragma once

#include <unordered_map>
#include "TextRenderer.h"

#ifndef TEXTRENDERERMANAGER_H
#define TEXTRENDERERMANAGER_H

class TextRendererManager {
private:
	typedef std::unordered_map<std::string, TextRenderer*> Map;

	Map map;

protected:
	TextRendererManager() {}

	~TextRendererManager() {
		Map::iterator it;
		for (it = map.begin(); it != map.end(); it++) {
			delete it->second;
		}
	}

public:
	static TextRendererManager* instance() {
		static TextRendererManager manager;
		return &manager;
	}

	TextRenderer* get(std::string name) throw(TextRendererException) {
		Map::iterator it;
		it = map.find(name);
		if (it == map.end()) {
			throw(TextRendererException("No Ubo called " + name + " found."));
		}

		return map[name];
	}

	void add(std::string name, TextRenderer* program) {
		Map::iterator it;
		it = map.find(name);
		if (it != map.end()) {
			std::cout << "Ubo already exists, replacing it..." << std::endl;
			delete it->second;
		}

		map[name] = program;
	}
};

#endif