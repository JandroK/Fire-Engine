#include "Application.h"
#include "Scene.h"
#include "Globals.h"
#include "DTEngine.h"

//Components
#include "Component.h"
#include "MeshRenderer.h"
#include "ComponentCamera.h"
#include "C_RigidBody.h"

//Modules
#include "Input.h"
#include "Editor.h"
#include "ResourceManager.h"
#include "Camera3D.h"
#include "Renderer3D.h"
#include "Physics3D.h"

#include "GameObject.h"
#include "Inspector.h"
#include "ResourceMesh.h"
#include "Material.h"
#include "ResourceTexture.h"

#include "Texture.h"
#include "QuadTreeBase.h"
#include "Window.h"
#include "CarControls.h"

#include"MathGeoLib/include/Math/Quat.h"
#include "Bullet/include/BulletDynamics/Dynamics/btRigidBody.h"
#include "Bullet/include/BulletCollision/CollisionShapes/btCollisionShape.h"
#include "PhysVehicle3D.h"

Scene::Scene(Application* app, bool start_enabled) : Module(app, start_enabled), root(nullptr)
{
	name = "scene";
}

bool Scene::Init()
{
	LOG(LogType::L_NORMAL, "Creating Scene");
	//Change the name "Root" by the name of scene when it has
	root = new GameObject("Root");

	// Create GameCamera and asignt as main
	GameObject* sceneCamera = CreateGameObjectEmpty("MainCamera");
	sceneCamera->transform->SetWorldPosition(float3(15, 5,15));
	sceneCamera->transform->SetWorldRotation(Quat::FromEulerXYZ(0, 5 * RADTODEG, 0));
	sceneCamera->AddComponent(ComponentType::CAMERA);
	mainCamera = static_cast<ComponentCamera*>(sceneCamera->GetComponent(ComponentType::CAMERA));
	mainCamera->SetIsMainCamera(true);

	jsonFile.FileToValue(SCENE_CONFIG);

	return true;
}

bool Scene::Start()
{
	// Import BakerHouse by default
	app->resourceManager->ImportFile("Resources/street/street2.fbx");

	GameObject* plane = root->GetChildrens()[1]->FindChildren("Plane001");
	static_cast<Material*>(plane->GetComponent(ComponentType::MATERIAL))->texture = app->resourceManager->greenTexture;
	GameObject* object010 = root->GetChildrens()[1]->FindChildren("Object010");
	static_cast<Material*>(object010->GetComponent(ComponentType::MATERIAL))->texture = app->resourceManager->whiteTexture;

	std::vector<GameObject*> street = root->GetChildrens()[1]->GetChildrens();
	for (int i = 0; i < street.size(); i++)
	{
		street.at(i)->AddComponent(ComponentType::RIGIDBODY);
	}
	GameObject* line = root->GetChildrens()[1]->FindChildren("Line002");
	static_cast<C_RigidBody*>(plane->GetComponent(ComponentType::RIGIDBODY))->SetAsStatic();
	static_cast<C_RigidBody*>(line->GetComponent(ComponentType::RIGIDBODY))->SetAsStatic();
	static_cast<C_RigidBody*>(object010->GetComponent(ComponentType::RIGIDBODY))->SetAsStatic();

	// We extract the camera from the list of root children for a moment and we return it to put 
	// so that it appears first in the hierarchy 
	// it would be easier to create after the "bakehouse" but we need to create it in the Init to define your framebuffer 
	root->EraseChildren(root->FindChildren(root->GetChildrens()[0]));
	root->AttachChild(mainCamera->GetOwner());
	Transform* transformCamera = static_cast<Transform*>(mainCamera->GetOwner()->GetComponent(ComponentType::TRANSFORM));
	transformCamera->SetPosition(float3(-50.0f, 44.0f, 80.0f));
	transformCamera->SetEulerRotation(float3(145.0f, 30.0f, -160.0f));
	transformCamera->SetTransformMFromM(transformCamera->GetLocalTransform());

	StartSpheresP2P();
	StartVehiclesFleet();

	PCube ramp = PCube(float3(6,2,6), float3(-4, 0.1, 30));
	ramp.InnerMesh();
	ramp.mesh->LoadToMemory();
	ramp.mesh->GenerateBounds();

	GameObject* rampObj = CreatePrimitive("Ramp", ramp.mesh);
	rampObj->transform->SetWorldPosition(float3(-4, 0.1, 30));
	rampObj->transform->SetWorldRotation(Quat::FromEulerXYZ(-20 * DEGTORAD, 0, 0));
	rampObj->transform->UpdateTransform();

	rampObj->AddComponent(ComponentType::RIGIDBODY, 0);
	static_cast<C_RigidBody*>(rampObj->GetComponent(ComponentType::RIGIDBODY))->SetAsStatic();
	rampObj->AddComponent(ComponentType::MATERIAL);
	static_cast<Material*>(rampObj->GetComponent(ComponentType::MATERIAL))->texture = app->resourceManager->greenTexture;

	return true;
}

void Scene::StartSpheresP2P()
{
	PSphere spherePrim1 = PSphere();
	spherePrim1.InnerMesh();
	spherePrim1.mesh->LoadToMemory();
	spherePrim1.mesh->GenerateBounds();

	GameObject* sphere1 = CreatePrimitive("Sphere1_P2P", spherePrim1.mesh);
	sphere1->transform->SetWorldPosition(float3(-4, 2, -5));
	sphere1->transform->UpdateTransform();
	sphere1->AddComponent(ComponentType::RIGIDBODY, 1);

	PSphere spherePrim2 = PSphere();
	spherePrim2.InnerMesh();
	spherePrim2.mesh->LoadToMemory();
	spherePrim2.mesh->GenerateBounds();

	GameObject* sphere2 = CreatePrimitive("Sphere2_P2P", spherePrim2.mesh);
	sphere2->transform->SetWorldPosition(float3(0, 2, -5));
	sphere2->transform->UpdateTransform();
	sphere2->AddComponent(ComponentType::RIGIDBODY, 1);

	C_RigidBody* body1 = static_cast<C_RigidBody*>(sphere1->GetComponent(ComponentType::RIGIDBODY));
	C_RigidBody* body2 = static_cast<C_RigidBody*>(sphere2->GetComponent(ComponentType::RIGIDBODY));

	body1->constraintBodies.push_back(body2);
	body2->constraintBodies.push_back(body1);
	btVector3 center;
	float r1, r2;
	body1->GetBody()->getCollisionShape()->getBoundingSphere(center, r1);
	body2->GetBody()->getCollisionShape()->getBoundingSphere(center, r2);
	app->physics->AddConstraintP2P(*body1->GetBody(), *body2->GetBody(), float3(r1, r1, r1), float3(r2, r2, r2));
}

void Scene::StartVehiclesFleet()
{
	mainCar = new CarControls();
	std::string carName = "Car";
	for (int i = 0; i < 6; i++)
	{
		PCube cubePrim = PCube(float3(2 + i * 0.5f, 1 + i * 0.5f, 3 + i * 0.5f), float3(-18.f + i * 6, 1.5f, 3.f));
		cubePrim.InnerMesh();
		cubePrim.mesh->LoadToMemory();
		cubePrim.mesh->GenerateBounds();

		carName = "Car" + std::to_string(i + 1);
		GameObject* car = CreatePrimitive(carName.c_str(), cubePrim.mesh);
		car->transform->SetWorldPosition(float3(-18.f + i * 6, 1.5f, 3.f));
		car->transform->UpdateTransform();
		car->AddComponent(ComponentType::RIGIDBODY);

		C_RigidBody* bodyCar = static_cast<C_RigidBody*>(car->GetComponent(ComponentType::RIGIDBODY));
		bodyCar->SetMass(130);
		bodyCar->SetAsVehicle();
		if (i == 0)
		{
			bodyCar->GetVehicle()->mainV = true;
			mainCar->vehicle = bodyCar->GetVehicle();
		}
	}
}


update_status Scene::PreUpdate(float dt)
{
	if (destroyList.size() > 0) {
		for (size_t i = 0; i < destroyList.size(); ++i)
		{
			Destroy(destroyList[i]);
		}
		destroyList.clear();
	}
	return update_status::UPDATE_CONTINUE;
}

update_status Scene::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN && app->editor->GetGameObjectSelected() != nullptr)
		app->editor->GetGameObjectSelected()->Destroy();

	if (app->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN && app->editor->GetSelectedAsset() != nullptr)
		app->editor->DestroySelectedAsset();

	UpdateGameObjects();
	if(mainCar->vehicle != nullptr && DTEngine::state == DTGState::PLAY) mainCar->Update();

	return update_status::UPDATE_CONTINUE;
}

bool Scene::CleanUp()
{
	LOG(LogType::L_NO_PRINTABLE, "Freeing Level Manager");
	//Delete all gameObjects
	delete root;

	return true;
}

GameObject* Scene::CreateGameObjectEmpty(const char* name)
{
	GameObject* obj = new GameObject(name);
	root->AttachChild(obj);

	return obj;
}

GameObject* Scene::CreateGameObjectChild(const char* name, GameObject* parent)
{
	GameObject* obj = new GameObject(name);

	if (parent != nullptr)
		parent->AttachChild(obj);

	return obj;
}

GameObject* Scene::CreateGameObjectParent(const char* name, GameObject* child)
{
	GameObject* obj = new GameObject(name);

	child->GetParent()->AttachChild(obj);
	child->GetParent()->EraseChildren(child->GetParent()->FindChildren(child));
	obj->AttachChild(child);

	return obj;
}

GameObject* Scene::CreateCamera()
{
	GameObject* camera = CreateGameObjectEmpty("Camera");
	camera->AddComponent(ComponentType::CAMERA);
	Transform* transformCamera = static_cast<Transform*>(camera->GetComponent(ComponentType::TRANSFORM));
	transformCamera->SetPosition(float3(0, 3.5f, -12));
	transformCamera->SetEulerRotation(float3(6, 0, 0));
	return camera;
}

GameObject* Scene::CreatePrimitive(const char* name, Mesh* mesh)
{
	GameObject* primitive = new GameObject(name);
	primitive->SetParent(root);

	MeshRenderer* meshRenderer;
	meshRenderer = static_cast<MeshRenderer*>(primitive->AddComponent(ComponentType::MESHRENDERER));
	meshRenderer->SetMesh(mesh);
	meshRenderer->SetBoundingBoxes(mesh);

	root->AddChildren(primitive);

	return primitive;
}

void Scene::Destroy(GameObject* obj)
{
	// Deselect actual gameObjectSelected
	app->editor->SetGameObjectSelected(nullptr);

	if (!obj->IsRoot())
	{
		// First, must unpin the object from the children list of his father, after will can delete object
		for (std::vector<GameObject*>::iterator i = obj->GetParent()->GetBeginChildren(); i != obj->GetParent()->GetEndChildren(); ++i)
		{
			if (*i._Ptr == obj)
			{
				obj->SetIndex(i);
				app->camera->quadTree->RemoveGameObject(obj);
				break;
			}
		}
	}

	delete obj;
	obj = nullptr;
}

void Scene::UpdateGameObjects()
{
	RecursiveUpdate(root);

	if (saveSceneRequest)SaveScene();
	if (loadSceneRequest)LoadScene();
}

bool Scene::SaveScene()
{
	LOG(LogType::L_NORMAL, "Saving scene");

	rootFile = jsonFile.GetRootValue();

	JsonParser scene = jsonFile.SetChild(rootFile, "GameObjects");

	SaveGameObjects(root, scene.SetChild(scene.GetRootValue(), root->name.c_str()));

	jsonFile.FileSerialization(rootFile, SCENE_CONFIG);
	saveSceneRequest = false;
	return true;
}

void Scene::SaveGameObjects(GameObject* parentGO, JsonParser& node)
{
	//node.SetChild(node.GetRootValue(), "Child");
	std::string num, strTmp;
	JsonParser& child = node;
	Transform* transform;
	float4x4 localTransform,globalTransform;

	LOG(LogType::L_NORMAL, (std::string("Save ") + parentGO->name).c_str());

	MeshRenderer* mesh;
	Material* material;
	ComponentCamera* camera;
	C_RigidBody* body;

	node.SetJNumber(node.ValueToObject(node.GetRootValue()), "UID", parentGO->GetUID());
	node.SetJString(node.ValueToObject(node.GetRootValue()), "name", parentGO->name.c_str());
	node.SetJBool(node.ValueToObject(node.GetRootValue()), "IsRoot", parentGO->IsRoot());

	node.SetJString(node.ValueToObject(node.GetRootValue()), "tag", parentGO->tag.c_str());
	node.SetJString(node.ValueToObject(node.GetRootValue()), "layer", parentGO->layer.c_str());


	node.SetJBool(node.ValueToObject(node.GetRootValue()), "active", parentGO->active);
	node.SetJBool(node.ValueToObject(node.GetRootValue()), "isStatic", parentGO->isStatic);
	node.SetJBool(node.ValueToObject(node.GetRootValue()), "isSelected", parentGO->isSelected);


	node.SetJBool(node.ValueToObject(node.GetRootValue()), "showChildrens", parentGO->GetShowChildrens());
	node.SetJBool(node.ValueToObject(node.GetRootValue()), "pendingToDelete", parentGO->GetPendingToDelete());

	//Create father Components
	JsonParser components = node.SetChild(node.GetRootValue(), "components");
	JsonParser tmp=node;
	for (size_t i = 0; i < parentGO->GetCompoments().size(); i++)
	{
		// Create Child of component
		num = "Component " + std::to_string(i);
		tmp = components.SetChild(components.GetRootValue(), num.c_str());

		tmp.SetJNumber(tmp.ValueToObject(tmp.GetRootValue()), "Type", (int)parentGO->GetCompoments().at(i)->GetType());
		tmp.SetJBool(tmp.ValueToObject(tmp.GetRootValue()), "active", parentGO->GetCompoments().at(i)->active);

		LOG(LogType::L_NORMAL, (std::string("Save ") + num +" of "+ parentGO->name).c_str());


		switch ((ComponentType)parentGO->GetCompoments().at(i)->GetType())
		{
			case ComponentType::TRANSFORM:
				num = "";
				transform = static_cast<Transform*>(parentGO->GetComponent(ComponentType::TRANSFORM));
				localTransform = transform->GetLocalTransform();
				globalTransform = transform->GetGlobalTransform();
				for (int i = 0; i < 4; i++)
					for (int j = 0; j < 4; j++)
					{
						num += std::to_string(localTransform.At(i, j)), strTmp+= std::to_string(globalTransform.At(i, j));
						strTmp += ",";
						num += ",";
					}
			
				tmp.SetJString(tmp.ValueToObject(tmp.GetRootValue()), "LocalTransform", num.c_str());
				tmp.SetJString(tmp.ValueToObject(tmp.GetRootValue()), "GlobalTransform", strTmp.c_str());

				break;

			case ComponentType::MESHRENDERER:
				mesh = static_cast<MeshRenderer*>(parentGO->GetComponent(ComponentType::MESHRENDERER));
				tmp.SetJString(tmp.ValueToObject(tmp.GetRootValue()),  "Mesh", mesh->GetMesh()->GetAssetPath());
				tmp.SetJString(tmp.ValueToObject(tmp.GetRootValue()), "LibraryPath", mesh->GetMesh()->GetLibraryPath());
				tmp.SetJNumber(tmp.ValueToObject(tmp.GetRootValue()), "PrimType", mesh->GetMesh()->primType);
				tmp.SetJNumber(tmp.ValueToObject(tmp.GetRootValue()), "SizeX", mesh->globalOBB.Size().x);
				tmp.SetJNumber(tmp.ValueToObject(tmp.GetRootValue()), "SizeY", mesh->globalOBB.Size().y);
				tmp.SetJNumber(tmp.ValueToObject(tmp.GetRootValue()), "SizeZ", mesh->globalOBB.Size().z);
				tmp.SetJNumber(tmp.ValueToObject(tmp.GetRootValue()), "Radius", mesh->globalOBB.r.MaxElementXZ());
				
				break;

			case ComponentType::MATERIAL:
				material = static_cast<Material*>(parentGO->GetComponent(ComponentType::MATERIAL));
				tmp.SetJString(tmp.ValueToObject(tmp.GetRootValue()), "Material", material->texture->path.c_str());
				tmp.SetJString(tmp.ValueToObject(tmp.GetRootValue()), "LibraryPath", material->texture->GetLibraryPath());
				break;

			case ComponentType::CAMERA:
				camera = static_cast<ComponentCamera*>(parentGO->GetComponent(ComponentType::CAMERA));
				tmp.SetJBool(tmp.ValueToObject(tmp.GetRootValue()), "isMainCamera", camera->GetIsMainCamera());
				tmp.SetJBool(tmp.ValueToObject(tmp.GetRootValue()), "showFrustrum", camera->GetShowFrustrum());
				break;

			case ComponentType::RIGIDBODY:
				body = static_cast<C_RigidBody*>(parentGO->GetComponent(ComponentType::RIGIDBODY));
				tmp.SetJNumber(tmp.ValueToObject(tmp.GetRootValue()), "mass", body->GetMass());
				tmp.SetJNumber(tmp.ValueToObject(tmp.GetRootValue()), "collision", (int)body->GetCollisionType());
				tmp.SetJBool(tmp.ValueToObject(tmp.GetRootValue()), "isKinematic", body->isKinematic);
				if (body->GetVehicle() != nullptr)
				{
					tmp.SetJBool(tmp.ValueToObject(tmp.GetRootValue()), "vehicle", true);
					tmp.SetJBool(tmp.ValueToObject(tmp.GetRootValue()), "mainV", body->GetVehicle()->mainV);
				}
				else
					tmp.SetJBool(tmp.ValueToObject(tmp.GetRootValue()), "vehicle", false);
				
				num = "";
				for (int i = 0; i < body->constraintBodies.size(); i++)
				{
					num += std::to_string(body->constraintBodies.at(i)->GetOwner()->GetUID());
					num += ",";
				}
				if(num != "") tmp.SetJString(tmp.ValueToObject(tmp.GetRootValue()), "Constraints", num.c_str());
				else tmp.SetJString(tmp.ValueToObject(tmp.GetRootValue()), "Constraints", "");

				break;
		}
		parentGO->GetCompoments().at(i)->GetType();
	}

	for (size_t i = 0; i <= parentGO->GetChildrens().size(); i++) 
	{
		num ="Child "+ std::to_string(i);
		if (parentGO->GetChildrens().size() > i) 
			SaveGameObjects(parentGO->GetChildrens()[i], child.SetChild(child.GetRootValue(), num.c_str()));
	}
}

bool Scene::LoadScene()
{
	LOG(LogType::L_NORMAL, "Loading configurations");

	Destroy(root);

	rootFile = jsonFile.GetRootValue();
	mainCar->vehicle = nullptr;

	rootGO = jsonFile.GetChild(rootFile, "GameObjects");
	root=LoadGameObject(rootGO.GetChild(rootGO.GetRootValue(), "Root"));
	LoadConstraints();

	loadSceneRequest = false;

	return true;
}

GameObject* Scene::LoadGameObject(JsonParser parent, GameObject* father)
{
	std::string num;
	std::string name=parent.JsonValToString("name");

	GameObject* gamObj = new GameObject(name.c_str(), parent.JsonValToNumber("UID"));

	LOG(LogType::L_NORMAL, (std::string("\n Loading ") + name).c_str());

	if (!parent.JsonValToBool("IsRoot"))gamObj->SetParent(father);

	gamObj->tag= parent.JsonValToString("tag");
	gamObj->layer= parent.JsonValToString("layer");
	gamObj->active= parent.JsonValToBool("active");
	gamObj->isStatic= parent.JsonValToBool("isStatic");
	gamObj->isSelected = parent.JsonValToBool("isSelected");
	gamObj->SetShowChildrens( parent.JsonValToBool("showChildrens"));
	gamObj->SetPendingToDelete( parent.JsonValToBool("pendingToDelete"));

	LoadComponents(parent, num, gamObj);
	int count = 0;
	num = "Child " + std::to_string(count);
	while(parent.ExistChild(parent.GetRootValue(), num.c_str()))
	{
		gamObj->AttachChild(LoadGameObject(parent.GetChild(parent.GetRootValue(), num.c_str()), gamObj));
		++count;
		num = "Child "+ std::to_string(count);
	}
	app->renderer3D->renderQueue.clear();

	return gamObj;
}

void Scene::LoadConstraints()
{
	for (int i = 0; i < app->physics->GetBodies().size(); i++)
	{
		for (int j = 0; j < app->physics->GetBodies().at(i)->bodiesUIDs.size(); j++)
		{
			for (int k = 0; k < app->physics->GetBodies().size(); k++)
			{
				if (app->physics->GetBodies().at(k)->GetCollisionType() != CollisionType::CAMERA)
				{
					int UID1 = app->physics->GetBodies().at(k)->GetOwner()->GetUID();
					int UID2 = app->physics->GetBodies().at(i)->bodiesUIDs.at(j);
					if (UID1 == UID2)
					{
						app->physics->GetBodies().at(i)->constraintBodies.push_back(app->physics->GetBodies().at(k));
						app->physics->GetBodies().at(i)->AddConstraintP2P(app->physics->GetBodies().at(k));
						break;
					}
				}
			}
		}		
	}
}

void Scene::LoadComponents(JsonParser& parent, std::string& num, GameObject*& gamObj)
{
	Transform* transform;
	MeshRenderer* meshRender;
	Mesh* mesh;
	Material* material;
	ComponentCamera* camera;
	C_RigidBody* body;
	float3 size = float3::one;
	double uuiidd;
	LOG(LogType::L_NORMAL, "Loading Components \n");
	std::string debugPath;
	std::string debugUID;

	JsonParser components = parent.GetChild(parent.GetRootValue(), "components");
	JsonParser tmp = components;

	for (int i = 0; i < 4; i++)
	{
		num = "Component " + std::to_string(i);
		LOG(LogType::L_NORMAL, (std::string("Loading ") + num).c_str());

		if (components.ExistChild(components.GetRootValue(), num.c_str()))
		{
			tmp = components.GetChild(components.GetRootValue(), num.c_str());
			switch ((ComponentType)(int)tmp.JsonValToNumber("Type"))
			{
			case ComponentType::TRANSFORM:
				gamObj->transform->SetGlobalTransform(strMatrixToF4x4(tmp.JsonValToString("GlobalTransform")));
				if (!gamObj->IsRoot()) gamObj->transform->SetTransformMFromM(gamObj->transform->GetGlobalTransform());
				gamObj->transform->SetLocalTransform(strMatrixToF4x4(tmp.JsonValToString("LocalTransform")));

				//gamObj->transform = transform;

				break;
			case ComponentType::MESHRENDERER:
				gamObj->AddComponent(ComponentType::MESHRENDERER);
				meshRender = static_cast<MeshRenderer*>(gamObj->GetComponent(ComponentType::MESHRENDERER));

				if (meshRender != NULL && tmp.JsonValToNumber("PrimType") == -1)
				{
					mesh = new Mesh();
					debugPath = tmp.JsonValToString("LibraryPath");
					mesh->SetAssetsPath(tmp.JsonValToString("Mesh"));
					mesh->SetLibraryPath(debugPath.c_str());
					mesh->LoadFromFME(debugPath.c_str());

					meshRender->SetMesh(mesh);
					meshRender->SetBoundingBoxes(mesh);
				}
				else
				{
					size = float3(tmp.JsonValToNumber("SizeX"), tmp.JsonValToNumber("SizeY"), tmp.JsonValToNumber("SizeZ"));
					mesh = app->editor->LoadPrimitive(tmp.JsonValToNumber("PrimType"), size, tmp.JsonValToNumber("Radius"), size.y);
					meshRender->SetMesh(mesh);
					meshRender->SetBoundingBoxes(mesh);
				}
				
				meshRender->SetOwner(gamObj);

				break;
			case ComponentType::MATERIAL:
				gamObj->AddComponent(ComponentType::MATERIAL);
				material = static_cast<Material*>(gamObj->GetComponent(ComponentType::MATERIAL));
				material->active = tmp.JsonValToBool("active");
				material->texture = new Texture(tmp.JsonValToString("Material"), gamObj->name);
				material->texture->SetLibraryPath(tmp.JsonValToString("LibraryPath"));
				material->texture->path = material->texture->GetLibraryPath();
				//material->texture->SetAssetsPath();
				material->SetOwner(gamObj);
				material->texture->LoadToMemory();

				break;
			case ComponentType::CAMERA:
				
				gamObj->AddComponent(ComponentType::CAMERA);
				camera = static_cast<ComponentCamera*>(gamObj->GetComponent(ComponentType::CAMERA));
				mainCamera = camera;
				camera->ReGenerateFrameBuffer(app->window->GetWindowWidth(), app->window->GetWindowHeight());
				//camera->SetOwner(gamObj);
				camera->SetIsMainCamera(tmp.JsonValToBool("isMainCamera"));
				camera->SetShowFrustrum(tmp.JsonValToBool("showFrustrum"));
				break;

			case ComponentType::RIGIDBODY:
				gamObj->AddComponent(ComponentType::RIGIDBODY, tmp.JsonValToNumber("collision"));
				body = static_cast<C_RigidBody*>(gamObj->GetComponent(ComponentType::RIGIDBODY));
				body->SetMass(tmp.JsonValToNumber("mass"));
				body->isKinematic = tmp.JsonValToBool("isKinematic");
				if (tmp.JsonValToBool("vehicle") == true)
				{
					body->RecoverVehicle(tmp.JsonValToBool("mainV"));
				}
				debugUID = tmp.JsonValToString("Constraints");
				if (debugUID != "")
				{
					body->bodiesUIDs.clear();
					body->bodiesUIDs = strMatrixToIntList(debugUID.c_str());
				}
				break;
			}
		}
		else break;
	}

}

float4x4 Scene::strMatrixToF4x4(const char* convert)
{
	std::string text = convert;
	std::string delimiter = ",";
	std::vector<float> floatArray{};

	size_t pos = 0;
	while ((pos = text.find(delimiter)) != std::string::npos) {
		floatArray.push_back(stof(text.substr(0, pos)));
		text.erase(0, pos + delimiter.length());
	}

	float4x4 matrix;
	int count = 0;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{

			matrix.At(i, j) = floatArray.at(count);
			++count;
		}

	return matrix;
}

std::vector<int> Scene::strMatrixToIntList(const char* convert)
{
	std::string text = convert;
	std::string delimiter = ",";
	std::vector<int> floatArray{};

	size_t pos = 0;
	while ((pos = text.find(delimiter)) != std::string::npos) {
		floatArray.push_back(stoi(text.substr(0, pos)));
		text.erase(0, pos + delimiter.length());
	}

	return floatArray;
}

void Scene::RecursiveUpdate(GameObject* parent)
{
	if (parent->GetPendingToDelete())
	{
		destroyList.push_back(parent);
		return;
	}

	if (parent->active)
	{
		parent->Update();

		for (size_t i = 0; i < parent->GetChildrens().size(); i++)
		{
			RecursiveUpdate(parent->GetChildrens()[i]);
		}
	}
}