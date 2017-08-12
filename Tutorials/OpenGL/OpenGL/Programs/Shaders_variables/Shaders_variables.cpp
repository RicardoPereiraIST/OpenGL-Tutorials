#include "Shaders_variables.h"

int main() {
	initializeGLFW();
	createWindow();
	initializeGLAD();
	initializeDebug();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_PROGRAM_POINT_SIZE);

	glViewport(0, 0, width, height);
	setCallbacks();

	//Camera
	Camera *camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	CameraManager::instance()->add("camera", camera);

	setShader();
	setVertices();
	setUbos();

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
	Shader* red = new Shader("Shaders/Shaders_variables/pointsize.vs", "Shaders/Shaders_variables/red.fs");
	ShaderManager::instance()->add("red", red);

	Shader* green = new Shader("Shaders/Shaders_variables/pointsize.vs", "Shaders/Shaders_variables/green.fs");
	ShaderManager::instance()->add("green", green);

	Shader* yellow = new Shader("Shaders/Shaders_variables/pointsize.vs", "Shaders/Shaders_variables/yellow.fs");
	ShaderManager::instance()->add("yellow", yellow);

	Shader* blue = new Shader("Shaders/Shaders_variables/pointsize.vs", "Shaders/Shaders_variables/blue.fs");
	ShaderManager::instance()->add("blue", blue);

	ShaderManager::instance()->get("red")->addUniformBlock("Matrices", 0);
	ShaderManager::instance()->get("green")->addUniformBlock("Matrices", 0);
	ShaderManager::instance()->get("blue")->addUniformBlock("Matrices", 0);
	ShaderManager::instance()->get("yellow")->addUniformBlock("Matrices", 0);
}

void setVertices() {
	VertexBuffers *vertex;
	vertex = new VertexBuffers(cubeVerticesFaceCullCCW, false, true);
	VertexManager::instance()->add("cube", vertex);
}


void drawObjects() {
	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

							  //make sure we clear the framebuffer's content
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glm::mat4 view = CameraManager::instance()->get("camera")->GetViewMatrix();
	UboManager::instance()->get("uboMatrices")->add(view, sizeof(glm::mat4), sizeof(glm::mat4));

	ShaderManager::instance()->get("red")->use();

	glm::mat4 model;
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-1.0f, -1.0f, -1.0f));
	ShaderManager::instance()->get("red")->setUniform("model", model);
	VertexManager::instance()->get("cube")->draw();

	ShaderManager::instance()->get("green")->use();
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(1.0f, -1.0f, -1.0f));
	ShaderManager::instance()->get("green")->setUniform("model", model);
	VertexManager::instance()->get("cube")->draw();

	ShaderManager::instance()->get("blue")->use();
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-1.0f, 1.0f, -1.0f));
	ShaderManager::instance()->get("blue")->setUniform("model", model);
	VertexManager::instance()->get("cube")->draw();

	ShaderManager::instance()->get("yellow")->use();
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(1.0f, 1.0f, -1.0f));
	ShaderManager::instance()->get("yellow")->setUniform("model", model);
	VertexManager::instance()->get("cube")->draw();
}

void setUbos() {
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	glm::mat4 view = CameraManager::instance()->get("camera")->GetViewMatrix();

	Ubo *ubo = new Ubo(2 * sizeof(glm::mat4));
	ubo->add(projection, 0, sizeof(glm::mat4));
	ubo->add(view, sizeof(glm::mat4), sizeof(glm::mat4));
	UboManager::instance()->add("uboMatrices", ubo);
}