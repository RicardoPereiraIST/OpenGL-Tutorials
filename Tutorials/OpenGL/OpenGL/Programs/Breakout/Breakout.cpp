#include "Breakout.h"

int main() {
	glfwSetKeyCallback(init.window, key_callback);

	//Camera
	Camera *camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	CameraManager::instance()->add("camera", camera);

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	manager = manager->instance(WIDTH, HEIGHT);

	gameLoop();

	manager->destroy();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			manager->keys[key] = true;
		else if (action == GLFW_RELEASE) {
			manager->keys[key] = false;
			manager->keysProcessed[key] = false;
		}
	}
}

void gameLoop() {
	while (!glfwWindowShouldClose(init.window)) {
		float currentFrame = glfwGetTime();
		init.deltaTime = currentFrame - init.lastFrame;
		init.lastFrame = currentFrame;

		manager->processInput(init.deltaTime);
		manager->update(init.deltaTime);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		manager->render();
		
		glfwPollEvents();
		glfwSwapBuffers(init.window);
	}
}