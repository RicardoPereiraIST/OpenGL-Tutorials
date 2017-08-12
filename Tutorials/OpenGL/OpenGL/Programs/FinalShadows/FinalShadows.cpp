#include "FinalShadows.h"

int main() {
	initializeGLFW();
	createWindow();
	initializeGLAD();
	initializeDebug();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glViewport(0, 0, width, height);
	setCallbacks();

	//Camera
	Camera *camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	CameraManager::instance()->add("camera", camera);

	setShader();
	setVertices();
	setTextures();

	setFrameBuffers();

	ShaderManager::instance()->get("shadowCube")->use();
	for (unsigned int i = 0; i < 20; i++) {
		ShaderManager::instance()->get("shadowCube")->setUniform(("sampleOffsetDirections[" + std::to_string(i) + "]").c_str(), sampleOffsetDirections[i]);
	}

	setLights();
	setShadows();

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

		pointLightPos.z = sin(glfwGetTime() * 0.5) * 3.0;

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		updateSpotLight();
		updatePointLights();
		updateShadows();
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

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
	{
		blinn = !blinn;
		blinnKeyPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
	{
		blinnKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !pre_samplerKeyPressed)
	{
		pre_sampler = !pre_sampler;
		pre_samplerKeyPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
	{
		pre_samplerKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !debugKeyPressed)
	{
		debug = !debug;
		debugKeyPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE)
	{
		debugKeyPressed = false;
	}
}

void setShader() {
	Shader* depth_shadow = new Shader("Shaders/FinalShadows/depth_shadow.vs", "Shaders/FinalShadows/depth_shadow.fs");
	ShaderManager::instance()->add("depth_shadow", depth_shadow);

	Shader* debug_quad = new Shader("Shaders/FinalShadows/debug_quad.vs", "Shaders/FinalShadows/debug_quad.fs");
	ShaderManager::instance()->add("debug_quad", debug_quad);

	Shader* shadow = new Shader("Shaders/FinalShadows/shadow_mapping.vs", "Shaders/FinalShadows/shadow_mapping.fs");
	ShaderManager::instance()->add("shadow", shadow);

	Shader* depth_cube = new Shader("Shaders/FinalShadows/point_depth_shadow.vs", "Shaders/FinalShadows/point_depth_shadow.fs", "Shaders/FinalShadows/point_depth_shadow.gs");
	ShaderManager::instance()->add("depth_cube", depth_cube);

	Shader* shadowCube = new Shader("Shaders/FinalShadows/shadow_cube.vs", "Shaders/FinalShadows/shadow_cube.fs");
	ShaderManager::instance()->add("shadowCube", shadowCube);

	Shader* finalS = new Shader("Shaders/FinalShadows/final_shadows.vs", "Shaders/FinalShadows/final_shadows.fs");
	ShaderManager::instance()->add("finalS", finalS);

	ShaderManager::instance()->get("debug_quad")->use();
	ShaderManager::instance()->get("debug_quad")->setUniform("depthMap", 0);

	ShaderManager::instance()->get("shadow")->use();
	ShaderManager::instance()->get("shadow")->setUniform("diffuseTexture", 0);
	ShaderManager::instance()->get("shadow")->setUniform("shadowMap", 1);

	ShaderManager::instance()->get("shadowCube")->use();
	ShaderManager::instance()->get("shadowCube")->setUniform("diffuseTexture", 0);
	ShaderManager::instance()->get("shadowCube")->setUniform("depthMap", 1);

	ShaderManager::instance()->get("finalS")->use();
	ShaderManager::instance()->get("finalS")->setUniform("diffuseTexture", 0);
	ShaderManager::instance()->get("finalS")->setUniform("shadowMap", 1);
	ShaderManager::instance()->get("finalS")->setUniform("depthMap", 2);
	ShaderManager::instance()->get("finalS")->setUniform("shadowMap2", 3);
}

void setTextures() {
	TextureLoader *texture1 = new TextureLoader();
	texture1->load("Images/floor/vector-wood-texture.jpg");
	TextureManager::instance()->add("plane", texture1);

	TextureLoader *shadowMap = new TextureLoader();
	shadowMap->loadShadowMap(SHADOW_WIDTH, SHADOW_HEIGHT);
	TextureManager::instance()->add("shadowMap", shadowMap);

	TextureLoader *shadowMap2 = new TextureLoader();
	shadowMap2->loadShadowMap(SHADOW_WIDTH, SHADOW_HEIGHT);
	TextureManager::instance()->add("shadowMap2", shadowMap2);

	TextureLoader *shadowCube = new TextureLoader(1, true);
	shadowCube->loadShadowCube(SHADOW_WIDTH, SHADOW_HEIGHT);
	TextureManager::instance()->add("shadowCube", shadowCube);
}

void setVertices() {
	VertexBuffers *vertex4;
	vertex4 = new VertexBuffers(quadVertices, false, true, 2);
	VertexManager::instance()->add("screen", vertex4);

	VertexBuffers *cube;
	cube = new VertexBuffers(shadowVertices, true, true);
	VertexManager::instance()->add("cube", cube);

	VertexBuffers *planeShadow;
	planeShadow = new VertexBuffers(planeShadowVertices, true, true);
	VertexManager::instance()->add("planeShadow", planeShadow);
}

void drawObjects() {

	//dir depth map
	ShaderManager::instance()->get("depth_shadow")->use();
	ShaderManager::instance()->get("depth_shadow")->setUniform("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	FrameBufferManager::instance()->get("shadowMap")->bind();
	glClear(GL_DEPTH_BUFFER_BIT);

	glCullFace(GL_FRONT);

	renderScene("depth_shadow", "plane", true);

	FrameBufferManager::instance()->unbind();

	glCullFace(GL_BACK);

	//spot depth cube
	ShaderManager::instance()->get("depth_shadow")->use();
	ShaderManager::instance()->get("depth_shadow")->setUniform("lightSpaceMatrix", spotSpaceMatrix);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	FrameBufferManager::instance()->get("shadowMap2")->bind();
	glClear(GL_DEPTH_BUFFER_BIT);

	glCullFace(GL_FRONT);

	renderScene("depth_shadow", "plane", true);

	FrameBufferManager::instance()->unbind();

	glCullFace(GL_BACK);

	//point depth cube
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	FrameBufferManager::instance()->get("shadowCube")->bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	ShaderManager::instance()->get("depth_cube")->use();
	for (unsigned int i = 0; i < 6; ++i)
		ShaderManager::instance()->get("depth_cube")->setUniform(("shadowMatrices[" + std::to_string(i) + "]").c_str(), shadowTransforms[i]);
	ShaderManager::instance()->get("depth_cube")->setUniform("far_plane", f);
	ShaderManager::instance()->get("depth_cube")->setUniform("lightPos", pointLightPos);
	renderScene("depth_cube", "");
	FrameBufferManager::instance()->unbind();

	//final render
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ShaderManager::instance()->get("finalS")->use();
	glm::mat4 projection = glm::perspective(CameraManager::instance()->get("camera")->Zoom, (float)width / (float)height, 0.1f, 100.0f);
	glm::mat4 view = CameraManager::instance()->get("camera")->GetViewMatrix();
	ShaderManager::instance()->get("finalS")->setUniform("projection", projection);
	ShaderManager::instance()->get("finalS")->setUniform("view", view);
	// set light uniforms
	ShaderManager::instance()->get("finalS")->setUniform("viewPos", CameraManager::instance()->get("camera")->Position);
	ShaderManager::instance()->get("finalS")->setUniform("lightSpaceMatrix", lightSpaceMatrix);
	ShaderManager::instance()->get("finalS")->setUniform("spotSpaceMatrix", spotSpaceMatrix);
	ShaderManager::instance()->get("finalS")->setUniform("blinn", blinn);
	if (blinn)
		ShaderManager::instance()->get("finalS")->setUniform("material.shininess", 64.0f);
	else
		ShaderManager::instance()->get("finalS")->setUniform("material.shininess", 16.0f);
	ShaderManager::instance()->get("finalS")->setUniform("far_plane", f);
	ShaderManager::instance()->get("finalS")->setUniform("pre_sampler", pre_sampler);

	TextureManager::instance()->get("plane")->bind(0);
	TextureManager::instance()->get("shadowMap")->bind(1);
	TextureManager::instance()->get("shadowCube")->bind(2, -1, true);
	TextureManager::instance()->get("shadowMap2")->bind(3);

	renderScene("finalS", "");

	// render Depth map to quad for visual debugging
	// ---------------------------------------------
	if (debug) {
		ShaderManager::instance()->get("debug_quad")->use();
		ShaderManager::instance()->get("debug_quad")->setUniform("near_plane", near_plane);
		ShaderManager::instance()->get("debug_quad")->setUniform("far_plane", far_plane);
		TextureManager::instance()->get("shadowMap")->bind(0);
		VertexManager::instance()->get("screen")->draw();
	}
}

void setLights() {
	Light *d2 = new Directional_Light("finalS", glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.3f, 0.3f, 0.3f), lightPos);
	LightManager::instance()->add("dirLight2", d2);

	Light *pl2 = new Point_Light("finalS", glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.3f, 0.3f, 0.3f), pointLightPos, 1.0f, 0.09f, 0.032f, 0);
	LightManager::instance()->add("pointLight1", pl2);

	Light *sl = new Spotlight("finalS", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), spotPos, spotDir, 1.0f, 0.09f, 0.032f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)));
	LightManager::instance()->add("spotLight", sl);
}

void updatePointLights() {
	Point_Light *pl2 = (Point_Light *)LightManager::instance()->get("pointLight1");
	pl2->setPosition(pointLightPos);
}

void updateSpotLight() {
	Spotlight *s1 = (Spotlight *)LightManager::instance()->get("spotLight");
	s1->setPosition(spotPos);
	s1->setDirection(spotDir);
}

void setFrameBuffers() {
	FrameBuffer * fb1 = new FrameBuffer("shadowCube", true);
	FrameBufferManager::instance()->add("shadowCube", fb1);

	FrameBuffer * fb2 = new FrameBuffer("shadowMap");
	FrameBufferManager::instance()->add("shadowMap", fb2);

	FrameBuffer * fb3 = new FrameBuffer("shadowMap2");
	FrameBufferManager::instance()->add("shadowMap2", fb3);
}

void renderScene(std::string shaderName, std::string textureName, bool depth)
{
	glDisable(GL_CULL_FACE);
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	ShaderManager::instance()->get(shaderName)->setUniform("model", model);
	draw("planeShadow", textureName);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(10.0f, 0.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.5f));
	ShaderManager::instance()->get(shaderName)->setUniform("model", model);
	draw("cube", textureName);

	if(depth)
		glEnable(GL_CULL_FACE);

	// room cube
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
	model = glm::scale(model, glm::vec3(5.0f));
	ShaderManager::instance()->get(shaderName)->setUniform("model", model);
		// note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
	ShaderManager::instance()->get(shaderName)->setUniform("reverse_normals", 1); // A small little hack to invert normals when drawing cube from the inside so lighting still works.
	draw("cube", textureName);

	ShaderManager::instance()->get(shaderName)->setUniform("reverse_normals", 0); // and of course disable it

	if (depth) return;

	// cubes
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0));
	model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	ShaderManager::instance()->get(shaderName)->setUniform("model", model);
	draw("cube", textureName);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0));
	model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.75f));
	ShaderManager::instance()->get(shaderName)->setUniform("model", model);
	draw("cube", textureName);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0));
	model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	ShaderManager::instance()->get(shaderName)->setUniform("model", model);
	draw("cube", textureName);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5));
	model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	ShaderManager::instance()->get(shaderName)->setUniform("model", model);
	draw("cube", textureName);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0));
	model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.75f));
	ShaderManager::instance()->get(shaderName)->setUniform("model", model);
	draw("cube", textureName);
}

void draw(std::string vertexBuffer, std::string textureName) {
	if (textureName == "")
		VertexManager::instance()->get(vertexBuffer)->draw();
	else
		VertexManager::instance()->get(vertexBuffer)->draw(textureName);
}

void setShadows() {
	//dir
	glm::mat4 lightProjection, lightView;
	float near_plane = 1.0f, far_plane = 7.5f;
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = glm::mat4();
	lightSpaceMatrix = lightProjection * lightView;

	//spot
	Spotlight *s1 = (Spotlight*)LightManager::instance()->get("spotLight");
	glm::mat4 depthProjectionMatrix = glm::perspective(glm::acos(s1->cutOff) * 2, aspect, 3.1f, f);
	glm::mat4 depthViewMatrix = glm::lookAt(s1->position, s1->position + s1->direction, CameraManager::instance()->get("camera")->Up);
	spotSpaceMatrix = glm::mat4();
	spotSpaceMatrix = depthProjectionMatrix * depthViewMatrix;

	//point
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, n, f);
	shadowTransforms = std::vector<glm::mat4>();
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(pointLightPos, pointLightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(pointLightPos, pointLightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
}

void updateShadows() {
	//point
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, n, f);
	shadowTransforms = std::vector<glm::mat4>();
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(pointLightPos, pointLightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(pointLightPos, pointLightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
}