#include "PBR.h"

int main() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	setShader();
	setVertices();
	setTextures();
	setFramebuffers();
	setPointLights();

	glViewport(0, 0, WIDTH, HEIGHT);
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
	// rusted iron
	TextureLoader *ironAlbedoMap = new TextureLoader();
	ironAlbedoMap->load("Images/PBR/rustediron1-alt2-Unreal-Engine/rustediron2_basecolor.png");
	TextureManager::instance()->add("ironAlbedoMap", ironAlbedoMap);
	
	TextureLoader *ironNormalMap = new TextureLoader();
	ironNormalMap->load("Images/PBR/rustediron1-alt2-Unreal-Engine/rustediron2_metallic.png");
	TextureManager::instance()->add("ironNormalMap", ironNormalMap);
	
	TextureLoader *ironMetallicMap = new TextureLoader();
	ironMetallicMap->load("Images/PBR/rustediron1-alt2-Unreal-Engine/rustediron2_normal.png");
	TextureManager::instance()->add("ironMetallicMap", ironMetallicMap);
	
	TextureLoader *ironRoughnessMap = new TextureLoader();
	ironRoughnessMap->load("Images/PBR/rustediron1-alt2-Unreal-Engine/rustediron2_roughness.png");
	TextureManager::instance()->add("ironRoughnessMap", ironRoughnessMap);
	
	TextureLoader *ironAOMap = new TextureLoader();
	ironAOMap->load("Images/PBR/rustediron1-alt2-Unreal-Engine/rustediron2_ao.png");
	TextureManager::instance()->add("ironAOMap", ironAOMap);


	//Gold
	TextureLoader *goldAlbedoMap = new TextureLoader();
	goldAlbedoMap->load("Images/PBR/gold-scuffed-Unreal-Engine/gold-scuffed_basecolor.png");
	TextureManager::instance()->add("goldAlbedoMap", goldAlbedoMap);

	TextureLoader *goldNormalMap = new TextureLoader();
	goldNormalMap->load("Images/PBR/gold-scuffed-Unreal-Engine/gold-scuffed_normal.png");
	TextureManager::instance()->add("goldNormalMap", goldNormalMap);

	TextureLoader *goldMetallicMap = new TextureLoader();
	goldMetallicMap->load("Images/PBR/gold-scuffed-Unreal-Engine/gold-scuffed_metallic.png");
	TextureManager::instance()->add("goldMetallicMap", goldMetallicMap);

	TextureLoader *goldRoughnessMap = new TextureLoader();
	goldRoughnessMap->load("Images/PBR/gold-scuffed-Unreal-Engine/gold-scuffed_roughness.png");
	TextureManager::instance()->add("goldRoughnessMap", goldRoughnessMap);

	TextureLoader *goldAOMap = new TextureLoader();
	goldAOMap->load("Images/PBR/gold-scuffed-Unreal-Engine/gold-scuffed_basecolor-boosted.png");
	TextureManager::instance()->add("goldAOMap", goldAOMap);

	// Grass
	TextureLoader *grassAlbedoMap = new TextureLoader();
	grassAlbedoMap->load("Images/PBR/grass1-Unreal-Engine2/grass1-albedo3.png");
	TextureManager::instance()->add("grassAlbedoMap", grassAlbedoMap);

	TextureLoader *grassNormalMap = new TextureLoader();
	grassNormalMap->load("Images/PBR/grass1-Unreal-Engine2/grass1-normal2.png");
	TextureManager::instance()->add("grassNormalMap", grassNormalMap);

	TextureLoader *grassMetallicMap = new TextureLoader();
	grassMetallicMap->load("Images/PBR/grass1-Unreal-Engine2/grass1-height.png");
	TextureManager::instance()->add("grassMetallicMap", grassMetallicMap);

	TextureLoader *grassRoughnessMap = new TextureLoader();
	grassRoughnessMap->load("Images/PBR/grass1-Unreal-Engine2/grass1-rough.png");
	TextureManager::instance()->add("grassRoughnessMap", grassRoughnessMap);

	TextureLoader *grassAOMap = new TextureLoader();
	grassAOMap->load("Images/PBR/grass1-Unreal-Engine2/grass1-ao.png");
	TextureManager::instance()->add("grassAOMap", grassAOMap);

	// plastic
	TextureLoader *plasticAlbedoMap = new TextureLoader();
	plasticAlbedoMap->load("Images/PBR/plasticpattern1-ue/plasticpattern1-albedo.png");
	TextureManager::instance()->add("plasticAlbedoMap", plasticAlbedoMap);

	TextureLoader *plasticNormalMap = new TextureLoader();
	plasticNormalMap->load("Images/PBR/plasticpattern1-ue/plasticpattern1-normal2b.png");
	TextureManager::instance()->add("plasticNormalMap", plasticNormalMap);

	TextureLoader *plasticMetallicMap = new TextureLoader();
	plasticMetallicMap->load("Images/PBR/plasticpattern1-ue/plasticpattern1-metalness.png");
	TextureManager::instance()->add("plasticMetallicMap", plasticMetallicMap);

	TextureLoader *plasticRoughnessMap = new TextureLoader();
	plasticRoughnessMap->load("Images/PBR/plasticpattern1-ue/plasticpattern1-roughness2.png");
	TextureManager::instance()->add("plasticRoughnessMap", plasticRoughnessMap);

	TextureLoader *plasticAOMap = new TextureLoader();
	plasticAOMap->load("Images/PBR/plasticpattern1-ue/plasticpattern1-ao.png");
	TextureManager::instance()->add("plasticAOMap", plasticAOMap);

	// wall
	TextureLoader *wallAlbedoMap = new TextureLoader();
	wallAlbedoMap->load("Images/PBR/redbricks2b-Unreal-Engine/redbricks2b-albedo.png");
	TextureManager::instance()->add("wallAlbedoMap", wallAlbedoMap);

	TextureLoader *wallNormalMap = new TextureLoader();
	wallNormalMap->load("Images/PBR/redbricks2b-Unreal-Engine/redbricks2b-normal.png");
	TextureManager::instance()->add("wallNormalMap", wallNormalMap);

	TextureLoader *wallMetallicMap = new TextureLoader();
	wallMetallicMap->load("Images/PBR/redbricks2b-Unreal-Engine/redbricks2b-metalness.png");
	TextureManager::instance()->add("wallMetallicMap", wallMetallicMap);

	TextureLoader *wallRoughnessMap = new TextureLoader();
	wallRoughnessMap->load("Images/PBR/redbricks2b-Unreal-Engine/redbricks2b-rough.png");
	TextureManager::instance()->add("wallRoughnessMap", wallRoughnessMap);

	TextureLoader *wallAOMap = new TextureLoader();
	wallAOMap->load("Images/PBR/redbricks2b-Unreal-Engine/redbricks2b-ao.png");
	TextureManager::instance()->add("wallAOMap", wallAOMap);

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
	glm::mat4 projection = glm::perspective(CameraManager::instance()->get("camera")->Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	ShaderManager::instance()->get("pbrShader")->use();
	ShaderManager::instance()->get("pbrShader")->setUniform("projection", projection);
	ShaderManager::instance()->get("backgroundShader")->use();
	ShaderManager::instance()->get("backgroundShader")->setUniform("projection", projection);

	ShaderManager::instance()->get("pbrShader")->use();
	glm::mat4 view = CameraManager::instance()->get("camera")->GetViewMatrix();
	ShaderManager::instance()->get("pbrShader")->setUniform("view", view);
	ShaderManager::instance()->get("pbrShader")->setUniform("camPos", CameraManager::instance()->get("camera")->Position);

	// bind pre-computed IBL data
	TextureManager::instance()->get("irradianceMap")->bind(0, -1, true);
	TextureManager::instance()->get("prefilterMap")->bind(1, -1, true);
	TextureManager::instance()->get("brdfLUTTexture")->bind(2);

	// rusted iron
	TextureManager::instance()->get("ironAlbedoMap")->bind(3);
	TextureManager::instance()->get("ironNormalMap")->bind(4);
	TextureManager::instance()->get("ironMetallicMap")->bind(5);
	TextureManager::instance()->get("ironRoughnessMap")->bind(6);
	TextureManager::instance()->get("ironAOMap")->bind(7);

	glm::mat4 model;
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-5.0, 0.0, 2.0));
	ShaderManager::instance()->get("pbrShader")->setUniform("model", model);
	VertexManager::instance()->get("sphere")->draw(GL_TRIANGLE_STRIP);

	// gold
	TextureManager::instance()->get("goldAlbedoMap")->bind(3);
	TextureManager::instance()->get("goldNormalMap")->bind(4);
	TextureManager::instance()->get("goldMetallicMap")->bind(5);
	TextureManager::instance()->get("goldRoughnessMap")->bind(6);
	TextureManager::instance()->get("goldAOMap")->bind(7);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-3.0, 0.0, 2.0));
	ShaderManager::instance()->get("pbrShader")->setUniform("model", model);
	VertexManager::instance()->get("sphere")->draw(GL_TRIANGLE_STRIP);

	// grass
	TextureManager::instance()->get("grassAlbedoMap")->bind(3);
	TextureManager::instance()->get("grassNormalMap")->bind(4);
	TextureManager::instance()->get("grassMetallicMap")->bind(5);
	TextureManager::instance()->get("grassRoughnessMap")->bind(6);
	TextureManager::instance()->get("grassAOMap")->bind(7);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-1.0, 0.0, 2.0));
	ShaderManager::instance()->get("pbrShader")->setUniform("model", model);
	VertexManager::instance()->get("sphere")->draw(GL_TRIANGLE_STRIP);

	// plastic
	TextureManager::instance()->get("plasticAlbedoMap")->bind(3);
	TextureManager::instance()->get("plasticNormalMap")->bind(4);
	TextureManager::instance()->get("plasticMetallicMap")->bind(5);
	TextureManager::instance()->get("plasticRoughnessMap")->bind(6);
	TextureManager::instance()->get("plasticAOMap")->bind(7);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(1.0, 0.0, 2.0));
	ShaderManager::instance()->get("pbrShader")->setUniform("model", model);
	VertexManager::instance()->get("sphere")->draw(GL_TRIANGLE_STRIP);

	// wall
	TextureManager::instance()->get("wallAlbedoMap")->bind(3);
	TextureManager::instance()->get("wallNormalMap")->bind(4);
	TextureManager::instance()->get("wallMetallicMap")->bind(5);
	TextureManager::instance()->get("wallRoughnessMap")->bind(6);
	TextureManager::instance()->get("wallAOMap")->bind(7);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(3.0, 0.0, 2.0));
	ShaderManager::instance()->get("pbrShader")->setUniform("model", model);
	VertexManager::instance()->get("sphere")->draw(GL_TRIANGLE_STRIP);

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