#include "Original.h"

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
	setModels();

	setLights();

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

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		updateSpotLight();
		setAndDrawObjects();

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

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !glassKeyPressed)
	{
		glass = !glass;
		glassKeyPressed = true;
	}
	
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE)
	{
		glassKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !refractKeyPressed)
	{
		refract = !refract;
		refractKeyPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
	{
		refractKeyPressed = false;
	}

}

void setShader() {
	Shader* shader = new Shader("Shaders/Original/simple_light_shader.vs", "Shaders/Original/simple_light_shader.fs");
	ShaderManager::instance()->add("lights", shader);

	Shader* shader2 = new Shader("Shaders/Original/light_maps.vs", "Shaders/Original/model.fs");
	ShaderManager::instance()->add("model", shader2);

	Shader* shader4 = new Shader("Shaders/Original/skybox.vs", "Shaders/Original/skybox.fs");
	ShaderManager::instance()->add("sky", shader4);

	Shader* shader5 = new Shader("Shaders/Original/reflect_cubemap.vs", "Shaders/Original/reflect_cubemap.fs");
	ShaderManager::instance()->add("cube_reflector", shader5);

	Shader* shader6 = new Shader("Shaders/Original/reflect_cubemap.vs", "Shaders/Original/refract_cubemap.fs");
	ShaderManager::instance()->add("cube_refractor", shader6);
}

void setTextures() {
	TextureLoader *texture4 = new TextureLoader(1, true);
	texture4->loadCubeMap(faces);
	TextureManager::instance()->add("sky", texture4);
}

void setVertices() {
	VertexBuffers *vertex;
	vertex = new VertexBuffers(cubeVerticesWithNormals, true, false);
	VertexManager::instance()->add("squareEBO", vertex);

	VertexBuffers *vertex2;
	vertex2 = new VertexBuffers(cubeWithMaps, true, true);
	VertexManager::instance()->add("fullCube", vertex2);

	VertexBuffers *vertex5;
	vertex5 = new VertexBuffers(skyboxVertices, false, false);
	VertexManager::instance()->add("sky", vertex5);

	VertexBuffers *vertex6;
	vertex6 = new VertexBuffers(verticesWithNormal, true, false);
	VertexManager::instance()->add("cube_reflector", vertex6);
}

void setModels() {
	Model *ourModel = new Model("Models/nanosuit_reflection/nanosuit.obj");
	ModelManager::instance()->add("nanosuit", ourModel);
}

void setLights() {
	Light *dl = new Directional_Light("model", glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(-0.2f, -1.0f, -0.3f));
	LightManager::instance()->add("dirLight", dl);

	for (int i = 0; i < 4; i++) {
		Light *p = new Point_Light("model", glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f), pointLightPositions[i], 1.0f, 0.09f, 0.032f, i);
		LightManager::instance()->add("pointLight"+std::to_string(i), p);
	}

	Light *sl = new Spotlight("model", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), CameraManager::instance()->get("camera")->Position, CameraManager::instance()->get("camera")->Front, 1.0f, 0.09f, 0.032f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)));
	LightManager::instance()->add("spotLight", sl);
}

void updateSpotLight() {
	Spotlight * s1 = (Spotlight *)LightManager::instance()->get("spotLight");
	s1->setPosition(CameraManager::instance()->get("camera")->Position);
	s1->setDirection(CameraManager::instance()->get("camera")->Front);
}

void setAndDrawObjects() {
	ShaderManager::instance()->get("lights")->use();

	glm::mat4 view = CameraManager::instance()->get("camera")->GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(CameraManager::instance()->get("camera")->Zoom), (float)width / (float)height, 0.1f, 100.0f);

	glm::mat4 model;
	ShaderManager::instance()->get("lights")->setUniform("view", view);
	ShaderManager::instance()->get("lights")->setUniform("projection", projection);

	for (unsigned int i = 0; i < 4; i++)
	{
		model = glm::mat4();
		model = glm::translate(model, pointLightPositions[i]);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		ShaderManager::instance()->get("lights")->setUniform("model", model);
		VertexManager::instance()->get("squareEBO")->draw();
	}

	if (!glass) {
		ShaderManager::instance()->get("model")->use();
		ShaderManager::instance()->get("model")->setUniform("view", view);
		ShaderManager::instance()->get("model")->setUniform("projection", projection);
		model = glm::mat4();
		ShaderManager::instance()->get("model")->setUniform("model", model);

		ShaderManager::instance()->get("model")->setUniform("material.shininess", 64.0f);
		ShaderManager::instance()->get("model")->setUniform("viewPos", CameraManager::instance()->get("camera")->Position);
		ModelManager::instance()->get("nanosuit")->Draw("model");
	}
	else {
		//GLASS MODEL
		if (!refract) {
			ShaderManager::instance()->get("cube_reflector")->use();
			ShaderManager::instance()->get("cube_reflector")->setUniform("view", view);
			ShaderManager::instance()->get("cube_reflector")->setUniform("projection", projection);
			model = glm::mat4();
			ShaderManager::instance()->get("cube_reflector")->setUniform("model", model);

			ShaderManager::instance()->get("cube_reflector")->setUniform("cameraPos", CameraManager::instance()->get("camera")->Position);
			ModelManager::instance()->get("nanosuit")->Draw("sky");
		}
		else {
			ShaderManager::instance()->get("cube_refractor")->use();
			ShaderManager::instance()->get("cube_refractor")->setUniform("view", view);
			ShaderManager::instance()->get("cube_refractor")->setUniform("projection", projection);
			model = glm::mat4();
			ShaderManager::instance()->get("cube_refractor")->setUniform("model", model);

			ShaderManager::instance()->get("cube_refractor")->setUniform("cameraPos", CameraManager::instance()->get("camera")->Position);
			ModelManager::instance()->get("nanosuit")->Draw("sky");
		}
	}

	//SKYBOX
	glDepthFunc(GL_LEQUAL);
	ShaderManager::instance()->get("sky")->use();
	view = glm::mat4(glm::mat3(CameraManager::instance()->get("camera")->GetViewMatrix()));
	projection = glm::perspective(glm::radians(CameraManager::instance()->get("camera")->Zoom), (float)width / (float)height, 0.1f, 100.0f);
	ShaderManager::instance()->get("sky")->setUniform("view", view);
	ShaderManager::instance()->get("sky")->setUniform("projection", projection);
	VertexManager::instance()->get("sky")->draw("sky", GL_TRIANGLES, true);
	glDepthFunc(GL_LESS);
}