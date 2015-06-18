#include "SpaceScene.hpp"

SpaceScene::SpaceScene() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


SpaceScene::~SpaceScene() {
	
}

void SpaceScene::OnLoad(Engine& engine) {
	this->engine = &engine;
	camera = std::make_unique<FPSCamera>(1280.0f, 720.0f);
	camera->position.Set(0.0f, 0.0f, 200.0f);
	engine.GetApp()->AddListener(camera.get());

	//Create a light to add to the renderer
	light = std::make_unique<PointLight>();
	light->color.Set(1.0f, 1.0f, 1.0f);
	light->ambientIntensity = 0.1f;
	light->diffuseIntensity = 0.5f;
	light->specularIntensity = 0.5f;

	//Create the texture of our planet
	std::vector<Image> images;
	images.push_back(ResourceManager::GenerateImage(2, 2));
	images.push_back(ResourceManager::GenerateImage(2, 2));
	images.push_back(ResourceManager::GenerateImage(2, 2));
	images.push_back(ResourceManager::GenerateImage(2, 2));
	images.push_back(ResourceManager::GenerateImage(2, 2));
	images.push_back(ResourceManager::GenerateImage(2, 2));
	planetTexture = std::make_unique<Cubemap>(images);

	//Skybox creation
	std::vector<const GLchar*> faces;
	faces.push_back("Resources/skybox/space/right.png"); 
	faces.push_back("Resources/skybox/space/left.png");
	faces.push_back("Resources/skybox/space/top.png");
	faces.push_back("Resources/skybox/space/bottom.png");
	faces.push_back("Resources/skybox/space/front.png");
	faces.push_back("Resources/skybox/space/back.png");
	skybox = std::make_unique<Skybox>(faces);

		//Load the model
	renderer = std::make_unique<GLRenderer>(camera.get());
	//nanosuitModel = ResourceManager::LoadModel("Resources/models/nanosuit/nanosuit.obj");

	//Create the universe
	universe = std::make_unique<Universe>(57);
	auto& star = universe->CreateCelestialBody(STAR, 3);
	light->position = star->position;

	universe->CreateCelestialBody(TERRESTRIAL, 7);
	universe->CreateCelestialBody(JOVIAN, 56);
	camera->position.Set(&Vector3(universe->bodies[0]->data.radius + 100.0f, 0.0f, 0.0f));

	canvas = std::make_unique<DebugCanvas>(1280, 720);
	canvas->camera = camera.get();

	engine.GetApp()->AddListener(this);

	texture1D = GenerateJovianTexture(256, RGBA8{ 50, 45, 200, 255 }, RGBA8{ 50, 120, 255, 255 }, 0.25f);

	//Load shaders
	terrestrialShader = std::make_unique<GLSLProgram>("Resources/shaders/lighting.vert", "Resources/shaders/lighting.frag");
	solarShader = std::make_unique<GLSLProgram>("Resources/shaders/Solar.vert", "Resources/shaders/Solar.frag");
	jovianShader = std::make_unique<GLSLProgram>("Resources/shaders/Jovian.vert", "Resources/shaders/Jovian.frag");
	wireframeShader = std::make_unique<GLSLProgram>("Resources/shaders/wireframe.vert", "Resources/shaders/wireframe.frag");
	skyboxShader = std::make_unique<GLSLProgram>("Resources/shaders/skybox.vert", "Resources/shaders/skybox.frag");
	modelShader = std::make_unique<GLSLProgram>("Resources/shaders/Model.vert", "Resources/shaders/Model.frag");
}

void SpaceScene::OnDestroy(Engine& engine) {

}


GLuint SpaceScene::GenerateJovianTexture(int textureSize, RGBA8 baseColor, RGBA8 bandColor, float bandingArea) {
	//TODO better 1D noise... move to noise.h
	auto noise = [](int x) {
		x = (x << 13) ^ x;
		return (1.0 - ((x * (x * x * 15731 + 789221) + 1376312589) & 0x7FFFFFF) / 1073741824.0);
	};

	float center = ((float)textureSize * 0.5f);
	float dr = bandColor.r - baseColor.r;
	float dg = bandColor.g - baseColor.g;
	float db = bandColor.b - baseColor.b;

	RGBA8* data = new RGBA8[textureSize];
	for (int x = 0; x < textureSize; x++) {
		float dist = abs(center - (float)x);
		float fade = (dist / center);

		RGBA8 color;
		float n = noise(x);
		color.r = baseColor.r + (dr - (dr * fade)) * (n - (n * fade));
		color.g = baseColor.g + (dg - (dg * fade)) * (n - (n * fade));
		color.b = baseColor.b + (db - (db * fade)) * (n - (n * fade));
		color.a = baseColor.a;
		data[x] = color;
	}

	GLuint textureID;
	glGenTextures(0, &textureID);
	glBindTexture(GL_TEXTURE0, textureID);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, textureSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE0, 0);
	return textureID;
}



void SpaceScene::Tick(float deltaTime) {
	camera->Update(deltaTime);
	canvas->Update(deltaTime);

	//Simulate gravity
	for (std::unique_ptr<CelestialBody>& source : universe->bodies) {
		//Apply the bodies force of gravity to other bodies
		for (std::unique_ptr<CelestialBody>& target : universe->bodies) {
			if (source == target) continue;

			Vector3 displacement = (target->position - source->position);
			float length = displacement.Length();
			float r2 = length * length;
			float totalMass = (source->data.mass + target->data.mass);

			float magnitude = (universe->G * (totalMass / r2));

			Vector3 force(-magnitude, 0, 0);
			force = force * displacement.Normalize();

			Vector3 velocity = (force / target->data.mass, 0, 0);
			target->position += (velocity * deltaTime);
		}
		//Apply the bodies linear velocity
		source->position += (source->linearVelocity * deltaTime);
	}
}


void SpaceScene::Render(float deltaTime) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Set render mode if wireframe is disabled
	if (wireframe) {
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	//Render each body in the universe
	for (std::unique_ptr<CelestialBody>& body : universe->bodies) {
		//Setup matricies
		glm::mat4 model = glm::translate(glm::mat4(1.0), body->position.ToGLM());
		model = glm::scale(model, glm::vec3(body->data.radius));
		glm::mat4 mvp = camera->GetCombinedMatrix() * model;

		//Lambda functor to render celestial bodies given a shader
		auto render = [&](std::unique_ptr<GLSLProgram>& shader) {
			shader->Use();

			glUniformMatrix4fv(shader->GetUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
			//Skip this statement if the shader does not have these uniforms
			if (!wireframe){
				glUniform3f(shader->GetUniformLocation("viewPosition"), camera->position.x, camera->position.y, camera->position.z);
				glUniformMatrix4fv(shader->GetUniformLocation("model"), 1, GL_FALSE, &model[0][0]);
			}

			std::unique_ptr<Mesh>& mesh = (std::unique_ptr<Mesh>&)body->GetComponent();
			glBindVertexArray(mesh->vertexArrayID);
			glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		};

		auto testJovianRenderer = [&](std::unique_ptr<GLSLProgram>& shader){
			shader->Use(body->data.name);
			glUniformMatrix4fv(shader->GetUniformLocation("model"), 1, GL_FALSE, &model[0][0]);
			glUniformMatrix4fv(shader->GetUniformLocation("mvp"), 1, GL_FALSE, &mvp[0][0]);
			//Lighting stuff
			glUniform3f(shader->GetUniformLocation("light.position"), light->position.x, light->position.y, light->position.z);
			glUniform3f(shader->GetUniformLocation("light.color"), light->color.x, light->color.y, light->color.z);
			glUniform1f(shader->GetUniformLocation("light.ambient"), light->ambientIntensity);
			glUniform1f(shader->GetUniformLocation("light.diffuse"), light->diffuseIntensity);
			glUniform1f(shader->GetUniformLocation("light.specular"), light->specularIntensity);
			std::unique_ptr<Mesh>& mesh = (std::unique_ptr<Mesh>&)body->GetComponent();
			glBindVertexArray(mesh->vertexArrayID);
			glBindTexture(GL_TEXTURE0, texture1D);
			glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		};


		if (wireframe)
			render(wireframeShader);
		else if (body->data.type == STAR)
			render(solarShader);
		else if (body->data.type == JOVIAN)
			testJovianRenderer(jovianShader);
		else
			render(terrestrialShader);
	}

	//renderer->Draw(nanosuitModel, wireframeShader);

	//Renable GL states if wireframe
	if (wireframe) {
		glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	//Draw the skybox
	//Will not be drawn in wireframe
	glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
	skyboxShader->Use();
	glm::mat4 projection = camera->GetProjectionMatrix();
	glm::mat4 view = glm::mat4(glm::mat3(camera->GetViewMatrix()));	// Remove any translation component of the view matrix
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader->GetProgram(), "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader->GetProgram(), "projection"), 1, GL_FALSE, &projection[0][0]);
	glBindVertexArray(skybox->vertexArrayID);
	skybox->cubemap->Bind(GL_TEXTURE0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // Set depth function back to default

	if (drawDeveloperCanvas) {
		canvas->Render(deltaTime);
	}
}


bool SpaceScene::OnKeyEvent(Input::Keycode keycode, bool isPressed, int mods) {
	//Not interesed in release events
	if (!isPressed) {
		return false;
	}

	switch (keycode) {
	case Input::KEY_ESCAPE:
		//Lets stash the engine i guess...
		engine->GetApp()->SetCursorHidden(false);
		//engine->GetApp()->Exit();
		return true;

	case Input::KEY_F3:
		wireframe = !wireframe;
		return true;

	case Input::KEY_GRAVE_ACCENT:
		drawDeveloperCanvas = !drawDeveloperCanvas;
		return true;
	}

	return false;
}

bool SpaceScene::OnCursorPosEvent(double xpos, double ypos) {
	//Do nothing
	return false;
}