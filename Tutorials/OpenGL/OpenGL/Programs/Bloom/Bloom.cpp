#include "Bloom.h"

int main() {
	initializeGLFW();
	createWindow();
	initializeGLAD();
	initializeDebug();

	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, width, height);
	setCallbacks();

	//Camera
	Camera *camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	CameraManager::instance()->add("camera", camera);

	setShader();
	setVertices();
	setTextures();
	setFrameBuffers();
	setPointLights();

	gameLoop();
	glfwTerminate();
	glUseProgram(0);
	glfwDestroyWindow(window);
	return 0;
}

void initializeGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // comment this line in a release build!
}

int initializeGLAD() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	return 0;
}

void initializeDebug() {
	GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
}

void setCallbacks() {
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
}

int createWindow() {
	window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);

	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	CameraManager::instance()->get("camera")->ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	CameraManager::instance()->get("camera")->ProcessMouseScroll(yoffset);
}

void gameLoop() {
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawObjects();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		CameraManager::instance()->get("camera")->ProcessKeyboard(FORWARD, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		CameraManager::instance()->get("camera")->ProcessKeyboard(BACKWARD, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		CameraManager::instance()->get("camera")->ProcessKeyboard(LEFT, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		CameraManager::instance()->get("camera")->ProcessKeyboard(RIGHT, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !bloomKeyPressed) {
		bloom = !bloom;
		bloomKeyPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) {
		bloomKeyPressed = false;
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
	Shader* shader = new Shader("Shaders/Bloom/bloom.vs", "Shaders/Bloom/bloom.fs");
	ShaderManager::instance()->add("shader", shader);

	Shader* shaderLight = new Shader("Shaders/Bloom/bloom.vs", "Shaders/Bloom/light_box.fs");
	ShaderManager::instance()->add("shaderLight", shaderLight);

	Shader* shaderBlur = new Shader("Shaders/Bloom/blur.vs", "Shaders/Bloom/blur.fs");
	ShaderManager::instance()->add("shaderBlur", shaderBlur);

	Shader* shaderBloomFinal = new Shader("Shaders/Bloom/bloom_final.vs", "Shaders/Bloom/bloom_final.fs");
	ShaderManager::instance()->add("shaderBloomFinal", shaderBloomFinal);

	ShaderManager::instance()->get("shader")->use();
	ShaderManager::instance()->get("shader")->setUniform("diffuseTexture", 0);
	ShaderManager::instance()->get("shaderBlur")->use();
	ShaderManager::instance()->get("shaderBlur")->setUniform("image", 0);
	ShaderManager::instance()->get("shaderBloomFinal")->use();
	ShaderManager::instance()->get("shaderBloomFinal")->setUniform("scene", 0);
	ShaderManager::instance()->get("shaderBloomFinal")->setUniform("bloomBlur", 1);
}

void setTextures() {
	TextureLoader *t1 = new TextureLoader();
	t1->load("Images/floor/wood.png", true);
	TextureManager::instance()->add("wood", t1);

	TextureLoader *t2 = new TextureLoader();
	t2->load("Images/containers/container2.png", true);
	TextureManager::instance()->add("container", t2);

	TextureLoader *t3 = new TextureLoader(2);
	t3->createTexture(width, height);
	TextureManager::instance()->add("colorBuffers", t3);

	TextureLoader *t5 = new TextureLoader(2);
	t5->createTexture(width, height);
	TextureManager::instance()->add("pingpongBuffers", t5);
}

void setVertices() {
	lightPositions.push_back(glm::vec3(0.0f, 0.5f, 1.5f));
	lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
	lightPositions.push_back(glm::vec3(3.0f, 0.5f, 1.0f));
	lightPositions.push_back(glm::vec3(-.8f, 2.4f, -1.0f));
	// colors
	lightColors.push_back(glm::vec3(2.0f, 2.0f, 2.0f));
	lightColors.push_back(glm::vec3(1.5f, 0.0f, 0.0f));
	lightColors.push_back(glm::vec3(0.0f, 0.0f, 1.5f));
	lightColors.push_back(glm::vec3(0.0f, 1.5f, 0.0f));

	VertexBuffers *v1 = new VertexBuffers(hdrVertices, true, true);
	VertexManager::instance()->add("cube", v1);
	
	VertexBuffers *v2 = new VertexBuffers(hdrQuad, false, true);
	VertexManager::instance()->add("quad", v2);
}

void setFrameBuffers() {
	FrameBuffer *f1 = new FrameBuffer();
	f1->createBuffers("colorBuffers", width, height);
	FrameBufferManager::instance()->add("hdr", f1);

	FrameBuffer *f2 = new FrameBuffer(2);
	f2->createMultipleBuffers("pingpongBuffers", width, height, false);
	FrameBufferManager::instance()->add("pingpong", f2);
}

void drawObjects() {
	FrameBufferManager::instance()->get("hdr")->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 projection = glm::perspective(CameraManager::instance()->get("camera")->Zoom, (float)width / (float)height, 0.1f, 100.0f);
	glm::mat4 view = CameraManager::instance()->get("camera")->GetViewMatrix();
	glm::mat4 model;
	ShaderManager::instance()->get("shader")->use();
	ShaderManager::instance()->get("shader")->setUniform("projection", projection);
	ShaderManager::instance()->get("shader")->setUniform("view", view);
	TextureManager::instance()->get("wood")->bind(0);

	ShaderManager::instance()->get("shader")->setUniform("viewPos", CameraManager::instance()->get("camera")->Position);
	// create one large cube that acts as the floor
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
	model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
	ShaderManager::instance()->get("shader")->setUniform("model", model);
	ShaderManager::instance()->get("shader")->setUniform("model", model);
	VertexManager::instance()->get("cube")->draw();
	// then create multiple cubes as the scenery
	TextureManager::instance()->get("container")->bind(0);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	ShaderManager::instance()->get("shader")->setUniform("model", model);
	VertexManager::instance()->get("cube")->draw();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.5f));
	ShaderManager::instance()->get("shader")->setUniform("model", model);
	VertexManager::instance()->get("cube")->draw();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	ShaderManager::instance()->get("shader")->setUniform("model", model);
	VertexManager::instance()->get("cube")->draw();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
	model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(1.25));
	ShaderManager::instance()->get("shader")->setUniform("model", model);
	VertexManager::instance()->get("cube")->draw();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
	model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	ShaderManager::instance()->get("shader")->setUniform("model", model);
	VertexManager::instance()->get("cube")->draw();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	ShaderManager::instance()->get("shader")->setUniform("model", model);
	VertexManager::instance()->get("cube")->draw();

	// finally show all the light sources as bright cubes
	ShaderManager::instance()->get("shaderLight")->use();
	ShaderManager::instance()->get("shaderLight")->setUniform("projection", projection);
	ShaderManager::instance()->get("shaderLight")->setUniform("view", view);

	for (unsigned int i = 0; i < lightPositions.size(); i++)
	{
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(lightPositions[i]));
		model = glm::scale(model, glm::vec3(0.25f));
		ShaderManager::instance()->get("shaderLight")->setUniform("model", model);
		ShaderManager::instance()->get("shaderLight")->setUniform("lightColor", lightColors[i]);
		VertexManager::instance()->get("cube")->draw();
	}
	FrameBufferManager::instance()->unbind();

	// 2. blur bright fragments with two-pass Gaussian Blur 
	// --------------------------------------------------
	bool horizontal = true, first_iteration = true;
	unsigned int amount = 10;
	ShaderManager::instance()->get("shaderBlur")->use();
	for (unsigned int i = 0; i < amount; i++)
	{
		FrameBufferManager::instance()->get("pingpong")->bindIndex(horizontal);
		ShaderManager::instance()->get("shaderBlur")->setUniform("horizontal", horizontal);
		// bind texture of other framebuffer (or scene if first iteration)
		if (first_iteration)
			TextureManager::instance()->get("colorBuffers")->bind(0, 1);
		else
			TextureManager::instance()->get("pingpongBuffers")->bind(0, !horizontal);

		VertexManager::instance()->get("quad")->draw(GL_TRIANGLE_STRIP);
		horizontal = !horizontal;

		if (first_iteration)
			first_iteration = false;
	}
	FrameBufferManager::instance()->unbind();

	// 3. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
	// --------------------------------------------------------------------------------------------------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ShaderManager::instance()->get("shaderBloomFinal")->use();
	TextureManager::instance()->get("colorBuffers")->bind(0, 0);
	TextureManager::instance()->get("pingpongBuffers")->bind(1, horizontal);

	ShaderManager::instance()->get("shaderBloomFinal")->setUniform("bloom", bloom);
	ShaderManager::instance()->get("shaderBloomFinal")->setUniform("exposure", exposure);
	VertexManager::instance()->get("quad")->draw(GL_TRIANGLE_STRIP);
}

void setPointLights() {
	Light *pl1 = new Point_Light("shader", glm::vec3(0.0f, 0.0f, 0.0f), lightColors[0], glm::vec3(0.0f, 0.0f, 0.0f), lightPositions[0], 0.0f, 0.0f, 1.0f, 0);
	LightManager::instance()->add("pointLight1", pl1);

	Light *pl2 = new Point_Light("shader", glm::vec3(0.0f, 0.0f, 0.0f), lightColors[1], glm::vec3(0.0f, 0.0f, 0.0f), lightPositions[1], 0.0f, 0.0f, 1.0f, 1);
	LightManager::instance()->add("pointLight2", pl2);

	Light *pl3 = new Point_Light("shader", glm::vec3(0.0f, 0.0f, 0.0f), lightColors[2], glm::vec3(0.0f, 0.0f, 0.0f), lightPositions[2], 0.0f, 0.0f, 1.0f, 2);
	LightManager::instance()->add("pointLight3", pl3);

	Light *pl4 = new Point_Light("shader", glm::vec3(0.0f, 0.0f, 0.0f), lightColors[3], glm::vec3(0.0f, 0.0f, 0.0f), lightPositions[3], 0.0f, 0.0f, 1.0f, 3);
	LightManager::instance()->add("pointLight4", pl4);
}