#include "Shaders_variables.h"

int main() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_PROGRAM_POINT_SIZE);

	setShader();
	setVertices();
	setUbos();

	gameLoop();
	return 0;
}

void gameLoop() {
	while (!glfwWindowShouldClose(init.window)) {
		float currentFrame = glfwGetTime();
		init.deltaTime = currentFrame - init.lastFrame;
		init.lastFrame = currentFrame;

		processInput(init.window);

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
}

void setShader() {
	Shader* red = new Shader("Shaders/Shaders_variables/pointsize.vs", "Shaders/Shaders_variables/red.fs");
	ShaderManager::instance()->add("red", red);

	Shader* green = new Shader("Shaders/Shaders_variables/pointsize.vs", "Shaders/Shaders_variables/green.fs");
	ShaderManager::instance()->add("green", green);

	Shader* yellow = new Shader("Shaders/Shaders_variables/pointsize.vs", "Shaders/Shaders_variables/yellow.fs");
	ShaderManager::instance()->add("yellow", yellow);

	Shader* blue = new Shader("Shaders/Shaders_variables/pointsize.vs", "Shaders/Shaders_variables/blue.fs");
	ShaderManager::instance()->add("blue", blue);

	ShaderManager::instance()->get("red")->addUniformBlock("Matrices", 0);
	ShaderManager::instance()->get("green")->addUniformBlock("Matrices", 0);
	ShaderManager::instance()->get("blue")->addUniformBlock("Matrices", 0);
	ShaderManager::instance()->get("yellow")->addUniformBlock("Matrices", 0);
}

void setVertices() {
	VertexBuffers *vertex;
	vertex = new VertexBuffers(cubeVerticesFaceCullCCW, false, true);
	VertexManager::instance()->add("cube", vertex);
}


void drawObjects() {
	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

							  //make sure we clear the framebuffer's content
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glm::mat4 view = CameraManager::instance()->get("camera")->GetViewMatrix();
	UboManager::instance()->get("uboMatrices")->add(view, sizeof(glm::mat4), sizeof(glm::mat4));

	ShaderManager::instance()->get("red")->use();

	glm::mat4 model;
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-1.0f, -1.0f, -1.0f));
	ShaderManager::instance()->get("red")->setUniform("model", model);
	VertexManager::instance()->get("cube")->draw();

	ShaderManager::instance()->get("green")->use();
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(1.0f, -1.0f, -1.0f));
	ShaderManager::instance()->get("green")->setUniform("model", model);
	VertexManager::instance()->get("cube")->draw();

	ShaderManager::instance()->get("blue")->use();
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-1.0f, 1.0f, -1.0f));
	ShaderManager::instance()->get("blue")->setUniform("model", model);
	VertexManager::instance()->get("cube")->draw();

	ShaderManager::instance()->get("yellow")->use();
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(1.0f, 1.0f, -1.0f));
	ShaderManager::instance()->get("yellow")->setUniform("model", model);
	VertexManager::instance()->get("cube")->draw();
}

void setUbos() {
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = CameraManager::instance()->get("camera")->GetViewMatrix();

	Ubo *ubo = new Ubo(2 * sizeof(glm::mat4));
	ubo->add(projection, 0, sizeof(glm::mat4));
	ubo->add(view, sizeof(glm::mat4), sizeof(glm::mat4));
	UboManager::instance()->add("uboMatrices", ubo);
}