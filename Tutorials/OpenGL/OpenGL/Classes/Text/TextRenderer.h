#pragma once

#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include "..\..\Includes\FreeType.h"
#include <iostream>
#include <map>
#include "..\..\Includes\Vertices.h"
#include "..\..\Includes\Textures.h"
#include "..\Shader\ShaderManager.h"

struct Character {
	std::string textureName;
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

class TextRendererException : public std::runtime_error {
public:
	TextRendererException(const std::string & msg) :
		std::runtime_error(msg) { }
};

class TextRenderer {
	private:
		std::map<GLchar, Character> Characters;
		std::string shader;
		std::string vertexName;

		void loadCharacters(std::string pathForFont, int size, int y_size) {
			// FreeType
			FT_Library ft;
			// All functions return a value different than 0 whenever an error occurred
			if (FT_Init_FreeType(&ft))
				throw(TextRendererException("ERROR::FREETYPE: Could not init FreeType Library"));

			// Load font as face
			FT_Face face;
			if (FT_New_Face(ft, pathForFont.c_str(), 0, &face))
				throw(TextRendererException("ERROR::FREETYPE: Failed to load font"));

			// Set size to load glyphs as
			FT_Set_Pixel_Sizes(face, 0, y_size);

			// Disable byte-alignment restriction
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			// Load first 128 characters of ASCII set
			for (GLubyte c = 0; c < size; c++)
			{
				// Load character glyph 
				if (FT_Load_Char(face, c, FT_LOAD_RENDER))
				{
					throw(TextRendererException("ERROR::FREETYTPE: Failed to load Glyph"));
					continue;
				}

				// Generate texture
				TextureLoader *t1 = new TextureLoader();
				t1->addTextureWithString(true, face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer, GL_RED, GL_RED, GL_UNSIGNED_BYTE, GL_LINEAR, GL_CLAMP_TO_EDGE);
				std::string tName = pathForFont + "_" + std::to_string(c);
				TextureManager::instance()->add(tName, t1);

				// Now store character for later use
				Character character = {
					tName,
					glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					face->glyph->advance.x
				};
				Characters.insert(std::pair<GLchar, Character>(c, character));
			}

			TextureManager::instance()->unbind();
			// Destroy FreeType once we're finished
			FT_Done_Face(face);
			FT_Done_FreeType(ft);

			VertexBuffers *v1 = new VertexBuffers();
			v1->createGliphQuad();
			vertexName = pathForFont + "_gliph";
			VertexManager::instance()->add(vertexName, v1);
		}

	public:
		TextRenderer(std::string sh, std::string pathForFont, int size = 128, int y_size = 48) {
			shader = sh;
			loadCharacters(pathForFont, size, y_size);
		}

		void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color = glm::vec3(1.0, 1.0f, 1.0f), bool reverse = false)
		{
			// Activate corresponding render state	
			ShaderManager::instance()->get(shader)->use();
			ShaderManager::instance()->get(shader)->setUniform("textColor", color.x, color.y, color.z);

			// Iterate through all characters
			std::string::const_iterator c;
			for (c = text.begin(); c != text.end(); c++)
			{
				Character ch = Characters[*c];

				GLfloat xpos = x + ch.Bearing.x * scale;
				GLfloat ypos;

				if(reverse)
					ypos = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;
				else
					ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

				GLfloat w = ch.Size.x * scale;
				GLfloat h = ch.Size.y * scale;
				// Update VBO for each character

				if (reverse) {
					GLfloat vertices[6][4] = {
						{ xpos,     ypos + h,   0.0, 1.0 },
						{ xpos + w, ypos,       1.0, 0.0 },
						{ xpos,     ypos,       0.0, 0.0 },

						{ xpos,     ypos + h,   0.0, 1.0 },
						{ xpos + w, ypos + h,   1.0, 1.0 },
						{ xpos + w, ypos,       1.0, 0.0 }
					};

					// Update content of VBO memory
					VertexManager::instance()->get(vertexName)->updateGliphQuad(vertices);
				}
				else {
					GLfloat vertices[6][4] = {
						{ xpos,     ypos + h,   0.0, 0.0 },
						{ xpos,     ypos,       0.0, 1.0 },
						{ xpos + w, ypos,       1.0, 1.0 },

						{ xpos,     ypos + h,   0.0, 0.0 },
						{ xpos + w, ypos,       1.0, 1.0 },
						{ xpos + w, ypos + h,   1.0, 0.0 }
					};

					// Update content of VBO memory
					VertexManager::instance()->get(vertexName)->updateGliphQuad(vertices);
				}

				// Render glyph texture over quad
				TextureManager::instance()->get(ch.textureName)->bind(0);
				// Render quad
				VertexManager::instance()->get(vertexName)->draw();
				// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
				x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
			}
			TextureManager::instance()->unbind();
		}
};

#endif