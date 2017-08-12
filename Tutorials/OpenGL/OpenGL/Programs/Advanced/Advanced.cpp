#include "Advanced.h"

int main() {

	initializeGLFW();
	createWindow();
	initializeGLAD();
	initializeDebug();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//STENCIL
	if (stencil) {
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	}

	if (blend) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
	}

	if (face_cull) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}

	glViewport(0, 0, width, height);
	setCallbacks();

	//Camera
	Camera *camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	CameraManager::instance()->add("camera", camera);
	
	setShader();
	setVertices();
	setTextures();

	if (framebuffer) {
		setFrameBuffers();
	}

	setWindows();

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

	if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !wireframeKeyPressed) {
		wireframe = !wireframe;
		wireframeKeyPressed = true;
		glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
	}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
		wireframeKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !trueFpsKeyPressed) {
		trueFps = !trueFps;
		trueFpsKeyPressed = true;
		CameraManager::instance()->get("camera")->true_fps = trueFps;
	}

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
		trueFpsKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blurKeyPressed) {
		blurScreen = !blurScreen;
		blurKeyPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) {
		blurKeyPressed = false;
	}

}

void setShader() {
	Shader* shader = new Shader("Shaders/Advanced/depth.vs", "Shaders/Advanced/depth.fs");
	ShaderManager::instance()->add("original", shader);

	Shader *shader2 = new Shader("Shaders/Advanced/shader_single_color.vs", "Shaders/Advanced/shader_single_color.fs");
	ShaderManager::instance()->add("color", shader2);

	Shader* shader3 = new Shader("Shaders/Advanced/fbo_screen.vs", "Shaders/Advanced/fbo_screen.fs");
	ShaderManager::instance()->add("screen", shader3);

	Shader* shader4 = new Shader("Shaders/Advanced/skybox.vs", "Shaders/Advanced/skybox.fs");
	ShaderManager::instance()->add("sky", shader4);

	Shader* shader5 = new Shader("Shaders/Advanced/reflect_cubemap.vs", "Shaders/Advanced/reflect_cubemap.fs");
	ShaderManager::instance()->add("cube_reflector", shader5);
	
	Shader* shader6 = new Shader("Shaders/Advanced/fbo_screen.vs", "Shaders/Others (random shaders)/Post-Processing/post_blur.fs");
	ShaderManager::instance()->add("blur", shader6);

	//SET KERNEL FOR SOME POST EFFECTS
	ShaderManager::instance()->get("blur")->use();
	for(int i = 0; i < 9; i++)
		ShaderManager::instance()->get("blur")->setUniform(("kernel[" + std::to_string(i) + "]").c_str(), blur[i]);

	ShaderManager::instance()->get("original")->use();
	ShaderManager::instance()->get("original")->setUniform("texture1", 0);

	ShaderManager::instance()->get("sky")->use();
	ShaderManager::instance()->get("sky")->setUniform("skybox", 0);

	if (framebuffer) {
		ShaderManager::instance()->get("screen")->use();
		ShaderManager::instance()->get("screen")->setUniform("screenTexture", 0);

		ShaderManager::instance()->get("blur")->use();
		ShaderManager::instance()->get("blur")->setUniform("screenTexture", 0);
	}
	
}

void setTextures() {
	TextureLoader *texture1 = new TextureLoader();
	texture1->load("Images/containers/container2.png");
	TextureManager::instance()->add("container", texture1);

	TextureLoader *texture2 = new TextureLoader();
	texture2->load("Images/floor/floor.jpg");
	TextureManager::instance()->add("floor", texture2);

	TextureLoader *texture3 = new TextureLoader();
	texture3->load("Images/windows/blending_transparent_window.png", false, GL_CLAMP_TO_EDGE);
	TextureManager::instance()->add("window", texture3);

	TextureLoader *texture4 = new TextureLoader(1, true);
	texture4->loadCubeMap(faces);
	TextureManager::instance()->add("sky", texture4);

	if (framebuffer) {
		TextureLoader *texture4 = new TextureLoader();
		texture4->loadFrame(width, height);
		TextureManager::instance()->add("screen", texture4);
	}
}

void setVertices() {
	VertexBuffers *vertex;
	vertex = new VertexBuffers(cubeVerticesFaceCullCCW, false, true);
	VertexManager::instance()->add("cube", vertex);

	VertexBuffers *vertex2;
	vertex2 = new VertexBuffers(planeVertices, false, true);
	VertexManager::instance()->add("plane", vertex2);

	VertexBuffers *vertex3;
	vertex3 = new VertexBuffers(transparentVertices, false, true);
	VertexManager::instance()->add("window", vertex3);

	VertexBuffers *vertex4;
	vertex4 = new VertexBuffers(quadVertices, false, true, 2);
	VertexManager::instance()->add("screen", vertex4);

	VertexBuffers *vertex5;
	vertex5 = new VertexBuffers(skyboxVertices, false, false);
	VertexManager::instance()->add("sky", vertex5);

	VertexBuffers *vertex6;
	vertex6 = new VertexBuffers(verticesWithNormal, true, false);
	VertexManager::instance()->add("cube_reflector", vertex6);
}

void setWindows() {
	windows.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
	windows.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
	windows.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
	windows.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
	windows.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

	for (unsigned int i = 0; i < windows.size(); i++)
	{
		float distance = glm::length(CameraManager::instance()->get("camera")->Position - windows[i]);
		sorted[distance] = windows[i];
	}
}

void drawObjects() {

	if (framebuffer) {
		FrameBufferManager::instance()->get("screen")->bind();
	}
	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

							 // make sure we clear the framebuffer's content
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


	//FLOOR
	ShaderManager::instance()->get("original")->use();
	glm::mat4 view = CameraManager::instance()->get("camera")->GetViewMatrix();
	glm::mat4 projection = glm::perspective(CameraManager::instance()->get("camera")->Zoom, (float)width / (float)height, 0.1f, 100.0f);
	glm::mat4 model;
	ShaderManager::instance()->get("original")->setUniform("view", view);
	ShaderManager::instance()->get("original")->setUniform("projection", projection);

	model = glm::mat4();
	ShaderManager::instance()->get("original")->setUniform("model", model);

	if (stencil)
		glStencilMask(0x00);

	VertexManager::instance()->get("plane")->draw("floor");

	if (face_cull) {
		glEnable(GL_CULL_FACE);
	}

	//CONTAINERS
	if (stencil) {
		glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should update the stencil buffer
		glStencilMask(0xFF); // enable writing to the stencil buffer
	}

	TextureManager::instance()->get("container")->bind(0);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
	ShaderManager::instance()->get("original")->setUniform("model", model);
	VertexManager::instance()->get("cube")->draw();

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
	ShaderManager::instance()->get("original")->setUniform("model", model);
	VertexManager::instance()->get("cube")->draw();

	//Reflector cube
	if (sky) {
		ShaderManager::instance()->get("cube_reflector")->use();
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-2.5f, 0.0f, -2.5f));
		ShaderManager::instance()->get("cube_reflector")->setUniform("cameraPos", CameraManager::instance()->get("camera")->Position);
		ShaderManager::instance()->get("cube_reflector")->setUniform("model", model);
		ShaderManager::instance()->get("cube_reflector")->setUniform("view", view);
		ShaderManager::instance()->get("cube_reflector")->setUniform("projection", projection);
		VertexManager::instance()->get("cube_reflector")->draw("sky", GL_TRIANGLES, true);

		ShaderManager::instance()->get("original")->use();
	}

	if (face_cull) {
		glDisable(GL_CULL_FACE);
	}

	//WINDOWS (NO OUTLINE ON THEM - STENCIL)
	TextureManager::instance()->get("window")->bind(0);

	for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
	{
		model = glm::mat4();
		model = glm::translate(model, it->second);
		ShaderManager::instance()->get("original")->setUniform("model", model);
		VertexManager::instance()->get("window")->draw();
	}


	if (stencil) {
		//SCALED UP CONTAINERS
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		ShaderManager::instance()->get("color")->use();

		ShaderManager::instance()->get("color")->setUniform("view", view);
		ShaderManager::instance()->get("color")->setUniform("projection", projection);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		model = glm::scale(model, glm::vec3(1.1f));
		ShaderManager::instance()->get("color")->setUniform("model", model);

		VertexManager::instance()->get("cube")->draw();

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.1f));
		ShaderManager::instance()->get("color")->setUniform("model", model);

		VertexManager::instance()->get("cube")->draw();

		glStencilMask(0xFF);
		glEnable(GL_DEPTH_TEST);
	}

	//SKYBOX
	if (sky) {
		glDepthFunc(GL_LEQUAL);
		ShaderManager::instance()->get("sky")->use();
		view = glm::mat4(glm::mat3(CameraManager::instance()->get("camera")->GetViewMatrix()));
		projection = glm::perspective(glm::radians(CameraManager::instance()->get("camera")->Zoom), (float)width / (float)height, 0.1f, 100.0f);
		ShaderManager::instance()->get("sky")->setUniform("view", view);
		ShaderManager::instance()->get("sky")->setUniform("projection", projection);
		VertexManager::instance()->get("sky")->draw("sky", GL_TRIANGLES, true);
		glDepthFunc(GL_LESS);
	}

	//FRAMEBUFFER
	if (framebuffer) {
		FrameBufferManager::instance()->unbind();
		glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
								  // clear all relevant buffers
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		ShaderManager::instance()->get(blurScreen ? "blur" : "screen")->use();
		VertexManager::instance()->get("screen")->draw("screen");
	}
}

void setFrameBuffers() {
	FrameBuffer * fb1 = new FrameBuffer("screen", width, height);
	FrameBufferManager::instance()->add("screen", fb1);
}