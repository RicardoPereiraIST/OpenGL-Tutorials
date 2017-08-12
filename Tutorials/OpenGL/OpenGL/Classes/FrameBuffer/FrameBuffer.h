#pragma once

#include <glad\glad.h>
#include <glfw3.h>
#include <iostream>
#include "..\Texture\TextureManager.h"

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

class FrameBufferException : public std::runtime_error {
public:
	FrameBufferException(const std::string & msg) :
		std::runtime_error(msg) { }
};

class FrameBuffer {
	private:
		unsigned int fbo, rbo;
		unsigned int *fboBuffers;
		unsigned int textureColorBufferMultiSampled;
		unsigned int n_buffers = 1;

		bool initialized = false;

		void checkError() {
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				throw new FrameBufferException("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
			}
		}

		void createFBO() {
			if (!initialized) {
				if (n_buffers > 1) {
					fboBuffers = new unsigned int[n_buffers];
					glGenFramebuffers(n_buffers, fboBuffers);
				}
				else {
					glGenFramebuffers(1, &fbo);
					glBindFramebuffer(GL_FRAMEBUFFER, fbo);
				}
				initialized = true;
			}

		}

		void createTexture(std::string textureName, int width, int height, bool depth_stencil) {
			if (!depth_stencil) {
				glTexImage2D(
					GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0,
					GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
				);

				attachTexture2D(textureName, GL_DEPTH_STENCIL_ATTACHMENT);
			}
			else {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
				attachTexture2D(textureName);
			}
		}

		void createShadowMap(std::string textureName, bool cube = false) {
			if (cube)
				attachTexture(textureName);
			else
				attachTexture2D(textureName, GL_DEPTH_ATTACHMENT);
				
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);

			checkError();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void create(std::string textureName, int width, int height, bool createRbo = true, bool rboStencil = true, bool texture = false, bool depth_stencil_texture = false) {
			//IF WE WANT DEPTH AND STENCIL AS TEXTURE
			if (texture) {
				createTexture(textureName, width, height, depth_stencil_texture);
			}
			else {
				attachTexture2D(textureName);
			}
			if(createRbo) {
				createRBO(width, height, rboStencil);
			}

			checkError();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void createMultisampled(int width, int height, int samples) {
			// create a multisampled color attachment texture
			glGenTextures(1, &textureColorBufferMultiSampled);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);

			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
			attachTexture2D(textureColorBufferMultiSampled, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE);

			createMultisampledRBO(width, height, samples);

			checkError();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

	public:
		FrameBuffer(int frameSize = 1) {
			n_buffers = frameSize;
			createFBO();
		}

		FrameBuffer(std::string textureName, int width, int height, bool createRbo = true, bool rboStencil = true, bool texture = false, bool depth_stencil_tex = false) : FrameBuffer(){
			create(textureName, width, height, createRbo, rboStencil, texture, depth_stencil_tex);
		}

		FrameBuffer(std::string textureName, bool cube = false) : FrameBuffer() {
			createShadowMap(textureName, cube);
		}

		FrameBuffer(int width, int height, int samples) : FrameBuffer() {
			createMultisampled(width, height, samples);
		}

		~FrameBuffer() {
			glDeleteFramebuffers(1, &fbo);
			glDeleteFramebuffers(1, &rbo);
			glDeleteFramebuffers(1, &textureColorBufferMultiSampled);
			if (fboBuffers) {
				glDeleteFramebuffers(n_buffers, fboBuffers);
				delete fboBuffers;
			}
		}

		void createBuffers(std::string textureName, int width, int height, bool createRbo = true, bool rboStencil = true) {
			unsigned int * b = TextureManager::instance()->get(textureName)->getBufferIDs();
			int size = TextureManager::instance()->get(textureName)->getSize();
			for (int i = 0; i < size; i++) {
				attachTexture2D(b[i], GL_COLOR_ATTACHMENT0 + i);
			}

			if (createRbo) {
				createRBO(width, height, rboStencil);
			}

			unsigned int *attachments = new unsigned int[size];
			for (int i = 0; i < size; i++)
				attachments[i] = GL_COLOR_ATTACHMENT0 + i;
			glDrawBuffers(size, attachments);

			delete[] attachments;

			checkError();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void createMultipleBuffers(std::string textureName, int width, int height, bool createRbo = true, bool rboStencil = true) {
			unsigned int * b = TextureManager::instance()->get(textureName)->getBufferIDs();
			int size = TextureManager::instance()->get(textureName)->getSize();
			for (int i = 0; i < size; i++) {
				glBindFramebuffer(GL_FRAMEBUFFER, fboBuffers[i]);
				attachTexture2D(b[i]);
				checkError();
			}

			if (createRbo) {
				createRBO(width, height, rboStencil);
			}

			checkError();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void createRBO(int width, int height, bool stencil = true) {
			glGenRenderbuffers(1, &rbo);

			GLenum type, attachment;
			type = stencil ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT24;
			attachment = stencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;

			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, type, width, height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo);
		}

		void createMultisampledRBO(int width, int height, int samples, bool stencil = true) {
			glGenRenderbuffers(1, &rbo);

			GLenum type, attachment;
			type = stencil ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT24;
			attachment = stencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;

			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo);
		}

		void setRBO(int width, int height, bool stencil = true) {
			GLenum type, attachment;
			type = stencil ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT24;
			attachment = stencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;

			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, type, width, height);
		}

		void attachTexture2D(std::string textureName, GLenum attach = GL_COLOR_ATTACHMENT0, GLenum target = GL_TEXTURE_2D, int level = 0) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, attach, target, TextureManager::instance()->get(textureName)->getID(), level);
		}

		void attachTexture(std::string textureName, GLenum attach = GL_DEPTH_ATTACHMENT) {
			glFramebufferTexture(GL_FRAMEBUFFER, attach, TextureManager::instance()->get(textureName)->getID(), 0);
		}

		void attachTexture2D(unsigned int textureID, GLenum attach = GL_COLOR_ATTACHMENT0, GLenum target = GL_TEXTURE_2D, int level = 0) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, attach, target, textureID, level);
		}

		void bind(GLenum option = GL_FRAMEBUFFER) {
			glBindFramebuffer(option, fbo);
		}

		void bindIndex(int i, GLenum option = GL_FRAMEBUFFER) {
			glBindFramebuffer(option, fboBuffers[i]);
		}
};

#endif