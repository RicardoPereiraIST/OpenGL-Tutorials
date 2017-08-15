#pragma once

#ifndef MULTIPLE_VERTICES_H
#define MULTIPLE_VERTICES_H

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <vector>

// Multiple data that was and can be useful at some point

extern float sharpen[9];

extern float blur[9];

extern float edge_detection[9];

extern std::vector<float> quadVertices;

extern std::vector<float> cubeVerticesFaceCullCCW;

extern std::vector<float> cubeVerticesFaceCullCW;

extern std::vector<float> cubeVertices;

extern std::vector<float> cubeVerticesWithNormals;

extern std::vector<float> cubeWithMaps;

extern std::vector<float> vertices;

extern std::vector<unsigned int> indices;

extern glm::vec3 pointLightPositions[];

extern std::vector<float> planeVertices;

extern std::vector<float> transparentVertices;

extern std::vector<std::string> faces;

extern std::vector<float> skyboxVertices;

extern std::vector<float> verticesWithNormal;

extern std::vector<float> points;

extern std::vector<float> pointsColors;

extern std::vector<float> instancingVertices;

extern std::vector<float> simpleCubeVertices;

extern std::vector<float> planeVerticesBlinnPhong;

extern std::vector<float> shadowVertices;

extern std::vector<float> planeShadowVertices;

extern std::vector<float> hdrVertices;

extern std::vector<float> hdrQuad;

extern std::vector<float> spriteVertices;

#endif // !MULTIPLE_VERTICES_H