#include "Render3D.hpp"
#include <Math/Random.hpp>
#include <Core/Platform.h>

namespace Raptor {

	Transform3D::Transform3D() {
		translation = Vector3();
		rotation = Vector3();
		scale = Vector3(1.0f, 1.0f, 1.0f);
	}

	Matrix4 TransformToMatrix(const Transform3D& transform) {
		Matrix4 result =
			Matrix4::Translate(transform.translation) *
			Matrix4::Rotate(transform.rotation) *
			Matrix4::Scale(transform.scale);
		return result;
	}

	void BindMaterial(const Material& material) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.diffuseMapID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material.specularMapID);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, material.normalMapID);
	}

	void Draw(DebugModelData& model) {
		for (auto i = 0; i < model.meshVertexBuffers.size(); i++) {
			auto& material = model.materials[model.meshMaterialIndex[i]];
			auto& buffer = model.meshVertexBuffers[i];
			auto& mesh = model.meshes[i];
			BindMaterial(material);
			glBindVertexArray(buffer.vertexArrayID);
			glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
		}
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
	}



	Camera::Camera() { }

	Camera::Camera(const Vector3& position, float viewportWidth, float viewportHeight) {
		this->position = position;
		this->viewportWidth = viewportWidth;
		this->viewportHeight = viewportHeight;
	}

	void UpdateCamera (Camera* camera) {
		camera->front.x = cos(RADIANS(camera->yaw)) * cos(RADIANS(camera->pitch));
		camera->front.y = sin(RADIANS(camera->pitch));
		camera->front.z = sin(RADIANS(camera->yaw)) * cos(RADIANS(camera->pitch));
		camera->front.Normalize();
		camera->view = Matrix4::LookAt(camera->position, camera->position + camera->front, Vector3(0.0f, 1.0f, 0.0f));
		camera->projection = Matrix4::Perspective(45.0f, camera->viewportWidth / camera->viewportHeight, camera->nearClip, camera->farClip);
	}

	void CalculateNormals(Vertex3D* vertices, U32 vertexCount, U32* indices, U32 indexCount) {
		for (U32 i = 0; i < vertexCount; i++)
			vertices[i].normal = Vector3(0.0f, 0.0f, 0.0f);

		for (uint32 i = 0; i < indexCount; i += 3) {
			uint32 index0 = indices[i + 0];
			uint32 index1 = indices[i + 1];
			uint32 index2 = indices[i + 2];
			Vector3 vert1 = vertices[index1].position - vertices[index0].position;
			Vector3 vert2 = vertices[index2].position - vertices[index0].position;

			Vector3 normal = vert1.Cross(vert2);
			normal.Normalize();
			vertices[index0].normal += normal;
			vertices[index1].normal += normal;
			vertices[index2].normal += normal;
		}
		for (uint32 i = 0; i < vertexCount; i++) {
			vertices[i].normal.Normalize();
		}
	}

	void FPSCameraControlUpdate(Camera* camera) {
		int dx, dy;
		PlatformGetCursorDelta(&dx, &dy);

		camera->yaw += dx;
		camera->pitch -= dy;

		static const float MOVEMENT_SPEED = 0.3f;
		static const float SPRINT_MULTIPLIER = 3.0f;
		float speed = MOVEMENT_SPEED;

		if (PlatformGetKey(KEY_LSHIFT)) speed *= SPRINT_MULTIPLIER;
		if (PlatformGetKey(KEY_W)) camera->position += speed * camera->front;
		if (PlatformGetKey(KEY_S)) camera->position -= speed * camera->front;
		if (PlatformGetKey(KEY_A)) camera->position -= camera->front.Cross(Vector3(0.0f, 1.0f, 0.0f)).Normalize() * speed;
		if (PlatformGetKey(KEY_D)) camera->position += camera->front.Cross(Vector3(0.0f, 1.0f, 0.0f)).Normalize()  * speed;
		if (PlatformGetKey(KEY_SPACE)) camera->position.y += speed;
		if (PlatformGetKey(KEY_LCTRL)) camera->position.y -= speed;
		camera->pitch = MathUtils::Clamp(camera->pitch, -89.0f, 89.0f);
	}



	void PushLight(const PointLight& light, U32 index, GLuint shader) {
		glUniform3fv(glGetUniformLocation(shader, ("pointLights[" + std::to_string(index) + "].position").c_str()), 1, &light.position.x);
		glUniform3fv(glGetUniformLocation(shader, ("pointLights[" + std::to_string(index) + "].color").c_str()), 1, &light.color.x);
		glUniform1f(glGetUniformLocation(shader, ("pointLights[" + std::to_string(index) + "].linear").c_str()), light.linear);
		glUniform1f(glGetUniformLocation(shader, ("pointLights[" + std::to_string(index) + "].quadratic").c_str()), light.quadratic);
		glUniform1f(glGetUniformLocation(shader, ("pointLights[" + std::to_string(index) + "].radius").c_str()), light.radius);
	}

	void PushLights(const PointLight* lights, U32 count, GLuint shader) {
		GLint lightsLoc = GetUniformLocation(shader, "pointLights[0].position");
	}

	void InitDepthShader(DepthShader* shader, U32 mapWidth, U32 mapHeight) {
		shader->program = CreateShader("shaders/DepthCubemap.vert", "shaders/DepthCubemap.frag", "shaders/DepthCubemap.geom");
		shader->depth_map = CreateDepthCubemap(mapWidth, mapHeight);
		shader->mapWidth = mapWidth;
		shader->mapHeight = mapHeight;
		shader->framebuffer = CreateDepthFramebuffer(shader->depth_map);
		shader->nearPlane = 1.0f;
		shader->farPlane = 50.0f;
		shader->shadowProjectionMatrix = Matrix4::Perspective(RADIANS(90.0f), (float)mapWidth / (float)mapHeight, shader->nearPlane, shader->farPlane);
	}

	GLuint CreateDepthFramebuffer(GLuint depthCubemap) {
		GLuint framebufferID;
		glGenFramebuffers(1, &framebufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			LOG_ERROR("Framebuffer not complete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return framebufferID;
	}

	GLuint CreateDepthCubemap(U32 textureWidth, U32 textureHeight) {
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		for (GLint i = 0; i < 6; i++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT24,
				textureWidth, textureHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

		return textureID;
	}

	void BeginDepthShadingPass(DepthShader* shader) {
		glViewport(0, 0, shader->mapWidth, shader->mapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shader->framebuffer);
		glClear(GL_DEPTH_BUFFER_BIT);

		static GLint farPlaneLoc = GetUniformLocation(shader->program, "farPlane");
		glUseProgram(shader->program);
		glUniform1f(farPlaneLoc, shader->farPlane);
	}

	void DepthShadingPushLight(PointLight& light, DepthShader* shader) {
		Matrix4 shadowTransforms[6];
		shadowTransforms[0] = shader->shadowProjectionMatrix * Matrix4::LookAt(light.position, light.position + Vector3(1.0, 0.0, 0.0), Vector3(0.0, -1.0, 0.0));
		shadowTransforms[1] = shader->shadowProjectionMatrix * Matrix4::LookAt(light.position, light.position + Vector3(-1.0, 0.0, 0.0), Vector3(0.0, -1.0, 0.0));
		shadowTransforms[2] = shader->shadowProjectionMatrix * Matrix4::LookAt(light.position, light.position + Vector3(0.0, 1.0, 0.0), Vector3(0.0, 0.0, 1.0));
		shadowTransforms[3] = shader->shadowProjectionMatrix * Matrix4::LookAt(light.position, light.position + Vector3(0.0, -1.0, 0.0), Vector3(0.0, 0.0, -1.0));
		shadowTransforms[4] = shader->shadowProjectionMatrix * Matrix4::LookAt(light.position, light.position + Vector3(0.0, 0.0, 1.0), Vector3(0.0, -1.0, 0.0));
		shadowTransforms[5] = shader->shadowProjectionMatrix * Matrix4::LookAt(light.position, light.position + Vector3(0.0, 0.0, -1.0), Vector3(0.0, -1.0, 0.0));

		static GLint shadowTransformLoc = GetUniformLocation(shader->program, "shadowTransforms[0]");
		static GLint lightPosLoc = GetUniformLocation(shader->program, "lightPos");
		glUniformMatrix4fv(shadowTransformLoc, 6, GL_FALSE, &shadowTransforms[0][0][0]);
		glUniform3fv(lightPosLoc, 1, &light.position.x);
	}

	void EndDepthShadingPass() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	void InitFowardShader(FowardShader* shader) {
		shader->program = CreateShader("shaders/FowardShading.vert", "shaders/FowardShading.frag");
		glUseProgram(shader->program);
		glUniform1i(glGetUniformLocation(shader->program, "depthMap"), 3);
	}

	void BeginFowardShadingPass(FowardShader* shader, DepthShader* depthShader, Camera* camera) {
		glViewport(0, 0, camera->viewportWidth, camera->viewportHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shader->program);
		glUniformMatrix4fv(2, 1, GL_FALSE, &camera->projection[0][0]);
		glUniformMatrix4fv(1, 1, GL_FALSE, &camera->view[0][0]);
		glUniform3fv(3, 1, &camera->position.x);
		glUniform1f(4, depthShader->farPlane);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthShader->depth_map);
	}

	void EndFowardShadingPass() {

	}


	struct SSAOShader {
		GLuint program;
		GLuint noiseTexture;
		GLuint framebuffer;
		GLuint colorbuffer;
	};

	//TODO we should proabably move screenWidth / height params into some
	//RenderContext that the renderer will use to obtain those values...
	//THis is where we can also store the RenderConfig etc...
	void InitSSAO(SSAOShader* shader, U32 screenWidth, U32 screenHeight) {
		shader->program = CreateShader("shaders/SSAO.vert", "shaders/SSAO.frag");

		glGenFramebuffers(1, &shader->framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, shader->framebuffer);

		glGenTextures(1, &shader->colorbuffer);
		glBindTexture(GL_TEXTURE_2D, shader->colorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shader->colorbuffer, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			LOG_ERROR("SSAO Frambuffer not complete!");

		//Create the Noise Texture
		Random rng(0);
		Vector3 noise[16];
		for (auto i = 0; i < 16; i++)
			noise[i] = Vector3(rng.Range(-1.0f, 1.0f), rng.Range(-1.0f, 1.0f), 0.0f);
		glGenTextures(1, &shader->noiseTexture);
		glBindTexture(GL_TEXTURE_2D, shader->noiseTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, noise);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//Really stupid wierd lerp thing!
		auto lerp = [](float a, float b, float f) -> float{
			auto result = a + f * (b - a);
			return result;
		};

		//The Kernel is a hemishpere aligned in tangent to the normal vector
		// The x and y components are varied from -1.0 to 1.0
		//The Z compoent is only from 0 to 1
		glUseProgram(shader->program);
		for (auto i = 0; i < 64; i++) {
			Vector3 sample = Vector3(rng.Range(-1.0f, 1.0f), rng.Range(-1.0f, 1.0f), rng.Range(0.0f, 1.0f));
			sample.Normalize();
			float scale = (float)i / 64.0f;
			scale = lerp(0.1f, 1.0f, scale * scale);
			glUniform3f(GetUniformLocation(shader->program, ("samples[" + std::to_string(i) + "]").c_str()),
				sample.x, sample.y, sample.z);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}



#if 1
	void InitDeferredShader(DeferredShader* shader, U32 screenWidth, U32 screenHeight) {
		glGenFramebuffers(1, &shader->gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, shader->gBuffer);

		glGenTextures(1, &shader->gPosition);
		glBindTexture(GL_TEXTURE_2D, shader->gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shader->gPosition, 0);

		glGenTextures(1, &shader->gNormal);
		glBindTexture(GL_TEXTURE_2D, shader->gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, shader->gNormal, 0);

		glGenTextures(1, &shader->gColor);
		glBindTexture(GL_TEXTURE_2D, shader->gColor);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, shader->gColor, 0);

		GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		glGenRenderbuffers(1, &shader->renderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, shader->renderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, shader->renderBuffer);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			LOG_ERROR("Framebuffer not complete!");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		shader->geometeryPassProgram = CreateShader("shaders/GBuffer.vert", "shaders/GBuffer.frag");
		shader->lightingPassProgram = CreateShader("shaders/DeferredShading.vert", "shaders/DeferredShading.frag");

		glGenVertexArrays(1, &shader->quadVertexArray);
	}
#endif

#if 0
	void InitDeferredShader(DeferredShader* shader, U32 screenWidth, U32 screenHeight) {
		glGenFramebuffers(1, &shader->gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, shader->gBuffer);

		glGenTextures(3, shader->textures);
		glBindTexture(GL_TEXTURE_2D, shader->textures[0]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32UI, screenWidth, screenHeight);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenTextures(1, &shader->gNormal);
		glBindTexture(GL_TEXTURE_2D, shader->)

	}
#endif

	void RemoveDeferredShader(DeferredShader* shader) {
		glDeleteFramebuffers(1, &shader->gBuffer);
		glDeleteTextures(1, &shader->gPosition);
		glDeleteTextures(1, &shader->gNormal);
		glDeleteTextures(1, &shader->gColor);

		glDeleteProgram(shader->geometeryPassProgram);
		glDeleteProgram(shader->lightingPassProgram);
	};

	void BeginDeferredShadingGeometryPass(DeferredShader* shader, Camera* camera) {
		glBindFramebuffer(GL_FRAMEBUFFER, shader->gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shader->geometeryPassProgram);

		glUniformMatrix4fv(1, 1, GL_FALSE, &camera->view[0][0]);
		glUniformMatrix4fv(2, 1, GL_FALSE, &camera->projection[0][0]);
	}

	void EndDeferredShadingGeometeryPass() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void BeginDeferredShadingLightingPass(DeferredShader* shader, Camera* camera) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shader->lightingPassProgram);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shader->gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, shader->gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, shader->gColor);

		glUniform3fv(3, 1, &camera->position.x);	//viewPos
	}

	void RenderScreenQuad() {
		static GLuint vertexArrayID;
		static GLuint vertexBufferID;
		if (vertexArrayID == 0) {
			GLfloat quadVertices[] = {
				// Positions        // Texture Coords
				-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// Setup plane VAO
			glGenVertexArrays(1, &vertexArrayID);
			glBindVertexArray(vertexArrayID);
			glGenBuffers(1, &vertexBufferID);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		}
		glBindVertexArray(vertexArrayID);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	//TODO we need some type of RenderContext inorder to store screen stuff.. etc
	//Or a better way to expose the functionalyity in the application layer
	void EndDeferredShadingLightingPass(DeferredShader* shader) {
		glBindVertexArray(shader->quadVertexArray);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		int width, height;
		PlatformGetSize(&width, &height);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, shader->gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Write to default framebuffer
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void CreateDebugCube(GLuint* vertexArrayID, GLuint* vertexBufferID, float scale) {
		float vertices[36 * 3] = {
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,
			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f
		};

		for (auto i = 0; i < 36 * 3; i++) {
			vertices[i] *= scale;
		}

		glGenVertexArrays(1, vertexArrayID);
		glGenBuffers(1, vertexBufferID);

		glBindVertexArray(*vertexArrayID);

		glBindBuffer(GL_ARRAY_BUFFER, *vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
		glBindVertexArray(0);
	}

	void DrawDebugCube() {
		static GLuint vertexArrayID = 0, vertexBufferID = 0;
		if (!vertexArrayID) CreateDebugCube(&vertexArrayID, &vertexBufferID, 0.1f);

		glBindVertexArray(vertexArrayID);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}



};
