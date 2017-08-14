#include "Blinn_Phong.h"

int main() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	setShader();
	setVertices();
	setTextures();
	setLights();
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

		updateSpotLight();
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

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
	{
		blinn = !blinn;
		blinnKeyPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
	{
		blinnKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && !pointKeyPressed)
	{
		point = !point;
		pointKeyPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE)
	{
		pointKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && !dirKeyPressed)
	{
		dir = !dir;
		dirKeyPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE)
	{
		dirKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !spotKeyPressed)
	{
		spot = !spot;
		spotKeyPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
	{
		spotKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !gammaKeyPressed)
	{
		gamma = !gamma;
		gammaKeyPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE)
	{
		gammaKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !framebufferKeyPressed)
	{
		framebuffer = !framebuffer;
		framebufferKeyPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
	{
		framebufferKeyPressed = false;
	}
}

void setShader() {
	Shader* shader = new Shader("Shaders/Blinn_Phong/blinn_phong.vs", "Shaders/Blinn_Phong/blinn_phong.fs");
	ShaderManager::instance()->add("original", shader);

	Shader* shader3 = new Shader("Shaders/Blinn_Phong/gamma.vs", "Shaders/Blinn_Phong/gamma.fs");
	ShaderManager::instance()->add("screen", shader3);

	ShaderManager::instance()->get("original")->use();
	ShaderManager::instance()->get("original")->setUniform("floorTexture", 0);

	ShaderManager::instance()->get("screen")->use();
	ShaderManager::instance()->get("screen")->setUniform("screenTexture", 0);
}

void setTextures() {
	TextureLoader *texture1 = new TextureLoader();
	texture1->load("Images/floor/vector-wood-texture.jpg");
	TextureManager::instance()->add("plane", texture1);

	TextureLoader *texture2 = new TextureLoader();
	texture2->load("Images/floor/vector-wood-texture.jpg", true);
	TextureManager::instance()->add("plane_gamma", texture2);

	TextureLoader *texture4 = new TextureLoader();
	texture4->loadFrame(WIDTH, HEIGHT);
	TextureManager::instance()->add("screen", texture4);
}

void setVertices() {
	VertexBuffers *vertex;
	vertex = new VertexBuffers(planeVerticesBlinnPhong, true, true);
	VertexManager::instance()->add("plane", vertex);

	VertexBuffers *vertex4;
	vertex4 = new VertexBuffers(quadVertices, false, true, 2);
	VertexManager::instance()->add("screen", vertex4);
}

void drawObjects() {

	if(framebuffer)
		FrameBufferManager::instance()->get("screen")->bind();

	glEnable(GL_DEPTH_TEST);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	ShaderManager::instance()->get("original")->use();
	glm::mat4 projection = glm::perspective(CameraManager::instance()->get("camera")->Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
	glm::mat4 view = CameraManager::instance()->get("camera")->GetViewMatrix();
	ShaderManager::instance()->get("original")->setUniform("projection", projection);
	ShaderManager::instance()->get("original")->setUniform("view", CameraManager::instance()->get("camera")->GetViewMatrix());

	if (blinn)
		ShaderManager::instance()->get("original")->setUniform("material.shininess", 32.0f);
	else
		ShaderManager::instance()->get("original")->setUniform("material.shininess", 8.0f);

	// set light uniforms
	ShaderManager::instance()->get("original")->setUniform("viewPos", CameraManager::instance()->get("camera")->Position);
	ShaderManager::instance()->get("original")->setUniform("blinn", blinn);
	ShaderManager::instance()->get("original")->setUniform("point", point);
	ShaderManager::instance()->get("original")->setUniform("dir", dir);
	ShaderManager::instance()->get("original")->setUniform("spot", spot);
	ShaderManager::instance()->get("original")->setUniform("gamma", gamma);
	// floor
	VertexManager::instance()->get("plane")->draw(gamma ? "plane_gamma" : "plane");

	if (framebuffer) {
		FrameBufferManager::instance()->unbind();
		glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
									// clear all relevant buffers
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		ShaderManager::instance()->get("screen")->use();
		VertexManager::instance()->get("screen")->draw("screen");
	}
}

void setLights() {
	Light *dl = new Directional_Light("original", glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(-0.0f, -0.1f, -0.0f));
	LightManager::instance()->add("dirLight", dl);

	Light *pl1 = new Point_Light("original", glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.09f, 0.032f, 0);
	LightManager::instance()->add("pointLight0", pl1);

	Light *sl = new Spotlight("original", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), CameraManager::instance()->get("camera")->Position, CameraManager::instance()->get("camera")->Front, 1.0f, 0.09f, 0.032f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)));
	LightManager::instance()->add("spotLight", sl);
}

void updateSpotLight() {
	Spotlight *s1 = (Spotlight*) LightManager::instance()->get("spotLight");
	s1->setPosition(CameraManager::instance()->get("camera")->Position);
	s1->setDirection(CameraManager::instance()->get("camera")->Front);
}

void setFrameBuffers() {
	FrameBuffer * fb1 = new FrameBuffer("screen", WIDTH, HEIGHT);
	FrameBufferManager::instance()->add("screen", fb1);
}