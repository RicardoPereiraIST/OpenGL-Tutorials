#include "HDR.h"

int main() {
	glEnable(GL_DEPTH_TEST);

	setShader();
	setVertices();
	setTextures();
	setFrameBuffers();
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
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !hdrKeyPressed) {
		hdr = !hdr;
		hdrKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) {
		hdrKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		if (exposure > 0.0f)
			exposure -= 0.01f;
		else
			exposure = 0.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		exposure += 0.01f;
	}
}

void setShader() {
	Shader* shader = new Shader("Shaders/HDR/hdr_lights.vs", "Shaders/HDR/hdr_lights.fs");
	ShaderManager::instance()->add("shader", shader);

	Shader* hdrShader = new Shader("Shaders/HDR/hdr.vs", "Shaders/HDR/hdr.fs");
	ShaderManager::instance()->add("hdrShader", hdrShader);

	ShaderManager::instance()->get("shader")->use();
	ShaderManager::instance()->get("shader")->setUniform("diffuseTexture", 0);

	ShaderManager::instance()->get("hdrShader")->use();
	ShaderManager::instance()->get("hdrShader")->setUniform("hdrBuffer", 0);
}

void setTextures() {
	TextureLoader *t1 = new TextureLoader();
	t1->load("Images/floor/wood.png", true);
	TextureManager::instance()->add("wood", t1);

	TextureLoader *t2 = new TextureLoader();
	t2->loadFrame(WIDTH, HEIGHT, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	TextureManager::instance()->add("screen", t2);
}

void setVertices() {
	VertexBuffers *v1 = new VertexBuffers(hdrVertices, true, true);
	VertexManager::instance()->add("cube", v1);

	VertexBuffers *v2 = new VertexBuffers(hdrQuad, false, true);
	VertexManager::instance()->add("quad", v2);
}

void drawObjects() {
	FrameBufferManager::instance()->get("screen")->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 projection = glm::perspective(CameraManager::instance()->get("camera")->Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = CameraManager::instance()->get("camera")->GetViewMatrix();
	ShaderManager::instance()->get("shader")->use();
	ShaderManager::instance()->get("shader")->setUniform("projection", projection);
	ShaderManager::instance()->get("shader")->setUniform("view", view);
	TextureManager::instance()->get("wood")->bind(0);
	ShaderManager::instance()->get("shader")->setUniform("viewPos", CameraManager::instance()->get("camera")->Position);
	// render tunnel
	glm::mat4 model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0));
	model = glm::scale(model, glm::vec3(2.5f, 2.5f, 27.5f));
	ShaderManager::instance()->get("shader")->setUniform("model", model);
	ShaderManager::instance()->get("shader")->setUniform("inverse_normals", true);
	VertexManager::instance()->get("cube")->draw();
	FrameBufferManager::instance()->unbind();

	// 2. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
	// --------------------------------------------------------------------------------------------------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ShaderManager::instance()->get("hdrShader")->use();
	TextureManager::instance()->get("screen")->bind(0);
	ShaderManager::instance()->get("hdrShader")->setUniform("hdr", hdr);
	ShaderManager::instance()->get("hdrShader")->setUniform("exposure", exposure);
	VertexManager::instance()->get("quad")->draw(GL_TRIANGLE_STRIP);

}

void setFrameBuffers() {
	FrameBuffer *f1 = new FrameBuffer("screen", WIDTH, HEIGHT);
	FrameBufferManager::instance()->add("screen", f1);
}

void setPointLights() {
	Light *pl1 = new Point_Light("shader", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(200.0f, 200.0f, 200.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 49.5f), 0.0f, 0.0f, 1.0f, 0);
	LightManager::instance()->add("pointLight1", pl1);

	Light *pl2 = new Point_Light("shader", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.4f, -1.9f, 9.0f), 0.0f, 0.0f, 1.0f, 1);
	LightManager::instance()->add("pointLight2", pl2);

	Light *pl3 = new Point_Light("shader", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.2f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.8f, 4.0f), 0.0f, 0.0f, 1.0f, 2);
	LightManager::instance()->add("pointLight3", pl3);

	Light *pl4 = new Point_Light("shader", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.8f, -1.7f, 6.0f), 0.0f, 0.0f, 1.0f, 3);
	LightManager::instance()->add("pointLight4", pl4);
}