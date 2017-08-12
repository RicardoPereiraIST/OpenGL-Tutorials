#pragma once

#ifndef MODEL_H
#define MODEL_H

#include <glad\glad.h>
#include <stdio.h>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Mesh.h"
#include "..\Shader\ShaderManager.h"
#include "..\Texture\TextureManager.h"
#include <boost\filesystem.hpp>

class ModelException : public std::runtime_error {
public:
	ModelException(const std::string & msg) :
		std::runtime_error(msg) { }
};

class Model {
	private:
		/*  Model Data  */
		std::vector<Mesh> meshes;
		std::vector<Texture> textures_loaded;
		std::string directory;
		bool gammaCorrection;
		unsigned int buffer;

		/*  Functions   */
		void loadModel(std::string const &path) throw(ModelException){
			Assimp::Importer importer;
			const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
				return;
			}

			processNode(scene->mRootNode, scene);
		}

		void processNode(aiNode *node, const aiScene *scene) {
			// process each mesh located at the current node
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				// the node object only contains indices to index the actual objects in the scene. 
				// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				meshes.push_back(processMesh(mesh, scene));
			}
			// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				processNode(node->mChildren[i], scene);
			}
		}

		Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
			// data to fill
			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;
			std::vector<Texture> textures;

			// Walk through each of the mesh's vertices
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				Vertex vertex;
				glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
								  // positions
				vector.x = mesh->mVertices[i].x;
				vector.y = mesh->mVertices[i].y;
				vector.z = mesh->mVertices[i].z;
				vertex.Position = vector;
				// normals
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.Normal = vector;
				// texture coordinates
				if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
				{
					glm::vec2 vec;
					// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
					// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
					vec.x = mesh->mTextureCoords[0][i].x;
					vec.y = mesh->mTextureCoords[0][i].y;
					vertex.TexCoords = vec;
				}
				else
					vertex.TexCoords = glm::vec2(0.0f, 0.0f);

				// tangent
				if (mesh->mTangents) {
					vector.x = mesh->mTangents[i].x;
					vector.y = mesh->mTangents[i].y;
					vector.z = mesh->mTangents[i].z;
					vertex.Tangent = vector;
				}
				if (mesh->mBitangents) {
					// bitangent
					vector.x = mesh->mBitangents[i].x;
					vector.y = mesh->mBitangents[i].y;
					vector.z = mesh->mBitangents[i].z;
					vertex.Bitangent = vector;
				}
				vertices.push_back(vertex);

			}
			// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				// retrieve all indices of the face and store them in the indices vector
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}

			// process materials
			if (mesh->mMaterialIndex >= 0)
			{
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
				// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
				// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
				// Same applies to other texture as the following list summarizes:
				// diffuse: texture_diffuseN
				// specular: texture_specularN
				// normal: texture_normalN

				// 1. diffuse maps
				std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
				textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
				// 2. specular maps
				std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
				textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
				// 3. normal maps
				std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
				textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
				// 4. height maps
				std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
				textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

				// Reflection maps
				std::vector<Texture> reflectionMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_reflection");
				textures.insert(textures.end(), reflectionMaps.begin(), reflectionMaps.end());
			}

			// return a mesh object created from the extracted mesh data
			return Mesh(vertices, indices, textures);
		}

		std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
			std::vector<Texture> textures;
			for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
			{
				aiString str;
				mat->GetTexture(type, i, &str);
				// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
				bool skip = false;
				for (unsigned int j = 0; j < textures_loaded.size(); j++)
				{
					if (std::strcmp(textures_loaded[j].path.C_Str(), str.C_Str()) == 0)
					{
						textures.push_back(textures_loaded[j]);
						skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
						break;
					}
				}
				if (!skip)
				{   // if texture hasn't been loaded already, load it
					Texture texture;
					TextureLoader *texture1 = new TextureLoader();
					texture.id = texture1->loadWithDir(str.C_Str(), this->directory);
					TextureManager::instance()->add(str.C_Str(), texture1);
					texture.type = typeName;
					texture.path = str;
					textures.push_back(texture);
					textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
				}
			}
			return textures;
		}

	public:
		/*  Functions   */
		Model(){}
		Model(std::string const &path, bool boost = false, bool gamma = false) : gammaCorrection(gamma)
		{
			if (!boost) {
				directory = path.substr(0, path.find_last_of('/'));
				loadModel(path);
			}
			else {
				boost::filesystem::path p(path);
				boost::filesystem::path full_p = boost::filesystem::complete(p); // complete == absolute
				directory = full_p.parent_path().string();
				loadModel(full_p.string());
			}
		}

		void addInstances(int attrib, int sizeof_instances, glm::mat4 *modelMatrices) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof_instances, &modelMatrices[0], GL_STATIC_DRAW);

			for (unsigned int i = 0; i < meshes.size(); i++)
			{
				meshes[i].setInstances(attrib);
			}
		}

		void Draw(std::string n, bool inst = false, int amount = 0) {
			for (unsigned int i = 0; i < meshes.size(); i++)
				meshes[i].Draw(n, inst, amount);
		}
};

#endif !MODEL_H