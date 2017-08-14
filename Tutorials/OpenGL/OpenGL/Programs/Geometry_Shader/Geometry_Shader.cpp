#include "Geometry_Shader.h"

int main() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	setShader();
	setVertices();
	setModels();

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

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !explodeKeyPressed)
	{
		explode = !explode;
		explodeKeyPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE)
	{
		explodeKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && !normalsKeyPressed)
	{
		normals = !normals;
		normalsKeyPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE)
	{
		normalsKeyPressed = false;
	}
}

void setShader() {
	Shader *shader = new Shader("Shaders/Geometry_Shader/geometry.vs", "Shaders/Geometry_Shader/geometry.fs", "Shaders/Geometry_Shader/geometry.gs");
	ShaderManager::instance()->add("original", shader);

	Shader* model = new Shader("Shaders/Geometry_Shader/explode_model.vs", "Shaders/Geometry_Shader/explode_model.fs", "Shaders/Geometry_Shader/explode_model.gs");
	ShaderManager::instance()->add("explode", model);

	Shader* normals = new Shader("Shaders/Geometry_Shader/visualize_normals.vs", "Shaders/Geometry_Shader/visualize_normals.fs", "Shaders/Geometry_Shader/visualize_normals.gs");
	ShaderManager::instance()->add("normals", normals);

	Shader* normal_model = new Shader("Shaders/Geometry_Shader/normal_model.vs", "Shaders/Geometry_Shader/normal_model.fs");
	ShaderManager::instance()->add("model", normal_model);
}

void setVertices() {
	VertexBuffers *vertex;
	vertex = new VertexBuffers(points, false, false, 2);
	VertexManager::instance()->add("square", vertex);

	VertexBuffers *vertex2;
	vertex2 = new VertexBuffers(pointsColors, true, false, 2);
	VertexManager::instance()->add("square2", vertex2);
}


void drawObjects() {
	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

							 // make sure we clear the framebuffer's content
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 1.0f, 100.0f);
	glm::mat4 view = CameraManager::instance()->get("camera")->GetViewMatrix();;
	glm::mat4 model;

	if (explode) {
		ShaderManager::instance()->get("explode")->use();
		ShaderManager::instance()->get("explode")->setUniform("projection", projection);
		ShaderManager::instance()->get("explode")->setUniform("view", view);
		ShaderManager::instance()->get("explode")->setUniform("model", model);

		// add time component to geometry shader in the form of a uniform

		ShaderManager::instance()->get("explode")->setUniform("time", (float)glfwGetTime());
		ModelManager::instance()->get("nanosuit")->Draw("explode");
	}
	else {
		//SEE NORMALS
		ShaderManager::instance()->get("model")->use();
		ShaderManager::instance()->get("model")->setUniform("projection", projection);
		ShaderManager::instance()->get("model")->setUniform("view", view);
		ShaderManager::instance()->get("model")->setUniform("model", model);
		ModelManager::instance()->get("nanosuit")->Draw("model");
	}

	if (normals) {
		ShaderManager::instance()->get("normals")->use();
		ShaderManager::instance()->get("normals")->setUniform("projection", projection);
		ShaderManager::instance()->get("normals")->setUniform("view", view);
		ShaderManager::instance()->get("normals")->setUniform("model", model);
		ModelManager::instance()->get("nanosuit")->Draw("normals");
	}
}

void setModels() {
	Model *ourModel = new Model("Models/nanosuit_reflection/nanosuit.obj");
	ModelManager::instance()->add("nanosuit", ourModel);
}