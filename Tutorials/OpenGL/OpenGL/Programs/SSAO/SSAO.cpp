#include "SSAO.h"

int main() {
	glEnable(GL_DEPTH_TEST);

	setShader();
	setVertices();
	setTextures();
	setFrameBuffers();
	setPointLights();
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
}

void setShader() {
	Shader* shaderGeometryPass = new Shader("Shaders/SSAO/ssao_geometry.vs", "Shaders/SSAO/ssao_geometry.fs");
	ShaderManager::instance()->add("shaderGeometryPass", shaderGeometryPass);

	Shader* shaderLightingPass = new Shader("Shaders/SSAO/ssao.vs", "Shaders/SSAO/ssao_lighting.fs");
	ShaderManager::instance()->add("shaderLightingPass", shaderLightingPass);

	Shader* shaderSSAO = new Shader("Shaders/SSAO/ssao.vs", "Shaders/SSAO/ssao.fs");
	ShaderManager::instance()->add("shaderSSAO", shaderSSAO);

	Shader* shaderSSAOBlur = new Shader("Shaders/SSAO/ssao.vs", "Shaders/SSAO/ssao_blur.fs");
	ShaderManager::instance()->add("shaderSSAOBlur", shaderSSAOBlur);
}

void setTextures() {
	TextureLoader *t3 = new TextureLoader(3);
	t3->addTexture(false, WIDTH, HEIGHT);
	t3->addTexture(false, WIDTH, HEIGHT);
	t3->addTexture(false, WIDTH, HEIGHT, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
	TextureManager::instance()->add("gbuffers", t3);

	TextureLoader *t6 = new TextureLoader();
	t6->addTexture(true, WIDTH, HEIGHT, GL_RED);
	TextureManager::instance()->add("ssaoColorBuffer", t6);

	TextureLoader *t7 = new TextureLoader();
	t7->addTexture(true, WIDTH, HEIGHT, GL_RED);
	TextureManager::instance()->add("ssaoColorBufferBlur", t7);

	////noise texture
	TextureLoader *t4 = new TextureLoader();
	t4->addTextureWithVector(true, 4, 4, ssaoNoise, GL_RGB32F);
	TextureManager::instance()->add("noiseTexture", t4);

	ShaderManager::instance()->get("shaderLightingPass")->use();
	ShaderManager::instance()->get("shaderLightingPass")->setUniform("gPosition", 0);
	ShaderManager::instance()->get("shaderLightingPass")->setUniform("gNormal", 1);
	ShaderManager::instance()->get("shaderLightingPass")->setUniform("gAlbedo", 2);
	ShaderManager::instance()->get("shaderLightingPass")->setUniform("ssao", 3);
	ShaderManager::instance()->get("shaderSSAO")->use();
	ShaderManager::instance()->get("shaderSSAO")->setUniform("gPosition", 0);
	ShaderManager::instance()->get("shaderSSAO")->setUniform("gNormal", 1);
	ShaderManager::instance()->get("shaderSSAO")->setUniform("texNoise", 2);
	ShaderManager::instance()->get("shaderSSAOBlur")->use();
	ShaderManager::instance()->get("shaderSSAOBlur")->setUniform("ssaoInput", 0);
}

void setVertices() {
	VertexBuffers *v1 = new VertexBuffers(hdrVertices, true, true);
	VertexManager::instance()->add("cube", v1);

	VertexBuffers *v2 = new VertexBuffers(hdrQuad, false, true);
	VertexManager::instance()->add("quad", v2);

	for (unsigned int i = 0; i < 64; ++i)
	{
		//if z between -1 and 1, we'd have a sphere sample kernel
		glm::vec3 sample(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator)
		);
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = (float)i / 64.0;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}

	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f);
		ssaoNoise.push_back(noise);
	}
}

void setModels() {
	Model *nanosuit = new Model("Models/nanosuit/nanosuit.obj");
	ModelManager::instance()->add("nanosuit", nanosuit);
}

void setFrameBuffers() {
	FrameBuffer *f1 = new FrameBuffer();
	f1->createBuffers("gbuffers", WIDTH, HEIGHT);
	FrameBufferManager::instance()->add("gBuffer", f1);

	FrameBuffer *f2 = new FrameBuffer("ssaoColorBuffer", WIDTH, HEIGHT, false);
	FrameBufferManager::instance()->add("ssaoFBO", f2);

	FrameBuffer *f3 = new FrameBuffer("ssaoColorBufferBlur", WIDTH, HEIGHT, false);
	FrameBufferManager::instance()->add("ssaoBlurFBO", f3);
}

void drawObjects() {
	FrameBufferManager::instance()->get("gBuffer")->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 projection = glm::perspective(glm::radians(CameraManager::instance()->get("camera")->Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 50.0f);
	glm::mat4 view = CameraManager::instance()->get("camera")->GetViewMatrix();
	glm::mat4 model;
	ShaderManager::instance()->get("shaderGeometryPass")->use();
	ShaderManager::instance()->get("shaderGeometryPass")->setUniform("projection", projection);
	ShaderManager::instance()->get("shaderGeometryPass")->setUniform("view", view);
	// room cube
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0, 7.0f, 0.0f));
	model = glm::scale(model, glm::vec3(7.5f, 7.5f, 7.5f));
	ShaderManager::instance()->get("shaderGeometryPass")->setUniform("model", model);
	ShaderManager::instance()->get("shaderGeometryPass")->setUniform("invertedNormals", 1); // invert normals as we're inside the cube
	VertexManager::instance()->get("cube")->draw();
	ShaderManager::instance()->get("shaderGeometryPass")->setUniform("invertedNormals", 0);
	// nanosuit model on the floor
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 5.0));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	ShaderManager::instance()->get("shaderGeometryPass")->setUniform("model", model);
	ModelManager::instance()->get("nanosuit")->Draw("shaderGeometryPass");
	FrameBufferManager::instance()->unbind();


	// 2. generate SSAO texture
	// ------------------------
	FrameBufferManager::instance()->get("ssaoFBO")->bind();
	glClear(GL_COLOR_BUFFER_BIT);
	ShaderManager::instance()->get("shaderSSAO")->use();
	// Send kernel + rotation 
	for (unsigned int i = 0; i < 64; ++i)
		ShaderManager::instance()->get("shaderSSAO")->setUniform(("samples[" + std::to_string(i) + "]").c_str(), ssaoKernel[i]);
	ShaderManager::instance()->get("shaderSSAO")->setUniform("projection", projection);
	TextureManager::instance()->get("gbuffers")->bind(0, 0);
	TextureManager::instance()->get("gbuffers")->bind(1, 1);
	TextureManager::instance()->get("noiseTexture")->bind(2);

	VertexManager::instance()->get("quad")->draw(GL_TRIANGLE_STRIP);
	FrameBufferManager::instance()->unbind();


	// 3. blur SSAO texture to remove noise
	// ------------------------------------
	FrameBufferManager::instance()->get("ssaoBlurFBO")->bind();
	glClear(GL_COLOR_BUFFER_BIT);
	ShaderManager::instance()->get("shaderSSAOBlur")->use();
	TextureManager::instance()->get("ssaoColorBuffer")->bind(0);
	VertexManager::instance()->get("quad")->draw(GL_TRIANGLE_STRIP);
	FrameBufferManager::instance()->unbind();


	// 4. lighting pass: traditional deferred Blinn-Phong lighting with added screen-space ambient occlusion
	// -----------------------------------------------------------------------------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ShaderManager::instance()->get("shaderLightingPass")->use();
	ShaderManager::instance()->get("shaderLightingPass")->setUniform("material.shininess", 8.0f);
	// send light relevant uniforms
	updatePointLights();
	TextureManager::instance()->get("gbuffers")->bind(0, 0);
	TextureManager::instance()->get("gbuffers")->bind(1, 1);
	TextureManager::instance()->get("gbuffers")->bind(2, 2);
	TextureManager::instance()->get("ssaoColorBufferBlur")->bind(3);
	VertexManager::instance()->get("quad")->draw(GL_TRIANGLE_STRIP);
}

void setPointLights() {
	Light *pl1 = new Point_Light("shaderLightingPass", glm::vec3(0.3f, 0.3f, 0.3f), lightColor, lightColor, glm::vec3(CameraManager::instance()->get("camera")->GetViewMatrix() * glm::vec4(lightPos, 1.0)), 1.0f, 0.09f, 0.032f, 0);
	LightManager::instance()->add("pointLight1", pl1);
}

void updatePointLights() {
	Point_Light *pl1 = (Point_Light *)LightManager::instance()->get("pointLight1");
	pl1->setPosition(glm::vec3(CameraManager::instance()->get("camera")->GetViewMatrix() * glm::vec4(lightPos, 1.0)));
}