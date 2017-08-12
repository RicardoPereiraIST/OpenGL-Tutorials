#include "Deferred_Shading.h"

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
	setModels();

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
}

void setShader() {
	Shader* shaderGeometryPass = new Shader("Shaders/Deferred_Shading/g_buffer.vs", "Shaders/Deferred_Shading/g_buffer.fs");
	ShaderManager::instance()->add("shaderGeometryPass", shaderGeometryPass);

	Shader* shaderLightingPass = new Shader("Shaders/Deferred_Shading/deferred_shading.vs", "Shaders/Deferred_Shading/deferred_shading.fs");
	ShaderManager::instance()->add("shaderLightingPass", shaderLightingPass);

	Shader* shaderLightBox = new Shader("Shaders/Deferred_Shading/deferred_light_box.vs", "Shaders/Deferred_Shading/deferred_light_box.fs");
	ShaderManager::instance()->add("shaderLightBox", shaderLightBox);

	ShaderManager::instance()->get("shaderLightingPass")->use();
	ShaderManager::instance()->get("shaderLightingPass")->setUniform("gPosition", 0);
	ShaderManager::instance()->get("shaderLightingPass")->setUniform("gNormal", 1);
	ShaderManager::instance()->get("shaderLightingPass")->setUniform("gAlbedoSpec", 2);
}

void setTextures() {
	TextureLoader *t5 = new TextureLoader(3);
	t5->addTexture(false, width, height);
	t5->addTexture(false, width, height);
	t5->addTexture(false, width, height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	TextureManager::instance()->add("gbuffers", t5);
}

void setVertices() {
	VertexBuffers *v1 = new VertexBuffers(hdrVertices, true, true);
	VertexManager::instance()->add("cube", v1);
	
	VertexBuffers *v2 = new VertexBuffers(hdrQuad, false, true);
	VertexManager::instance()->add("quad", v2);

	objectPositions.push_back(glm::vec3(-3.0, -3.0, -3.0));
	objectPositions.push_back(glm::vec3(0.0, -3.0, -3.0));
	objectPositions.push_back(glm::vec3(3.0, -3.0, -3.0));
	objectPositions.push_back(glm::vec3(-3.0, -3.0, 0.0));
	objectPositions.push_back(glm::vec3(0.0, -3.0, 0.0));
	objectPositions.push_back(glm::vec3(3.0, -3.0, 0.0));
	objectPositions.push_back(glm::vec3(-3.0, -3.0, 3.0));
	objectPositions.push_back(glm::vec3(0.0, -3.0, 3.0));
	objectPositions.push_back(glm::vec3(3.0, -3.0, 3.0));
}

void setModels() {
	Model *nanosuit = new Model("Models/nanosuit/nanosuit.obj");
	ModelManager::instance()->add("nanosuit", nanosuit);
}

void setFrameBuffers() {
	FrameBuffer *f1 = new FrameBuffer();
	f1->createBuffers("gbuffers", width, height);
	FrameBufferManager::instance()->add("gBuffer", f1);
}

void drawObjects() {
	FrameBufferManager::instance()->get("gBuffer")->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 projection = glm::perspective(glm::radians(CameraManager::instance()->get("camera")->Zoom), (float)width / (float)height, 0.1f, 100.0f);
	glm::mat4 view = CameraManager::instance()->get("camera")->GetViewMatrix();
	glm::mat4 model;
	ShaderManager::instance()->get("shaderGeometryPass")->use();
	ShaderManager::instance()->get("shaderGeometryPass")->setUniform("projection", projection);
	ShaderManager::instance()->get("shaderGeometryPass")->setUniform("view", view);
	for (unsigned int i = 0; i < objectPositions.size(); i++)
	{
		model = glm::mat4();
		model = glm::translate(model, objectPositions[i]);
		model = glm::scale(model, glm::vec3(0.25f));
		ShaderManager::instance()->get("shaderGeometryPass")->setUniform("model", model);
		ModelManager::instance()->get("nanosuit")->Draw("shaderGeometryPass");
	}
	FrameBufferManager::instance()->unbind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	TextureManager::instance()->get("gbuffers")->bind(0, 0);
	TextureManager::instance()->get("gbuffers")->bind(1, 1);
	TextureManager::instance()->get("gbuffers")->bind(2, 2);

	ShaderManager::instance()->get("shaderLightingPass")->use();
	ShaderManager::instance()->get("shaderLightingPass")->setUniform("material.shininess", 16.0f);
	ShaderManager::instance()->get("shaderLightingPass")->setUniform("viewPos", CameraManager::instance()->get("camera")->Position);
	VertexManager::instance()->get("quad")->draw(GL_TRIANGLE_STRIP);

	FrameBufferManager::instance()->blitToDefault("gBuffer", width, height, GL_DEPTH_BUFFER_BIT);

	// now render all light cubes with forward rendering as we'd normally do
	ShaderManager::instance()->get("shaderLightBox")->use();
	ShaderManager::instance()->get("shaderLightBox")->setUniform("projection", projection);
	ShaderManager::instance()->get("shaderLightBox")->setUniform("view", view);
	for (unsigned int i = 0; i < lightPositions.size(); i++)
	{
		model = glm::mat4();
		model = glm::translate(model, lightPositions[i]);
		model = glm::scale(model, glm::vec3(0.25f));
		ShaderManager::instance()->get("shaderLightBox")->setUniform("model", model);
		ShaderManager::instance()->get("shaderLightBox")->setUniform("lightColor", lightColors[i]);
		VertexManager::instance()->get("cube")->draw();
	}
}

void setPointLights() {

	srand(13);
	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		// calculate slightly random offsets
		float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
		float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// also calculate random color
		float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}

	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		Light *pl1 = new Point_Light("shaderLightingPass", glm::vec3(0.1f, 0.1f, 0.1f)/32.0f, glm::vec3(0.1f, 0.1f, 0.1f)/32.0f, lightColors[i], lightPositions[i], 1.0f, 0.7f, 1.8f, i, 256/5);
		LightManager::instance()->add("pointLight"+std::to_string(i), pl1);
	}
}