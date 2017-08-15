#pragma once

#ifndef UBOMANAGER_H
#define UBOMANAGER_H

#include <unordered_map>
#include "Ubo.h"

class UboManager {
private:
	typedef std::unordered_map<std::string, Ubo*> Map;

	Map map;

protected:
	UboManager() {}

	~UboManager() {
		Map::iterator it;
		for (it = map.begin(); it != map.end(); it++) {
			delete it->second;
		}
	}

public:
	static UboManager* instance() {
		static UboManager manager;
		return &manager;
	}

	Ubo* get(std::string name) throw(UboException) {
		Map::iterator it;
		it = map.find(name);
		if (it == map.end()) {
			throw(UboException("No Ubo called " + name + " found."));
		}

		return map[name];
	}

	void add(std::string name, Ubo* program) {
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