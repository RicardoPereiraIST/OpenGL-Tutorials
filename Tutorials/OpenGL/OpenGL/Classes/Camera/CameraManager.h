#pragma once

#include <unordered_map>
#include "Camera.h"
#include <iostream>

#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

class CameraManager {
private:
	typedef std::unordered_map<std::string, Camera*> Map;

	Map map;

protected:
	CameraManager() {}

	~CameraManager() {
		Map::iterator it;
		for (it = map.begin(); it != map.end(); it++) {
			delete it->second;
		}
	}

public:
	static CameraManager* instance() {
		static CameraManager manager;
		return &manager;
	}

	Camera* get(std::string name) throw(CameraException) {
		Map::iterator it;
		it = map.find(name);
		if (it == map.end()) {
			throw(CameraException("No camera called " + name + " found."));
		}

		return map[name];
	}

	void add(std::string name, Camera* program) {
		Map::iterator it;
		it = map.find(name);
		if (it != map.end()) {
			std::cout << "Camera already exists, replacing it..." << std::endl;
			delete it->second;
		}

		map[name] = program;
	}
};

#endif