#include "Instancing.h"

int main() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	setShader();
	setVertices();
	setModelMatrices();
	setModels();

	gameLoop();
	delete modelMatrices;
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
	Shader* shader = new Shader("Shaders/Instancing/instancing_planets.vs", "Shaders/Instancing/instancing_planets.fs");
	ShaderManager::instance()->add("original", shader);

	Shader* asteroids = new Shader("Shaders/Instancing/instancing_asteroids.vs", "Shaders/Instancing/instancing_planets.fs");
	ShaderManager::instance()->add("asteroids", asteroids);
}

void setVertices() {
	VertexBuffers *vertex;
	vertex = new VertexBuffers(instancingVertices, true, false, translations, 2);
	VertexManager::instance()->add("square", vertex);
}

void setModels() {
	Model *planet = new Model("Models/planet/planet.obj");
	ModelManager::instance()->add("planet", planet);

	Model *rock = new Model("Models/rock/rock.obj");
	// Instances attribute on shader is 5
	rock->addInstances(5, amount * sizeof(glm::mat4), modelMatrices);
	ModelManager::instance()->add("rock", rock);
}

void setModelMatrices() {
	modelMatrices = new glm::mat4[amount];
	srand(glfwGetTime()); // initialize random seed	
	float radius = 50.0;
	float offset = 2.5f;
	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model;
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep HEIGHT of field smaller compared to WIDTH of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		modelMatrices[i] = model;
	}
}

void drawObjects() {
	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

							 // make sure we clear the framebuffer's content
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	ShaderManager::instance()->get("original")->use();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
	glm::mat4 view = CameraManager::instance()->get("camera")->GetViewMatrix();;
	ShaderManager::instance()->get("original")->setUniform("projection", projection);
	ShaderManager::instance()->get("original")->setUniform("view", view);

	ShaderManager::instance()->get("asteroids")->use();
	ShaderManager::instance()->get("asteroids")->setUniform("projection", projection);
	ShaderManager::instance()->get("asteroids")->setUniform("view", view);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
	model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));

	ShaderManager::instance()->get("original")->use();
	ShaderManager::instance()->get("original")->setUniform("model", model);
	ModelManager::instance()->get("planet")->Draw("original");

	// draw meteorites
	ShaderManager::instance()->get("asteroids")->use();
	ModelManager::instance()->get("rock")->Draw("asteroids", true, amount);
}