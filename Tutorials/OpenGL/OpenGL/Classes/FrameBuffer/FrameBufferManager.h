#pragma once

#ifndef FRAMEBUFFERMANAGER_H
#define FRAMEBUFFERMANAGER_H

#include <unordered_map>
#include "FrameBuffer.h"

class FrameBufferManager {
private:
	typedef std::unordered_map<std::string, FrameBuffer*> Map;

	Map map;

protected:
	FrameBufferManager() {}

	~FrameBufferManager() {
		Map::iterator it;
		for (it = map.begin(); it != map.end(); it++) {
			delete it->second;
		}
	}

public:
	static FrameBufferManager* instance() {
		static FrameBufferManager manager;
		return &manager;
	}

	FrameBuffer* get(std::string name) throw(FrameBufferException) {
		Map::iterator it;
		it = map.find(name);
		if (it == map.end()) {
			throw(FrameBufferException("No framebuffer called " + name + " found."));
		}

		return map[name];
	}

	void add(std::string name, FrameBuffer* program) {
		Map::iterator it;
		it = map.find(name);
		if (it != map.end()) {
			std::cout << "FrameBuffer already exists, replacing it..." << std::endl;
			delete it->second;
		}

		map[name] = program;
	}

	void blit(std::string read, std::string write, int width, int height, GLenum type = GL_COLOR_BUFFER_BIT) {
		FrameBufferManager::instance()->get(read)->bind(GL_READ_FRAMEBUFFER);
		FrameBufferManager::instance()->get(write)->bind(GL_DRAW_FRAMEBUFFER);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, type, GL_NEAREST);
		unbind();
	}

	void blitToDefault(std::string read, int width, int height, GLenum type = GL_COLOR_BUFFER_BIT) {
		FrameBufferManager::instance()->get(read)->bind(GL_READ_FRAMEBUFFER);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, type, GL_NEAREST);
		unbind();
	}

	void unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};

#endif