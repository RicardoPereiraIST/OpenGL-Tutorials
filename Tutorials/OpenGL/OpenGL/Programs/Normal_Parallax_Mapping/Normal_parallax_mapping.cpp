#include "Normal_parallax_mapping.h"

int main() {
	glEnable(GL_DEPTH_TEST);

	setShader();
	setVertices();
	setTextures();
	setPointLights();

	gameLoop();
	return 0;
}

void gameLoop() {
	while (!glfwWindowShouldClose(init.window)) {
		float currentFrame = glfwGetTime();
		init.deltaTime = currentFrame - init.lastFrame;
		init.lastFrame = currentFrame;

		processInput(init.window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawObjects();

		glfwSwapBuffers(init.window);
		glfwPollEvents();
	}
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		CameraManager::instance()->get("camera")->ProcessKeyboard(FORWARD, init.deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		CameraManager::instance()->get("camera")->ProcessKeyboard(BACKWARD, init.deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		CameraManager::instance()->get("camera")->ProcessKeyboard(LEFT, init.deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		CameraManager::instance()->get("camera")->ProcessKeyboard(RIGHT, init.deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !toyKeyPressed) {
		toy = !toy;
		toyKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) {
		toyKeyPressed = false;
	}
}

void setShader() {
	Shader* normal_mapping = new Shader("Shaders/Normal_Parallax_Mapping/normal_parallax_mapping.vs", "Shaders/Normal_Parallax_Mapping/normal_parallax_mapping.fs");
	ShaderManager::instance()->add("normal_mapping", normal_mapping);

	ShaderManager::instance()->get("normal_mapping")->use();
	ShaderManager::instance()->get("normal_mapping")->setUniform("texture_diffuse1", 0);
	ShaderManager::instance()->get("normal_mapping")->setUniform("texture_normal1", 1);
	ShaderManager::instance()->get("normal_mapping")->setUniform("depthMap", 2);
}

void setTextures() {
	TextureLoader *diffuseMap = new TextureLoader();
	diffuseMap->load("Images/walls/bricks2.jpg");
	TextureManager::instance()->add("diffuseMap", diffuseMap);

	TextureLoader *normalMap = new TextureLoader();
	normalMap->load("Images/walls/bricks2_normal.jpg");
	TextureManager::instance()->add("normalMap", normalMap);

	TextureLoader *depthMap = new TextureLoader();
	depthMap->load("Images/walls/bricks2_disp.jpg");
	TextureManager::instance()->add("depthMap", depthMap);

	TextureLoader *diffuseMap2 = new TextureLoader();
	diffuseMap2->load("Images/floor/wood.png");
	TextureManager::instance()->add("diffuseMap2", diffuseMap2);

	TextureLoader *normalMap2 = new TextureLoader();
	normalMap2->load("Images/toy_box/toy_box_normal.png");
	TextureManager::instance()->add("normalMap2", normalMap2);

	TextureLoader *depthMap2 = new TextureLoader();
	depthMap2->load("Images/toy_box/toy_box_disp.png");
	TextureManager::instance()->add("depthMap2", depthMap2);
}

void setVertices() {
	// positions
	glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
	glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
	glm::vec3 pos3(1.0f, -1.0f, 0.0f);
	glm::vec3 pos4(1.0f, 1.0f, 0.0f);
	// texture coordinates
	glm::vec2 uv1(0.0f, 1.0f);
	glm::vec2 uv2(0.0f, 0.0f);
	glm::vec2 uv3(1.0f, 0.0f);
	glm::vec2 uv4(1.0f, 1.0f);
	// normal vector
	glm::vec3 nm(0.0f, 0.0f, 1.0f);

	// calculate tangent/bitangent vectors of both triangles
	glm::vec3 tangent1, bitangent1;
	glm::vec3 tangent2, bitangent2;
	// triangle 1
	// ----------
	glm::vec3 edge1 = pos2 - pos1;
	glm::vec3 edge2 = pos3 - pos1;
	glm::vec2 deltaUV1 = uv2 - uv1;
	glm::vec2 deltaUV2 = uv3 - uv1;

	GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent1 = glm::normalize(tangent1);

	bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent1 = glm::normalize(bitangent1);

	// triangle 2
	// ----------
	edge1 = pos3 - pos1;
	edge2 = pos4 - pos1;
	deltaUV1 = uv3 - uv1;
	deltaUV2 = uv4 - uv1;

	f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent2 = glm::normalize(tangent2);


	bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent2 = glm::normalize(bitangent2);


	std::vector<float> quadVerticesTangent = {
		// positions            // normal         // texcoords  // tangent                          // bitangent
		pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
		pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
		pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

		pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
	};

	VertexBuffers *v1 = new VertexBuffers(quadVerticesTangent, true, true, true, true);
	VertexManager::instance()->add("quad", v1);
}

void drawObjects() {
	glm::mat4 projection = glm::perspective(CameraManager::instance()->get("camera")->Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = CameraManager::instance()->get("camera")->GetViewMatrix();
	ShaderManager::instance()->get("normal_mapping")->use();
	ShaderManager::instance()->get("normal_mapping")->setUniform("projection", projection);
	ShaderManager::instance()->get("normal_mapping")->setUniform("view", view);

	glm::mat4 model;
	ShaderManager::instance()->get("normal_mapping")->setUniform("model", model);
	ShaderManager::instance()->get("normal_mapping")->setUniform("viewPos", CameraManager::instance()->get("camera")->Position);
	ShaderManager::instance()->get("normal_mapping")->setUniform("height_scale", 0.1f);
	ShaderManager::instance()->get("normal_mapping")->setUniform("calculateOwnBit", false);
	ShaderManager::instance()->get("normal_mapping")->setUniform("steep", true);
	TextureManager::instance()->get(toy ? "diffuseMap2" : "diffuseMap")->bind(0);
	TextureManager::instance()->get(toy ? "normalMap2" : "normalMap")->bind(1);
	TextureManager::instance()->get(toy ? "depthMap2" : "depthMap")->bind(2);

	VertexManager::instance()->get("quad")->draw();
}

void setPointLights() {
	Light *pl1 = new Point_Light("normal_mapping", glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.5f, 1.0f, 0.3f), 1.0f, 0.09f, 0.032f, 0);
	LightManager::instance()->add("pointLight1", pl1);
}