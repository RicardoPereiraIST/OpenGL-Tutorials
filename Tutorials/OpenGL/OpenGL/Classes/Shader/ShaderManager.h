#pragma once

#include <unordered_map>
#include "Shader.h"

#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

class ShaderManager {
	private:
		typedef std::unordered_map<std::string, Shader*> Map;

		Map map;

	protected:
		ShaderManager() {}

		~ShaderManager() {
			Map::iterator it;
			for (it = map.begin(); it != map.end(); it++) {
				delete it->second;
			}
		}

	public:
		static ShaderManager* instance() {
			static ShaderManager manager;
			return &manager;
		}

		Shader* get(std::string name) throw(ShaderProgramException) {
			Map::iterator it;
			it = map.find(name);
			if (it == map.end()) {
				throw(ShaderProgramException("No ShaderProgram called " + name + " found."));
			}

			return map[name];
		}

		void add(std::string name, Shader* program) {
			Map::iterator it;
			it = map.find(name);
			if (it != map.end()) {
				std::cout << "ShaderProgram already exists, replacing it..." << std::endl;
				delete it->second;
			}

			map[name] = program;
		}
};

#endif