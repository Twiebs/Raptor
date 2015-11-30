#include <Core/Platform.h>

#include <Engine/Editor.hpp>
#include <Engine/Engine.hpp>
#include <Engine/Assets.hpp>
#include <Engine/Debug.hpp>
#include <Engine/GFX3D.hpp>
#include <Engine/ECSManager.hpp>

#include <Graphics/Liquid.hpp>
#include <Graphics/GLRenderer.hpp>
#include <Graphics/GLSLProgram.hpp>
#include <Graphics/Skybox.hpp>
#include <Graphics/imgui.h>

#include <Math/Random.hpp>
#include <Math/Procedural3D.hpp>

#define IMPLEMENT_GLOBAL_PROFILER
#include <Utils/global_profiler.h>

#define IMPLEMENT_GLOBAL_ASSET_MANAGER
#include <Engine/GlobalAssetManager.hpp>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <new>

typedef std::string AssetID;

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
	GFX3D::Line(btv3(from), btv3(to), btclr(color));
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
	capacity(capacity), count(0) {
	objects = (TObject*)malloc(capacity * sizeof(TObject));
}

template<typename TObject>
BasicObjectAllocator<TObject>::~BasicObjectAllocator() {
	free(objects);
}




//
//
//struct PhysicsDynamicBox {
//	btBoxShape shape;
//	btRigidBody body;
//	btDefaultMotionState motionState;
//	PhysicsDynamicBox(V3 bounds);
//};
//
//PhysicsDynamicBox::PhysicsDynamicBox(V3 bounds)
//	: shape(btVector3(bounds.x, bounds.y, bounds.z)),
//	motionState(),
//	body(1.0f, 0, &shape) {
//	body.setMotionState(&motionState);
//}


class PhysicsWorld {
public:
	btDbvtBroadphase broadphase;
	btDefaultCollisionConfiguration collisionConfiguration;
	btCollisionDispatcher dispatcher;
	btSequentialImpulseConstraintSolver solver;
	btDiscreteDynamicsWorld dynamicsWorld;

	PhysicsDebugDraw debugDraw;

	PhysicsWorld();
	~PhysicsWorld();
	void Update();
	void DebugDraw();
};

PhysicsWorld::PhysicsWorld() 
	: dispatcher(&collisionConfiguration),
	dynamicsWorld(&dispatcher, &broadphase, &solver, &collisionConfiguration)
{
	dynamicsWorld.setGravity(btVector3(0, -9.8, 0.0));
	dynamicsWorld.setDebugDrawer(&debugDraw);
}

PhysicsWorld::~PhysicsWorld() {
	//for (auto box : boxes) {
	//	dynamicsWorld.removeRigidBody(&box->body);
	//}

	//boxes.clear();
}


global_variable PhysicsWorld* global_physicsWorld;

struct PhysicsDynamicBox {
	std::unique_ptr<btBoxShape> shape;
	btRigidBody* body;
	btDefaultMotionState* motionState;
	PhysicsDynamicBox(const V3& position, const V3& bounds);
	~PhysicsDynamicBox();
};

PhysicsDynamicBox::PhysicsDynamicBox(const V3& position, const V3& bounds) {
	shape = std::make_unique<btBoxShape>(btVector3(bounds.x, bounds.y, bounds.z));
	body = new btRigidBody(1.0f, 0, shape.get());
	motionState = new btDefaultMotionState();
	body->setMotionState(motionState);
	body->getWorldTransform().setOrigin(btVector3(position.x, position.y + bounds.y, position.z));
	global_physicsWorld->dynamicsWorld.addRigidBody(body);
}

PhysicsDynamicBox::~PhysicsDynamicBox() {
	global_physicsWorld->dynamicsWorld.removeRigidBody(body);
	// delete shape;
	delete body;
	delete motionState;
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

struct ControlState {
	V3 linearVelocity;
};

static void FPSPlayerControls(V3* linearVelocity, float rotation, ControllerParams* params, float deltaTime) {
	if (PlatformGetKey(KEY_W)) {
		float velocityScalar = params->movementSpeed;
		linearVelocity->x += cos(RADIANS(rotation)) * velocityScalar * deltaTime;
		linearVelocity->z += sin(RADIANS(rotation)) * velocityScalar * deltaTime;
	}

	if (PlatformGetKey(KEY_S)) {
		float velocityScalar = params->movementSpeed;
		linearVelocity->x -= cos(RADIANS(rotation)) * velocityScalar * deltaTime;
		linearVelocity->z -= sin(RADIANS(rotation)) * velocityScalar * deltaTime;
	}

	if (PlatformGetKey(KEY_SPACE)) {
		linearVelocity->y += 0.5f;
	}
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

struct ModelRenderComponent {
	ModelHandle handle;
};


typedef TypeList<ModelRenderComponent, Transform3D, PhysicsDynamicBox> ComponentList;


template<typename TComponent>
inline void ShowComponentInspector(TComponent* component) {
#if 0
	static_assert(false, "The generic case for ShowComponentInspector was called!  There was no"
		"Introspection function generated for this component type!");
#endif
	// ImGui::Text("Unimplemented component inspector");
}

template<> inline void ShowComponentInspector<Transform3D>(Transform3D* component) {
	if (ImGui::CollapsingHeader("Transform3D")) {
		ImGui::DragFloat3("translation", &component->translation.x, 0.1f);
		ImGui::DragFloat3("rotation", &component->rotation.x, 0.1f);
		ImGui::DragFloat3("scale", &component->scale.x, 0.1f);
	}
}



template<typename TCompnentType, int N>	struct ComponentPropertyInspector;
template<int N> struct ComponentPropertyInspector<TypeList<>, N> {
	static void ShowGUI(ECSManager<ComponentList>* manager, const Entity& entity) { }
};

template<typename TFirst, typename... TTypeList, int N>
struct ComponentPropertyInspector<TypeList<TFirst, TTypeList...>, N> {
	static void ShowGUI(ECSManager<ComponentList>* manager, const Entity& entity) {
		const size_t componentTypeIndex = IndexOfType<TFirst, ComponentList>::value;
		auto entityComponentIndexForType = manager->componentIndicesForEntity[entity.id][componentTypeIndex];
		if (entityComponentIndexForType != -1) {
			TFirst* component = manager->GetComponent<TFirst>(entity);
			ShowComponentInspector<TFirst>(component);
		}
		ComponentPropertyInspector<TypeList<TTypeList...>, N + 1>::ShowGUI(manager, entity);
	}
};


static void ShowEntityPanel(ECSManager<ComponentList>* manager) {
	static int selectedEntity = -1;

	ImGui::BeginChild("EntityList", ImVec2(250, 0));
	for (U32 i = 0; i < manager->entities.size(); i++) {
		if (ImGui::Selectable("Entity", selectedEntity == i)) {
			selectedEntity = i;
		}
	}
	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("Inspector");
	if (selectedEntity != -1) {
		auto& entity = manager->entities[selectedEntity];
		ComponentPropertyInspector<ComponentList, 0>::ShowGUI(manager, entity);
	}
	ImGui::EndChild();
}



static inline void ShowEnityCreator() {
	ImGui::Begin("EntityCreator");
	auto manifest = GetGlobalAssetManifest();
	ImGui::End();
}


void RunDeferredTest() {
	ECSManager<ComponentList> ecsManager(512);

	EditorWindow editorWindow;
	GetGlobalAssetManifest()->Deserialize();
	editorWindow.assetBrowser.manifest = GetGlobalAssetManifest();

	ShaderHandle materialShaderHandle;
	
	MaterialInfo materialInfo = { };
	materialInfo.name = "MyTestMaterial";
	materialInfo.directory = ASSET_FILE("materials/stone_tile/");
	materialInfo.diffuseTextureFilename = "diffuse.tga";
	materialInfo.normalTextureFilename = "normal.tga";
	materialInfo.specularTextureFilename = "specular.tga";
	auto testMaterialHandel = LoadMaterial(materialInfo);

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



	GFX3D::SetDebugRenderSettings(&editorWindow.renderSettings);

	PhysicsWorld physWorld;
	global_physicsWorld = &physWorld;
	physWorld.debugDraw.SetCamera(&camera);
	btBoxShape shape = btBoxShape(btVector3(16.0f * 0.5f, 0.1f * 0.5f, 16.0f * 0.5f));
	btRigidBody::btRigidBodyConstructionInfo info(0.0f, 0, &shape); // 0 mass denotes a static collider
	btRigidBody body = btRigidBody(info);
	body.getWorldTransform().setOrigin(btVector3(8.0f, -0.05f, 8.0f));
	physWorld.dynamicsWorld.addRigidBody(&body);

	btCapsuleShape capsule = btCapsuleShape(0.5f, 1.73f);
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(v3bt(camera.position));
	btVector3 localInertia(0, 0, 0);
	capsule.calculateLocalInertia(1.0f, localInertia);
	btDefaultMotionState motionState = btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo constructInfo(1.0f, &motionState, &capsule, localInertia);
	btRigidBody playerBody = btRigidBody(constructInfo);
	playerBody.setAngularFactor(0.0f);
	physWorld.dynamicsWorld.addRigidBody(&playerBody);
	playerBody.setFriction(0.0f);


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
	
	Random rng(0);
	auto GetRandomModel = [&]() -> ModelHandle{
		auto entryIndex = rng.Range(0, assetManifest.entries.size() - 1);
		auto& entry = assetManifest.entries[entryIndex];
		auto modelHandle = LoadModel(entry.name);
		return modelHandle;
	};

	auto CreateRandomEntityAtCurrentPosition = [&]() {
		auto entity = ecsManager.CreateEntity();
		auto modelRenderComponent = ecsManager.CreateComponent<ModelRenderComponent>(entity);
		// modelRenderComponent->handle = GetRandomModel();
		modelRenderComponent->handle = LoadModel(ASSET_NAME(test));

		auto transform = ecsManager.CreateComponent<Transform3D>(entity);
		transform->translation = camera.position;
		// auto floorPosition = rng.PointInRectangle(roomFloor);
		// transform->translation = V3(floorPosition.x, 0.0f, floorPosition.y);

		auto physicsBox = ecsManager.CreateComponent<PhysicsDynamicBox>(entity, transform->translation, V3(1.0));
	};


	PointLight lights[64];
	for (U32 i = 0; i < 16; i++) {
		lights[i].position = rng.PointInCuboid(roomBounds);
		// lights[i].position = V3(0.5f + (1.0f * i), 0.5f, 0.5f);
		lights[i].SetRadius(16.0f);
		lights[i].color = V3(0.35) * rng.vector3();
		AddLight(&lights[i]);
	}



	//PhysicsDynamicBox box0(V3(1.0));
	//PhysicsDynamicBox box1(V3(1.0));
	//PhysicsDynamicBox box2(V3(1.0));
	//PhysicsDynamicBox box3(V3(1.0));
	//physWorld.dynamicsWorld.addRigidBody(&box0.body);
	//physWorld.dynamicsWorld.addRigidBody(&box1.body);
	//physWorld.dynamicsWorld.addRigidBody(&box2.body);
	//physWorld.dynamicsWorld.addRigidBody(&box3.body);

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
		static bool isEntityMenuActive = false;

		
		int keycode;
		while (keycode = PlatformPopKeypress()) {
			ProcessEditorKeypress(&editorWindow, keycode);
			if (keycode == KEY_F) {
				if (PlatformGetKey(KEY_LSHIFT)) {
					isEntityMenuActive = true;
				} else {
					CreateRandomEntityAtCurrentPosition();
				}
			}
		}


		if (PlatformGetButton(MOUSE_BUTTON_RIGHT)) 
			FPSCameraControlUpdate(&camera);



		//auto linearVelocity = btv3(playerBody.getLinearVelocity());
		//// playerBody.getWorldTransform().getBasis().getEulerYPR(rotation.x, rotation.y, rotation.z);
		//FPSPlayerControls(&linearVelocity, camera.yaw, &controllerParams, (float)deltaTime);
		//// std::cout << linearVelocity;
		//
		//playerBody.setLinearVelocity(v3bt(linearVelocity));


		//// playerBody.getWorldTransform().getRotation().setEuler(rotation.x, rotation.y, rotation.z);

		physWorld.Update();

		//btVector3 pos = playerBody.getWorldTransform().getOrigin();
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

		//auto& crateModel = GetModel(crateModelHandle);
		//auto& rockModel = GetModel(rockModelHandle);

		auto& testMaterial = GetMaterial(testMaterialHandel);
		auto& materialShader = GetShader(materialShaderHandle);

		ecsManager.ProcessComponent<PhysicsDynamicBox>([&ecsManager](const Entity& entity, PhysicsDynamicBox* component) {
			auto transform = ecsManager.GetComponent<Transform3D>(entity);
			auto& origin = component->body->getWorldTransform().getOrigin();
			transform->translation.x = origin.x();
			transform->translation.y = origin.y();
			transform->translation.z = origin.z();
		});

		GFX3D::SetMaterialShader(materialShader);
		ecsManager.ProcessComponent<ModelRenderComponent>([&ecsManager](const Entity& entity, ModelRenderComponent* component) {
			auto& model = GetModel(component->handle);
			auto transform = ecsManager.GetComponent<Transform3D>(entity);
			GFX3D::AddModel(model, transform->translation);
		});

		GFX3D::SetMaterial((Material*)&testMaterial);
		GFX3D::AddMesh(mesh);
		GFX3D::AddMesh(mesh, Vector3(0.0f, 16.0f, 0.0f), V3(90.0f, 0.0f, 0.0f));
		GFX3D::AddMesh(mesh, Vector3(16.0, 0.0f, 0.0f), V3(0.0f, 0.0f, 90.0f));

		GFX3D::EndFrame();

		physWorld.DebugDraw();

		ImGui::BeginFrame();
		ShowEditorWindow(&editorWindow);
		ImGui::Begin("ECSWindowTest");
		ShowEntityPanel(&ecsManager);
		ImGui::End();
		ImGui::EndFrame();

		CheckAssetFolderForChanges(&editorWindow);
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