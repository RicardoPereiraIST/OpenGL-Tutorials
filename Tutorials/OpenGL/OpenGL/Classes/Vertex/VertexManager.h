#pragma once

#include <unordered_map>
#include "VertexBuffers.h"

#ifndef VERTEXMANAGER_H
#define VERTEXMANAGER_H

class VertexManager {
private:
	typedef std::unordered_map<std::string, VertexBuffers*> Map;

	Map map;

protected:
	VertexManager() {}

	~VertexManager() {
		Map::iterator it;
		for (it = map.begin(); it != map.end(); it++) {
			delete it->second;
		}
	}

public:
	static VertexManager* instance() {
		static VertexManager manager;
		return &manager;
	}

	VertexBuffers* get(std::string name) throw(VertexBuffersException) {
		Map::iterator it;
		it = map.find(name);
		if (it == map.end()) {
			throw(VertexBuffersException("No Vertex called " + name + " found."));
		}

		return map[name];
	}

	void add(std::string name, VertexBuffers* program) {
		Map::iterator it;
		it = map.find(name);
		if (it != map.end()) {
			std::cout << "Vertex already exists, replacing it..." << std::endl;
			delete it->second;
		}

		map[name] = program;
	}
};

#endif