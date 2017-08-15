#pragma once

#include <unordered_map>
#include "SpriteRenderer.h"

#ifndef SPRITERENDERERMANAGER_H
#define SPRITERENDERERMANAGER_H

class SpriteRendererManager {
private:
	typedef std::unordered_map<std::string, SpriteRenderer*> Map;

	Map map;

protected:
	SpriteRendererManager() {}

	~SpriteRendererManager() {
		Map::iterator it;
		for (it = map.begin(); it != map.end(); it++) {
			delete it->second;
		}
	}

public:
	static SpriteRendererManager* instance() {
		static SpriteRendererManager manager;
		return &manager;
	}

	SpriteRenderer* get(std::string name) throw(SpriteRendererException) {
		Map::iterator it;
		it = map.find(name);
		if (it == map.end()) {
			throw(SpriteRendererException("No SpriteRenderer called " + name + " found."));
		}

		return map[name];
	}

	void add(std::string name, SpriteRenderer* program) {
		Map::iterator it;
		it = map.find(name);
		if (it != map.end()) {
			std::cout << "SpriteRenderer already exists, replacing it..." << std::endl;
			delete it->second;
		}

		map[name] = program;
	}
};

#endif