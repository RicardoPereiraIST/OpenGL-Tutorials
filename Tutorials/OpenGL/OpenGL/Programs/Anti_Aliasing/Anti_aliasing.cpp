#include "Anti_aliasing.h"

int main() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_MULTISAMPLE);
	
	setShader();
	setVertices();
	setTextures();
	setFrameBuffers();

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
	Shader* shader = new Shader("Shaders/Anti_Aliasing/anti_aliasing.vs", "Shaders/Anti_Aliasing/anti_aliasing.fs");
	ShaderManager::instance()->add("original", shader);

	//grayscale
	Shader *shader2 = new Shader("Shaders/Anti_Aliasing/simple_post.vs", "Shaders/Anti_Aliasing/simple_post.fs");
	ShaderManager::instance()->add("screen", shader2);

	ShaderManager::instance()->get("screen")->use();
	ShaderManager::instance()->get("screen")->setUniform("screenTexture", 0);
}

void setTextures() {
	TextureLoader *texture4 = new TextureLoader();
	texture4->loadFrame(WIDTH, HEIGHT);
	TextureManager::instance()->add("screen", texture4);
}

void setVertices() {
	VertexBuffers *vertex;
	vertex = new VertexBuffers(simpleCubeVertices, false, false);
	VertexManager::instance()->add("cube", vertex);

	VertexBuffers *vertex4;
	vertex4 = new VertexBuffers(quadVertices, false, true, 2);
	VertexManager::instance()->add("screen", vertex4);
}

void drawObjects() {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 1. draw scene as normal in multisampled buffers
	FrameBufferManager::instance()->get("anti_aliasing")->bind();
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// set transformation matrices		
	ShaderManager::instance()->get("original")->use();
	glm::mat4 projection = glm::perspective(CameraManager::instance()->get("camera")->Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
	ShaderManager::instance()->get("original")->setUniform("projection", projection);
	ShaderManager::instance()->get("original")->setUniform("view", CameraManager::instance()->get("camera")->GetViewMatrix());
	ShaderManager::instance()->get("original")->setUniform("model", glm::mat4());

	VertexManager::instance()->get("cube")->draw();

	// 2. now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
	FrameBufferManager::instance()->blit("anti_aliasing", "screen", WIDTH, HEIGHT);

	// 3. now render quad with scene's visuals as its texture image
	FrameBufferManager::instance()->unbind();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	// draw Screen quad
	ShaderManager::instance()->get("screen")->use();
	VertexManager::instance()->get("screen")->draw("screen");
}

void setFrameBuffers() {
	FrameBuffer * fb1 = new FrameBuffer(WIDTH, HEIGHT, 4);
	FrameBufferManager::instance()->add("anti_aliasing", fb1);

	FrameBuffer * fb2 = new FrameBuffer("screen", WIDTH, HEIGHT, false, false, true, true);
	FrameBufferManager::instance()->add("screen", fb2);
}