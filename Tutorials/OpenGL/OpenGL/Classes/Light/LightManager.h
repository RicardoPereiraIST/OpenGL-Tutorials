#pragma once

#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <unordered_map>
#include "Light.h"
#include <iostream>	

class LightException : public std::runtime_error {
public:
	LightException(const std::string & msg) :
		std::runtime_error(msg) { }
};

class LightManager {
private:
	typedef std::unordered_map<std::string, Light*> Map;

	Map map;

protected:
	LightManager() {}

	~LightManager() {
		Map::iterator it;
		for (it = map.begin(); it != map.end(); it++) {
			delete it->second;
		}
	}

public:
	static LightManager* instance() {
		static LightManager manager;
		return &manager;
	}

	Light* get(std::string name)
		throw(LightException) {
		Map::iterator it;
		it = map.find(name);
		//map[name]->print();
		if (it == map.end()) {
			throw(LightException("No light called " + name + " found."));
		}

		return map[name];
	}

	void add(std::string name, Light* model) {
		Map::iterator it;
		it = map.find(name);
		if (it != map.end()) {
			delete it->second;
		}
		map[name] = model;
	}
};

#endif