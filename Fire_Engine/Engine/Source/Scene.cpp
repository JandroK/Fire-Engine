#include "Application.h"
#include "Scene.h"
#include "Globals.h"

//Components
#include "Component.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "ComponentCamera.h"

//Modules
#include "Input.h"
#include "Editor.h"
#include "ResourceManager.h"
#include "Camera3D.h"

#include "GameObject.h"
#include "Inspector.h"
#include "ResourceMesh.h"

#include"MathGeoLib/include/Math/Quat.h"

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
	GameObject* sceneCamera = CreateGameObject("MainCamera");
	sceneCamera->AddComponent(ComponentType::CAMERA);
	mainCamera = static_cast<ComponentCamera*>(sceneCamera->GetComponent(ComponentType::CAMERA));
	mainCamera->SetIsMainCamera(true);

	return true;
}

bool Scene::Start()
{
	// Import BakerHouse by default
	app->resourceManager->ImportFile("BakerHouse.fbx");

	// Edit his tramsformation because his original transforms are breken (scale = (100,100,100) and rotate 90�)
	Transform* transformChimney = root->GetChildrens()[1]->GetChildrens()[0]->transform;
	Transform* transformBakerhouse = root->GetChildrens()[1]->GetChildrens()[1]->transform;
	Transform* parentTransform = root->GetChildrens()[1]->transform;

	float3 size(1, 1, 1);
	Quat rotationQuat(0, 0, 0, 1);

	transformChimney->SetTransformMatrix(transformChimney->GetPosition(), rotationQuat, size, parentTransform);
	transformBakerhouse->SetTransformMatrix(transformBakerhouse->GetPosition(), rotationQuat, size, parentTransform);

	// We extract the camera from the list of root children for a moment and we return it to put 
	// so that it appears first in the hierarchy 
	// it would be easier to create after the "bakehouse" but we need to create it in the Init to define your framebuffer 
	root->EraseChildren(root->FindChildren(root->GetChildrens()[0]));
	root->AttachChild(mainCamera->GetOwner());
	Transform* transformCamera = static_cast<Transform*>(mainCamera->GetOwner()->GetComponent(ComponentType::TRANSFORM));
	transformCamera->SetPosition(float3(0, 1, -12));

	return true;
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

	UpdateGameObjects();

	return update_status::UPDATE_CONTINUE;
}

bool Scene::CleanUp()
{
	LOG(LogType::L_NO_PRINTABLE, "Freeing Level Manager");
	//Delete all gameObjects
	delete root;

	return true;
}

GameObject* Scene::CreateGameObject(const char* name, GameObject* parent)
{
	GameObject* obj = new GameObject(name);

	if (parent != nullptr)
	{
		parent->AddChildren(obj);
		obj->SetParent(parent);
	}
	else
	{
		root->AddChildren(obj);
		obj->SetParent(root);
	}

	return obj;
}

GameObject* Scene::CreatePrimitive(const char* name, Mesh* mesh)
{
	GameObject* primitive = new GameObject(name);
	primitive->SetParent(root);

	MeshRenderer* meshRenderer;
	meshRenderer = static_cast<MeshRenderer*>(primitive->AddComponent(ComponentType::MESHRENDERER));
	meshRenderer->SetMesh(mesh);

	root->AddChildren(primitive);

	return nullptr;
}

void Scene::Destroy(GameObject* obj)
{
	// Deselect actual gameObjectSelected
	static_cast<Inspector*>(app->editor->GetTab(TabType::INSPECTOR))->gameObjectSelected = nullptr;

	// First, must unpin the object from the children list of his father, after will can delete object
	for (std::vector<GameObject*>::iterator i = obj->GetParent()->GetBeginChildren(); i != obj->GetParent()->GetEndChildren(); ++i)
	{
		if (*i._Ptr == obj)
		{
			obj->SetIndex(i);
			break;
		}
	}

	delete obj;
	obj = nullptr;
}

void Scene::UpdateGameObjects()
{
	RecursiveUpdate(root);
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