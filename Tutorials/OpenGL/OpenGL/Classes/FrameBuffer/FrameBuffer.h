#pragma once

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <glad\glad.h>
#include "..\Texture\TextureManager.h"

class FrameBufferException : public std::runtime_error {
public:
	FrameBufferException(const std::string & msg) :
		std::runtime_error(msg) { }
};

class FrameBuffer {
	private:
		friend class FrameBufferManager;

		unsigned int fbo, rbo;
		unsigned int *fboBuffers;
		unsigned int msfbo;
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

		void createMultisampledTexture(int width, int height, int samples, bool stencil, bool color) {
			// create a multisampled color attachment texture
			glGenTextures(1, &msfbo);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msfbo);

			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
			attachTexture2D(msfbo, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE);

			createMultisampledRBO(width, height, samples, stencil, color);

			checkError();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void createMultisampledFrame(std::string textureName, int width, int height, int samples, bool stencil, bool color) {
			glGenFramebuffers(1, &msfbo);
			glGenFramebuffers(1, &fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, msfbo);
			createMultisampledRBO(width, height, samples, stencil, color);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureManager::instance()->get(textureName)->getID(), 0);

			checkError();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		static bool checkBound(unsigned int buffer, GLenum option){
			GLenum binding = GL_FRAMEBUFFER_BINDING;
			if (option == GL_READ_FRAMEBUFFER)
				binding = GL_READ_FRAMEBUFFER_BINDING;
			else if (option == GL_DRAW_FRAMEBUFFER)
				binding = GL_DRAW_FRAMEBUFFER_BINDING;
			else if (option == GL_RENDERBUFFER)
				binding = GL_RENDERBUFFER_BINDING;

			int bound;
			glGetIntegerv(binding, &bound);
			return bound == buffer;
		}

		void bindPartial(unsigned int buffer) {
			if (!checkBound(fbo, GL_READ_FRAMEBUFFER)) {
				glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer);
			}
			if (!checkBound(fbo, GL_DRAW_FRAMEBUFFER)) {
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buffer);
			}
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

		FrameBuffer(int width, int height, int samples, bool stencil = true, bool color = false) : FrameBuffer() {
			createMultisampledTexture(width, height, samples, stencil, color);
		}

		FrameBuffer(std::string textureName, int width, int height, int samples, bool stencil = true, bool color = false) : FrameBuffer() {
			createMultisampledFrame(textureName, width, height, samples, stencil, color);
		}

		~FrameBuffer() {
			glDeleteFramebuffers(1, &fbo);
			glDeleteFramebuffers(1, &rbo);
			glDeleteFramebuffers(1, &msfbo);
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

			if(!checkBound(rbo, GL_RENDERBUFFER))
				glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, type, width, height);
			if (!checkBound(0, GL_RENDERBUFFER))
				glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo);
		}

		void createMultisampledRBO(int width, int height, int samples, bool stencil = true, bool color = false) {
			glGenRenderbuffers(1, &rbo);

			GLenum type, attachment;
			type = color ? GL_RGB : stencil ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT24;
			attachment = color ? GL_COLOR_ATTACHMENT0 : stencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;

			if (!checkBound(rbo, GL_RENDERBUFFER))
				glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, type, width, height);
			if (!checkBound(0, GL_RENDERBUFFER))
				glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo);
		}

		void setRBO(int width, int height, bool stencil = true) {
			GLenum type, attachment;
			type = stencil ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT24;
			attachment = stencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;

			if (!checkBound(rbo, GL_RENDERBUFFER))
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
			if (!checkBound(fbo, option)) {
				if (option == GL_FRAMEBUFFER) {
					bindPartial(fbo);
					return;
				}
				glBindFramebuffer(option, fbo);
			}
		}

		void bindMultisampled(GLenum option = GL_FRAMEBUFFER) {
			if (!checkBound(msfbo, option)) {
				if (option == GL_FRAMEBUFFER) {
					bindPartial(msfbo);
					return;
				}
				glBindFramebuffer(option, msfbo);
			}
		}

		void bindIndex(int i, GLenum option = GL_FRAMEBUFFER) {
			if (!checkBound(fboBuffers[i], option)) {
				if (option == GL_FRAMEBUFFER) {
					bindPartial(fboBuffers[i]);
					return;
				}
				glBindFramebuffer(option, fboBuffers[i]);
			}
		}
};

#endif