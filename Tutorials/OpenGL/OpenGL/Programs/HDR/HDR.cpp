#include "HDR.h"

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
	t2->loadFrame(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
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
	glm::mat4 projection = glm::perspective(CameraManager::instance()->get("camera")->Zoom, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
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
	FrameBuffer *f1 = new FrameBuffer("screen", width, height);
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