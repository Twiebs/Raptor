#include <Core/Platform.h>

#include <Engine/Editor.hpp>
#include <Engine/Engine.hpp>
#include <Engine/Assets.hpp>
#include <Engine/Debug.hpp>
#include <Engine/GFX3D.hpp>

#include <Graphics/Liquid.hpp>
#include <Graphics/GLRenderer.hpp>
#include <Graphics/GLSLProgram.hpp>
#include <Graphics/Skybox.hpp>
#include <Graphics/imgui.h>

#include <Math/Random.hpp>
#include <Math/Procedural3D.hpp>

#define IMPLEMENT_GLOBAL_PROFILER
#include <Utils/global_profiler.h>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <new>

auto v3bt = [](const V3& a) -> btVector3 {
	return btVector3(a.x, a.y, a.z);
};

auto btv3 = [](const btVector3& a) -> V3 {
	return V3(a.x(), a.y(), a.z());
};

auto btclr = [](const btVector3& color) -> Color {
	return Color(color.x(), color.y(), color.z(), 1.0f);
};

class PhysicsDebugDraw : public btIDebugDraw {
public:
	PhysicsDebugDraw() { } 
	PhysicsDebugDraw(Camera* camera) : camera(camera) { } 
	void SetCamera(Camera* camera) { this->camera = camera; }
	virtual ~PhysicsDebugDraw() { }
	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) final;
	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) final;
	virtual void reportErrorWarning(const char* warningString) final;
	virtual void draw3dText(const btVector3& location, const char* textString) final;
	virtual void setDebugMode(int debugMode) final;
	virtual int	getDebugMode() const final;

private:
	Camera* camera;
};

void PhysicsDebugDraw::drawLine (const btVector3& from, const btVector3& to, const btVector3& color) {
	GFX3D::DrawLine(btv3(from), btv3(to), btclr(color));
}

void PhysicsDebugDraw::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) { }
void PhysicsDebugDraw::reportErrorWarning(const char* warningString) { }
void PhysicsDebugDraw::draw3dText(const btVector3& location, const char* textString) { }
void PhysicsDebugDraw::setDebugMode(int debugMode) { }
int	 PhysicsDebugDraw::getDebugMode() const { return 1; }


template<typename TObject>
class BasicObjectAllocator {
	size_t count;
	size_t capacity;
	TObject* objects;
public:
	BasicObjectAllocator(size_t capacity);
	~BasicObjectAllocator();

	TObject* operator[](size_t index) {
		assert(index < count);
		return &objects[index];
	}

	size_t Count() { return count; }

	template<typename... TArgs>
	TObject* CreateObject(TArgs... args) {
		assert(count < capacity && "BasicObjectAllocator cannot grow!");
		auto result = new (&objects[count++]) TObject(args...);
		return result;
	}

	TObject* AllocateUninitalized() {
		assert(count < capacity && "BasicObjectAllocator does not have grow functionality");
		auto result = &objects[count++];
		return result;
	}

};

template<typename TObject>
BasicObjectAllocator<TObject>::BasicObjectAllocator(size_t capacity) :
	capacity(capacity), count(0)
{
	objects = (TObject*)malloc(capacity * sizeof(TObject));
}

template<typename TObject>
BasicObjectAllocator<TObject>::~BasicObjectAllocator() {
	free(objects);
}


struct PhysicsDynamicBox {
	btBoxShape shape;
	btRigidBody body;
	btDefaultMotionState motionState;
	PhysicsDynamicBox(V3 bounds);
};

PhysicsDynamicBox::PhysicsDynamicBox(V3 bounds)
	: shape(btVector3(bounds.x, bounds.y, bounds.z)),
	motionState(),
	body(1.0f, 0, &shape) {
	body.setMotionState(&motionState);
}


class PhysicsWorld {
public:
	btDbvtBroadphase broadphase;
	btDefaultCollisionConfiguration collisionConfiguration;
	btCollisionDispatcher dispatcher;
	btSequentialImpulseConstraintSolver solver;
	btDiscreteDynamicsWorld dynamicsWorld;
	BasicObjectAllocator<PhysicsDynamicBox> boxAllocator;

	PhysicsDebugDraw debugDraw;

	PhysicsWorld();
	~PhysicsWorld();
	void Update();
	void DebugDraw();
	PhysicsDynamicBox* CreateDynamicBox(const V3& bounds);
};

PhysicsWorld::PhysicsWorld() 
	: dispatcher(&collisionConfiguration),
	dynamicsWorld(&dispatcher, &broadphase, &solver, &collisionConfiguration),
	boxAllocator(100)
{
	dynamicsWorld.setGravity(btVector3(0, -9.8, 0.0));
	dynamicsWorld.setDebugDrawer(&debugDraw);
}

PhysicsWorld::~PhysicsWorld() {
	for (U32 i = 0; i < boxAllocator.Count(); i++) {
		auto box = boxAllocator[i];
		dynamicsWorld.removeRigidBody(&box->body);
	}

	//for (auto box : boxes) {
	//	dynamicsWorld.removeRigidBody(&box->body);
	//}

	//boxes.clear();
}



PhysicsDynamicBox* PhysicsWorld::CreateDynamicBox(const V3& bounds) {
	auto result = boxAllocator.AllocateUninitalized();
	new (&result->shape) btBoxShape(btVector3(bounds.x, bounds.y, bounds.z));
	new (&result->motionState) btDefaultMotionState;
	new (&result->body) btRigidBody(1.0f, &result->motionState, &result->shape);
	dynamicsWorld.addRigidBody(&result->body);
	return result;
}

void PhysicsWorld::Update() {
	static const float TIME_STEP = 1.0f / 60.0f;
	static const int SUB_STEPS = 10;
	dynamicsWorld.stepSimulation(TIME_STEP, SUB_STEPS);
}

void PhysicsWorld::DebugDraw() {
	dynamicsWorld.debugDrawWorld();
}

struct ControllerParams {
	float movementSpeed;
};

static void FPSPlayerControls(V3* linearVelocity, V3* front, ControllerParams* params, float deltaTime) {
	if (PlatformGetKey(KEY_W)) 
		*linearVelocity -= params->movementSpeed * deltaTime;
	if (PlatformGetKey(KEY_S)) 
		*linearVelocity += params->movementSpeed * deltaTime;
}


struct ScopedMemory {
	size_t size;
	void* data;

	ScopedMemory(size_t size);
	~ScopedMemory();
};

ScopedMemory::ScopedMemory(size_t capacity) {
	data = malloc(capacity);
}

ScopedMemory::~ScopedMemory() {
	free(data);
}



void RunDeferredTest() {
	ShaderHandle materialShaderHandle;
	
	MaterialAssetInfo materialInfo = { };
	materialInfo.name = "MyTestMaterial";
	materialInfo.directory = ASSET_FILE("materials/stone_tile/");
	materialInfo.diffuseTextureFilename = "diffuse.tga";
	materialInfo.normalTextureFilename = "normal.tga";
	materialInfo.specularTextureFilename = "specular.tga";
	auto testMaterialHandel = LoadMaterial(materialInfo);

	ProfilerBeginEntry("LoadModels");
	auto rockModelHandle = LoadModel(ASSET_FILE("models/rock0/rock1.obj"));
	auto crateModelHandle = LoadModel(ASSET_FILE("models/containers/wood_barrel_0.obj"));
	ProfilerEndEntry("LoadModels");

	ProfilerBeginEntry("BuildShaders");
	ShaderBuilder materailShaderBuilder("MaterialShader");
	materailShaderBuilder.addSourceFile(VERTEX_SHADER, SHADER_FILE("DeferredGenericMaterial.vert"));
	materailShaderBuilder.addSourceFile(FRAGMENT_SHADER, SHADER_FILE("DeferredGenericMaterial.frag"));
	materialShaderHandle = LoadShader(materailShaderBuilder);

	ShaderBuilder skyboxShaderBuilder("Skybox");
	skyboxShaderBuilder.addSourceFile(VERTEX_SHADER, SHADER_FILE("skybox.vert"));
	skyboxShaderBuilder.addSourceFile(FRAGMENT_SHADER, SHADER_FILE("skybox.frag"));
	auto& skyboxShaderHandle = LoadShader(skyboxShaderBuilder);
	ProfilerEndEntry("BuildShaders");

	GLuint vao, vbo;
	CreateSkyboxVertices(&vao, &vbo);

	// DirectionalLight directionalLight;


	
	// AddLight(directionalLight);


	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);

	int w, h;
	PlatformGetSize(&w, &h);

	Camera camera(w, h, 45.0f, 0.1f, 1000.0f);

	FrameParameters frameParameters = { };
	frameParameters.clearColor = V3(0.1f, 0.1f, 0.1f);

	ImGui::Init();

	EditorWindow editorWindow;
	GFX3D::SetDebugRenderSettings(&editorWindow.renderSettings);

	MeshData data;
	Mesh mesh;


	ProfilerBeginEntry("GeneratePlaneMesh");
	CreatePlaneMesh(&data, 0, 0, 16.0f, 16.0f, 2, 1.0f/16.0f, nullptr);
	ProfilerEndEntry("GeneratePlaneMesh");
	InitMesh(&mesh, &data);
	FreeMeshData(&data);

	Cuboid roomBounds;
	roomBounds.position = V3(0.0);
	roomBounds.size = V3(16.0f);
	Rectangle roomFloor(roomBounds.x, roomBounds.y, roomBounds.w, roomBounds.l);

	camera.position = roomBounds.Center();
	





	V3 entityPositions[64];
	Random rng(0);
	for (U32 i = 0; i < 64; i++) {
		auto point = rng.PointInRectangle(roomFloor);
		entityPositions[i] = V3(point.x, 0.0f, point.y);
	}

	PointLight lights[64];
	for (U32 i = 0; i < 16; i++) {
		lights[i].position = rng.PointInCuboid(roomBounds);
		// lights[i].position = V3(0.5f + (1.0f * i), 0.5f, 0.5f);
		lights[i].SetRadius(16.0f);
		lights[i].color = V3(0.35) * rng.vector3();
		AddLight(&lights[i]);
	}

	PhysicsWorld physWorld;
	physWorld.debugDraw.SetCamera(&camera);
	btBoxShape shape = btBoxShape(btVector3(16.0f, 0.1f, 16.0f));
	btRigidBody::btRigidBodyConstructionInfo info(0.0f, 0, &shape); // 0 mass denotes a static collider
	btRigidBody body = btRigidBody(info);
	physWorld.dynamicsWorld.addRigidBody(&body);
	
	btCapsuleShape capsule = btCapsuleShape(0.5f, 1.73f);
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(v3bt(camera.position));
	btVector3 localInertia(0, 0, 0);
	capsule.calculateLocalInertia(50.0f, localInertia);
	btDefaultMotionState motionState = btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo constructInfo(50.0f, &motionState, &capsule, localInertia);
	btRigidBody playerBody = btRigidBody(constructInfo);
	physWorld.dynamicsWorld.addRigidBody(&playerBody);

	PhysicsDynamicBox box0(V3(1.0));
	PhysicsDynamicBox box1(V3(1.0));
	PhysicsDynamicBox box2(V3(1.0));
	PhysicsDynamicBox box3(V3(1.0));
	physWorld.dynamicsWorld.addRigidBody(&box0.body);
	physWorld.dynamicsWorld.addRigidBody(&box1.body);
	physWorld.dynamicsWorld.addRigidBody(&box2.body);
	physWorld.dynamicsWorld.addRigidBody(&box3.body);

	//physWorld.CreateDynamicBox(V3(1.0));
	//physWorld.CreateDynamicBox(V3(1.0));
	//physWorld.CreateDynamicBox(V3(1.0));
	//physWorld.CreateDynamicBox(V3(1.0));
	//physWorld.CreateDynamicBox(V3(1.0));


	//BasicObjectAllocator<PhysicsDynamicBox> boxAllocator(100);
	//auto CreateDynamicBox = [&boxAllocator, &physWorld]() {
	//	auto physbox = boxAllocator.CreateObject(V3(1.0));
	//	physWorld.dynamicsWorld.addRigidBody(&physbox->body);
	//};

	//for (U32 i = 0; i < 10; i++) {
	//	CreateDynamicBox();
	//}

	ControllerParams controllerParams;
	controllerParams.movementSpeed = 3.5f;


	PlatformRun([&](double deltaTime) -> void {
		int keycode;
		while (keycode = PlatformPopKeypress()) {
			ProcessEditorKeypress(&editorWindow, keycode);
		}

		if (PlatformGetButton(MOUSE_BUTTON_RIGHT)) 
			FPSCameraControlUpdate(&camera);
		if (PlatformGetKey(KEY_F)) {

		}



		auto linearVelocity = btv3(playerBody.getLinearVelocity());
		// playerBody.getWorldTransform().getBasis().getEulerYPR(rotation.x, rotation.y, rotation.z);
		FPSPlayerControls(&linearVelocity, &camera.front, &controllerParams, (float)deltaTime);
		// std::cout << linearVelocity;
		
		playerBody.setLinearVelocity(v3bt(linearVelocity));
		// playerBody.getWorldTransform().getRotation().setEuler(rotation.x, rotation.y, rotation.z);

		physWorld.Update();

		btVector3 pos = playerBody.getWorldTransform().getOrigin();
		// camera.position.x = pos.x();
		// camera.position.y = pos.y();
		// camera.position.z = pos.z();
			// light.position = camera.position + (camera.front * 6.0f);

		GFX3D::BeginFrame(&frameParameters, &camera);

		//auto cubemap = DebugGetShadowMap();
		//auto& skyboxShader = GetShader(skyboxShaderHandle);
		//glUseProgram(skyboxShader.id);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
		//SetCameraUniforms(camera);
		//auto centeredView = camera.view * Matrix4::Translate(camera.position);
		//Uniform::SetMatrix4(UniformLocation::VIEW_MATRIX, centeredView);
		//DrawSkybox(vao, cubemap, skyboxShader);

		auto& crateModel = GetModel(crateModelHandle);
		auto& rockModel = GetModel(rockModelHandle);
		auto& testMaterial = GetMaterial(testMaterialHandel);
		auto& materialShader = GetShader(materialShaderHandle);


		GFX3D::SetMaterialShader(materialShader);
		for (U32 i = 0; i < 64; i++) {
			// GFX3D::AddModel(crateModel, entityPositions[i]);
		}
		

		GFX3D::SetMaterial((Material*)&testMaterial);
		GFX3D::AddMesh(mesh);
		GFX3D::AddMesh(mesh, Vector3(0.0f, 16.0f, 0.0f), V3(90.0f, 0.0f, 0.0f));
		GFX3D::AddMesh(mesh, Vector3(16.0, 0.0f, 0.0f), V3(0.0f, 0.0f, 90.0f));

		GFX3D::EndFrame();

		physWorld.DebugDraw();

		ImGui::BeginFrame();
		ShowEditorWindow(&editorWindow);
		ImGui::EndFrame();

		Engine::Update();
	});
}


void RunGLDebugContextTest() {
	PlatformRun([](double deltaTime) {
		
	});
}

int main() {
	PlatformCreate("GraphicsTestBed", 1792, 1008);
	//CodeModule editorModule = LoadModule("editor.dll");

	Engine::Init();
	// RunGLDebugContextTest();
	RunDeferredTest();
	Engine::Terminate();

	return 0;
}