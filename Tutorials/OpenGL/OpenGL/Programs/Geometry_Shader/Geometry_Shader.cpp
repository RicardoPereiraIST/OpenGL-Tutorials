#include "Geometry_Shader.h"

int main() {
	initializeGLFW();
	createWindow();
	initializeGLAD();
	initializeDebug();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glViewport(0, 0, width, height);
	setCallbacks();

	//Camera
	Camera *camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	CameraManager::instance()->add("camera", camera);

	setShader();
	setVertices();
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

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !explodeKeyPressed)
	{
		explode = !explode;
		explodeKeyPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE)
	{
		explodeKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && !normalsKeyPressed)
	{
		normals = !normals;
		normalsKeyPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE)
	{
		normalsKeyPressed = false;
	}
}

void setShader() {
	Shader *shader = new Shader("Shaders/Geometry_Shader/geometry.vs", "Shaders/Geometry_Shader/geometry.fs", "Shaders/Geometry_Shader/geometry.gs");
	ShaderManager::instance()->add("original", shader);

	Shader* model = new Shader("Shaders/Geometry_Shader/explode_model.vs", "Shaders/Geometry_Shader/explode_model.fs", "Shaders/Geometry_Shader/explode_model.gs");
	ShaderManager::instance()->add("explode", model);

	Shader* normals = new Shader("Shaders/Geometry_Shader/visualize_normals.vs", "Shaders/Geometry_Shader/visualize_normals.fs", "Shaders/Geometry_Shader/visualize_normals.gs");
	ShaderManager::instance()->add("normals", normals);

	Shader* normal_model = new Shader("Shaders/Geometry_Shader/normal_model.vs", "Shaders/Geometry_Shader/normal_model.fs");
	ShaderManager::instance()->add("model", normal_model);
}

void setVertices() {
	VertexBuffers *vertex;
	vertex = new VertexBuffers(points, false, false, 2);
	VertexManager::instance()->add("square", vertex);

	VertexBuffers *vertex2;
	vertex2 = new VertexBuffers(pointsColors, true, false, 2);
	VertexManager::instance()->add("square2", vertex2);
}


void drawObjects() {
	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

							 // make sure we clear the framebuffer's content
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 1.0f, 100.0f);
	glm::mat4 view = CameraManager::instance()->get("camera")->GetViewMatrix();;
	glm::mat4 model;

	if (explode) {
		ShaderManager::instance()->get("explode")->use();
		ShaderManager::instance()->get("explode")->setUniform("projection", projection);
		ShaderManager::instance()->get("explode")->setUniform("view", view);
		ShaderManager::instance()->get("explode")->setUniform("model", model);

		// add time component to geometry shader in the form of a uniform

		ShaderManager::instance()->get("explode")->setUniform("time", (float)glfwGetTime());
		ModelManager::instance()->get("nanosuit")->Draw("explode");
	}
	else {
		//SEE NORMALS
		ShaderManager::instance()->get("model")->use();
		ShaderManager::instance()->get("model")->setUniform("projection", projection);
		ShaderManager::instance()->get("model")->setUniform("view", view);
		ShaderManager::instance()->get("model")->setUniform("model", model);
		ModelManager::instance()->get("nanosuit")->Draw("model");
	}

	if (normals) {
		ShaderManager::instance()->get("normals")->use();
		ShaderManager::instance()->get("normals")->setUniform("projection", projection);
		ShaderManager::instance()->get("normals")->setUniform("view", view);
		ShaderManager::instance()->get("normals")->setUniform("model", model);
		ModelManager::instance()->get("nanosuit")->Draw("normals");
	}
}

void setModels() {
	Model *ourModel = new Model("Models/nanosuit_reflection/nanosuit.obj");
	ModelManager::instance()->add("nanosuit", ourModel);
}