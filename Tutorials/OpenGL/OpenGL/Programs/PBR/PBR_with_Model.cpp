#include "PBR.h"

int main() {
	initializeGLFW();
	createWindow();
	initializeGLAD();
	initializeDebug();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	setCallbacks();

	//Camera
	Camera *camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	CameraManager::instance()->add("camera", camera);

	setShader();
	setVertices();
	setTextures();
	setModels();
	setFramebuffers();
	setPointLights();

	glViewport(0, 0, width, height);
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
	glfwWindowHint(GLFW_SAMPLES, 4);
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
	Shader* pbrShader = new Shader("Shaders/PBR/pbr.vs", "Shaders/PBR/pbr_textures.fs");
	ShaderManager::instance()->add("pbrShader", pbrShader);

	Shader* equirectangularToCubemapShader = new Shader("Shaders/PBR/cubemap.vs", "Shaders/PBR/equirectangular_to_cubemap.fs");
	ShaderManager::instance()->add("equirectangularToCubemapShader", equirectangularToCubemapShader);

	Shader* backgroundShader = new Shader("Shaders/PBR/background.vs", "Shaders/PBR/background.fs");
	ShaderManager::instance()->add("backgroundShader", backgroundShader);

	Shader* irradianceShader = new Shader("Shaders/PBR/cubemap.vs", "Shaders/PBR/irradiance_convolution.fs");
	ShaderManager::instance()->add("irradianceShader", irradianceShader);

	Shader* prefilterShader = new Shader("Shaders/PBR/cubemap.vs", "Shaders/PBR/prefilter.fs");
	ShaderManager::instance()->add("prefilterShader", prefilterShader);

	Shader* brdfShader = new Shader("Shaders/PBR/brdf.vs", "Shaders/PBR/brdf.fs");
	ShaderManager::instance()->add("brdfShader", brdfShader);

	ShaderManager::instance()->get("pbrShader")->use();
	ShaderManager::instance()->get("pbrShader")->setUniform("irradianceMap", 0);
	ShaderManager::instance()->get("pbrShader")->setUniform("prefilterMap", 1);
	ShaderManager::instance()->get("pbrShader")->setUniform("brdfLUT", 2);
	ShaderManager::instance()->get("pbrShader")->setUniform("albedoMap", 3);
	ShaderManager::instance()->get("pbrShader")->setUniform("normalMap", 4);
	ShaderManager::instance()->get("pbrShader")->setUniform("metallicMap", 5);
	ShaderManager::instance()->get("pbrShader")->setUniform("roughnessMap", 6);
	ShaderManager::instance()->get("pbrShader")->setUniform("aoMap", 7);

	ShaderManager::instance()->get("backgroundShader")->use();
	ShaderManager::instance()->get("backgroundShader")->setUniform("environmentMap", 0);

	ShaderManager::instance()->get("equirectangularToCubemapShader")->use();
	ShaderManager::instance()->get("equirectangularToCubemapShader")->setUniform("equirectangularMap", 0);
	ShaderManager::instance()->get("equirectangularToCubemapShader")->setUniform("projection", captureProjection);

	ShaderManager::instance()->get("irradianceShader")->use();
	ShaderManager::instance()->get("irradianceShader")->setUniform("environmentMap", 0);
	ShaderManager::instance()->get("irradianceShader")->setUniform("projection", captureProjection);

	ShaderManager::instance()->get("prefilterShader")->use();
	ShaderManager::instance()->get("prefilterShader")->setUniform("environmentMap", 0);
	ShaderManager::instance()->get("prefilterShader")->setUniform("projection", captureProjection);
}

void setTextures() {
	TextureLoader *hdr = new TextureLoader();
	hdr->loadHDR("Images/skyboxes/Newport_Loft/Newport_Loft_Ref.hdr");
	TextureManager::instance()->add("hdr", hdr);

	TextureLoader *envCubeMap = new TextureLoader(1, true);
	envCubeMap->setCubeMap(512, 512, true, false);
	TextureManager::instance()->add("envCubeMap", envCubeMap);

	TextureLoader *irradianceMap = new TextureLoader(1, true);
	irradianceMap->setCubeMap(32, 32);
	TextureManager::instance()->add("irradianceMap", irradianceMap);

	TextureLoader *prefilterMap = new TextureLoader(1, true);
	prefilterMap->setCubeMap(128, 128, true, true);
	TextureManager::instance()->add("prefilterMap", prefilterMap);

	TextureLoader *brdfLUTTexture = new TextureLoader();
	brdfLUTTexture->addTexture(true, 512, 512, GL_RG16F, GL_RG, GL_FLOAT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	TextureManager::instance()->add("brdfLUTTexture", brdfLUTTexture);

	TextureLoader *pistolAlbedoMap = new TextureLoader();
	pistolAlbedoMap->load("Models/Cerberus_by_Andrew_Maximov/Textures/Cerberus_A.tga");
	TextureManager::instance()->add("pistolAlbedoMap", pistolAlbedoMap);
		
	TextureLoader *pistolNormalMap = new TextureLoader();
	pistolNormalMap->load("Models/Cerberus_by_Andrew_Maximov/Textures/Cerberus_N.tga");
	TextureManager::instance()->add("pistolNormalMap", pistolNormalMap);
		
	TextureLoader *pistolMetallicMap = new TextureLoader();
	pistolMetallicMap->load("Models/Cerberus_by_Andrew_Maximov/Textures/Cerberus_M.tga");
	TextureManager::instance()->add("pistolMetallicMap", pistolMetallicMap);
		
	TextureLoader *pistolRoughnessMap = new TextureLoader();
	pistolRoughnessMap->load("Models/Cerberus_by_Andrew_Maximov/Textures/Cerberus_R.tga");
	TextureManager::instance()->add("pistolRoughnessMap", pistolRoughnessMap);
		
	TextureLoader *pistolAOMap = new TextureLoader();
	pistolAOMap->load("Models/Cerberus_by_Andrew_Maximov/Textures/Raw/Cerberus_AO.tga");
	TextureManager::instance()->add("pistolAOMap", pistolAOMap);
}

void setVertices() {
	VertexBuffers *sphere = new VertexBuffers();
	sphere->initializeSphere();
	VertexManager::instance()->add("sphere", sphere);

	VertexBuffers *vertex2;
	vertex2 = new VertexBuffers(cubeWithMaps, true, true);
	VertexManager::instance()->add("cube", vertex2);

	VertexBuffers *v3 = new VertexBuffers(hdrQuad, false, true);
	VertexManager::instance()->add("quad", v3);
}

void setFramebuffers() {
	FrameBuffer *f1 = new FrameBuffer();
	f1->createRBO(512, 512, false);
	FrameBufferManager::instance()->add("captureFrameBuffer", f1);

	ShaderManager::instance()->get("equirectangularToCubemapShader")->use();

	TextureManager::instance()->get("hdr")->bind(0);

	glViewport(0, 0, 512, 512);

	FrameBufferManager::instance()->get("captureFrameBuffer")->bind();
	for (unsigned int i = 0; i < 6; ++i)
	{
		ShaderManager::instance()->get("equirectangularToCubemapShader")->setUniform("view", captureViews[i]);
		FrameBufferManager::instance()->get("captureFrameBuffer")->attachTexture2D("envCubeMap", GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		VertexManager::instance()->get("cube")->draw();
	}
	FrameBufferManager::instance()->unbind();

	// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
	TextureManager::instance()->get("envCubeMap")->bind(-1, -1, true);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	FrameBufferManager::instance()->get("captureFrameBuffer")->bind();
	FrameBufferManager::instance()->get("captureFrameBuffer")->setRBO(32, 32, false);

	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
	// -----------------------------------------------------------------------------

	ShaderManager::instance()->get("irradianceShader")->use();

	TextureManager::instance()->get("envCubeMap")->bind(0, -1, true);

	glViewport(0, 0, 32, 32);
	FrameBufferManager::instance()->get("captureFrameBuffer")->bind();
	for (unsigned int i = 0; i < 6; ++i)
	{
		ShaderManager::instance()->get("irradianceShader")->setUniform("view", captureViews[i]);
		FrameBufferManager::instance()->get("captureFrameBuffer")->attachTexture2D("irradianceMap", GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		VertexManager::instance()->get("cube")->draw();
	}
	FrameBufferManager::instance()->unbind();

	ShaderManager::instance()->get("prefilterShader")->use();

	TextureManager::instance()->get("envCubeMap")->bind(0, -1, true);

	FrameBufferManager::instance()->get("captureFrameBuffer")->bind();
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// resize framebuffer according to mip-level size.
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		FrameBufferManager::instance()->get("captureFrameBuffer")->setRBO(mipWidth, mipHeight, false);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);

		ShaderManager::instance()->get("prefilterShader")->setUniform("roughness", roughness);

		for (unsigned int i = 0; i < 6; ++i)
		{
			ShaderManager::instance()->get("prefilterShader")->setUniform("view", captureViews[i]);

			FrameBufferManager::instance()->get("captureFrameBuffer")->attachTexture2D("prefilterMap", GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			VertexManager::instance()->get("cube")->draw();
		}
	}
	FrameBufferManager::instance()->unbind();

	FrameBufferManager::instance()->get("captureFrameBuffer")->bind();
	FrameBufferManager::instance()->get("captureFrameBuffer")->setRBO(512, 512, false);
	FrameBufferManager::instance()->get("captureFrameBuffer")->attachTexture2D("brdfLUTTexture");

	glViewport(0, 0, 512, 512);
	ShaderManager::instance()->get("brdfShader")->use();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	VertexManager::instance()->get("quad")->draw(GL_TRIANGLE_STRIP);

	FrameBufferManager::instance()->unbind();
}

void setModels() {
	Model *pistol = new Model("Models/Cerberus_by_Andrew_Maximov/Cerberus_LP.FBX");
	ModelManager::instance()->add("pistol", pistol);
}

void drawObjects() {
	glm::mat4 projection = glm::perspective(CameraManager::instance()->get("camera")->Zoom, (float)width / (float)height, 0.1f, 100.0f);
	ShaderManager::instance()->get("pbrShader")->use();
	ShaderManager::instance()->get("pbrShader")->setUniform("projection", projection);
	ShaderManager::instance()->get("backgroundShader")->use();
	ShaderManager::instance()->get("backgroundShader")->setUniform("projection", projection);

	ShaderManager::instance()->get("pbrShader")->use();
	glm::mat4 view = CameraManager::instance()->get("camera")->GetViewMatrix();
	ShaderManager::instance()->get("pbrShader")->setUniform("view", view);
	ShaderManager::instance()->get("pbrShader")->setUniform("camPos", CameraManager::instance()->get("camera")->Position);

	TextureManager::instance()->get("irradianceMap")->bind(0, -1, true);
	TextureManager::instance()->get("prefilterMap")->bind(1, -1, true);
	TextureManager::instance()->get("brdfLUTTexture")->bind(2);

	TextureManager::instance()->get("pistolAlbedoMap")->bind(3);
	TextureManager::instance()->get("pistolNormalMap")->bind(4);
	TextureManager::instance()->get("pistolMetallicMap")->bind(5);
	TextureManager::instance()->get("pistolRoughnessMap")->bind(6);
	TextureManager::instance()->get("pistolAOMap")->bind(7);

	glm::mat4 model;
	model = glm::mat4();
	model = glm::scale(model, glm::vec3(0.1f));
	model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(0.0, 0.0, 1.0)));
	model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
	ShaderManager::instance()->get("pbrShader")->setUniform("model", model);
	ModelManager::instance()->get("pistol")->Draw("pbrShader");

	// render light source (simply re-render sphere at light positions)
	// this looks a bit off as we use the same shader, but it'll make their positions obvious and 
	// keeps the codeprint small.
	for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
	{
		glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
		newPos = lightPositions[i];
		
		updatePointLights(newPos, i);

		model = glm::mat4();
		model = glm::translate(model, newPos);
		model = glm::scale(model, glm::vec3(0.5f));
		ShaderManager::instance()->get("pbrShader")->setUniform("model", model);
		VertexManager::instance()->get("sphere")->draw(GL_TRIANGLE_STRIP);
	}

	// render skybox (render as last to prevent overdraw)
	ShaderManager::instance()->get("backgroundShader")->use();

	ShaderManager::instance()->get("backgroundShader")->setUniform("view", view);
	TextureManager::instance()->get("envCubeMap")->bind(0, -1, true);
	VertexManager::instance()->get("cube")->draw();

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void setPointLights() {
	for (int i = 0; i < 4; i++) {
		Light *p = new Point_Light("pbrShader", pointLightPositions[i], lightColors[i], i);
		LightManager::instance()->add("pointLight" + std::to_string(i), p);
	}
}

void updatePointLights(glm::vec3 newPos, int i) {
	Point_Light *p = (Point_Light *)LightManager::instance()->get("pointLight" + std::to_string(i));
	p->setPosition(newPos);
}