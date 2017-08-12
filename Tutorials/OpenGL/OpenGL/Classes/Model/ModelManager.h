#pragma once

#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include <unordered_map>
#include "Model.h"

class ModelManager {
private:
	typedef std::unordered_map<std::string, Model*> Map;

	Map map;

protected:
	ModelManager() {}

	~ModelManager() {
		Map::iterator it;
		for (it = map.begin(); it != map.end(); it++) {
			delete it->second;
		}
	}

public:
	static ModelManager* instance() {
		static ModelManager manager;
		return &manager;
	}

	Model* get(std::string name)
		throw(ModelException) {
		Map::iterator it;
		it = map.find(name);
		//map[name]->print();
		if (it == map.end()) {
			throw(ModelException("No model called " + name + " found."));
		}

		return map[name];
	}

	void add(std::string name, Model* model) {
		Map::iterator it;
		it = map.find(name);
		if (it != map.end()) {
			std::cout << "Model already exists, replacing it..." << std::endl;
			delete it->second;
		}
		map[name] = model;
	}
};

#endif