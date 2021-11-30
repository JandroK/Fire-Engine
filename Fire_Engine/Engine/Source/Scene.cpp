#include "Application.h"
#include "Scene.h"
#include "Globals.h"

//Components
#include "Component.h"
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
#include "Material.h"
#include "ResourceTexture.h"

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
	GameObject* sceneCamera = CreateGameObjectEmpty("MainCamera");
	sceneCamera->AddComponent(ComponentType::CAMERA);
	mainCamera = static_cast<ComponentCamera*>(sceneCamera->GetComponent(ComponentType::CAMERA));
	mainCamera->SetIsMainCamera(true);

	jsonFile.FileToValue(SCENE_CONFIG);

	return true;
}

bool Scene::Start()
{
	// Import BakerHouse by default
	app->resourceManager->ImportFile("BakerHouse.fbx");

	// Edit his tramsformation because his original transforms are breken (scale = (100,100,100) and rotate 90º)
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
	transformCamera->SetPosition(float3(0,1,-12));
	transformCamera->SetTransformMFromM(transformCamera->GetLocalTransform());

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

void Scene::CreateCamera()
{
	GameObject* camera = CreateGameObjectEmpty("Camera");
	camera->AddComponent(ComponentType::CAMERA);
	Transform* transformCamera = static_cast<Transform*>(camera->GetComponent(ComponentType::TRANSFORM));
	transformCamera->SetPosition(float3(0, 3.5f, -12));
	transformCamera->SetEulerRotaion(float3(6, 0, 0));
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

	return nullptr;
}

void Scene::Destroy(GameObject* obj)
{
	// Deselect actual gameObjectSelected
	app->editor->SetGameObjectSelected(nullptr);

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

	MeshRenderer* mesh;
	Material* material;

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
				tmp.SetJString(tmp.ValueToObject(tmp.GetRootValue()),  "Mesh", parentGO->name.c_str());
				break;

			case ComponentType::MATERIAL:
				material = static_cast<Material*>(parentGO->GetComponent(ComponentType::MATERIAL));
				tmp.SetJString(tmp.ValueToObject(tmp.GetRootValue()), "Material", material->texture->path.c_str());
				break;

			default:
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

	rootFile = jsonFile.GetRootValue();

	rootGO = jsonFile.GetChild(rootFile, "GameObjects");
	//rootGO=
	LoadGameObject(rootGO.GetChild(rootGO.GetRootValue(), "Root"));

	loadSceneRequest = false;

	return true;
}

GameObject* Scene::LoadGameObject(JsonParser parent)
{
	std::string num;
	Transform* transform;
	std::string convert;

	std::string name=parent.JsonValToString("name");
	GameObject* gamObj = new GameObject(name.c_str());
	gamObj->tag= parent.JsonValToString("tag");
	gamObj->layer= parent.JsonValToString("layer");
	gamObj->active= parent.JsonValToBool("active");
	gamObj->isStatic= parent.JsonValToBool("isStatic");
	gamObj->isSelected = parent.JsonValToBool("isSelected");
	gamObj->SetShowChildrens( parent.JsonValToBool("showChildrens"));
	gamObj->SetPendingToDelete( parent.JsonValToBool("pendingToDelete"));

	LoadComponents(parent, num, gamObj, transform);

	return gamObj;
}

void Scene::LoadComponents(JsonParser& parent, std::string& num, GameObject*& gamObj, Transform*& transform)
{
	JsonParser components = parent.GetChild(parent.GetRootValue(), "components");
	JsonParser tmp = components;

	for (int i = 0; i < 4; i++)
	{
		num = "Component " + std::to_string(i);
		if (components.ExistChild(components.GetRootValue(), num.c_str()))
		{
			tmp = components.GetChild(components.GetRootValue(), num.c_str());
			switch ((ComponentType)tmp.JsonValToNumber("Type"))
			{
			case ComponentType::TRANSFORM:
				gamObj->AddComponent(ComponentType::TRANSFORM);
				transform = static_cast<Transform*>(gamObj->GetComponent(ComponentType::TRANSFORM));
				transform->SetLocalTransform(strMatrixToF4x4(tmp.JsonValToString("LocalTransform")));
				transform->SetLocalTransform(strMatrixToF4x4(tmp.JsonValToString("GlobalTransform")));

				gamObj;
				//gamObj->GetCompoments().end(). = ;

				break;
			case ComponentType::MESHRENDERER:
				gamObj->AddComponent(ComponentType::MESHRENDERER);

				break;
			case ComponentType::MATERIAL:
				gamObj->AddComponent(ComponentType::MATERIAL);

				break;
			default:
				break;
			}

			//gamObj->AddComponent();

		}
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