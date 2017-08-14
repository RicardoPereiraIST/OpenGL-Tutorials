#include "Render_Text.h"

int main() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	setShader();
	setTextRenderer();

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
	Shader* shader = new Shader("Shaders/Text/text.vs", "Shaders/Text/text.fs");
	ShaderManager::instance()->add("text", shader);

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(WIDTH), 0.0f, static_cast<GLfloat>(HEIGHT));
	ShaderManager::instance()->get("text")->use();
	ShaderManager::instance()->get("text")->setUniform("projection", projection);
}

void setTextRenderer() {
	TextRenderer * t1 = new TextRenderer("text", "Fonts/Arial/arial.ttf");
	TextRendererManager::instance()->add("arial", t1);
}

void drawObjects() {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	TextRendererManager::instance()->get("arial")->renderText("This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
	TextRendererManager::instance()->get("arial")->renderText("(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
}