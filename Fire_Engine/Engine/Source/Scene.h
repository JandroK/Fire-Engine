#pragma once
#include "Module.h"

#include<vector>
#include "Transform.h"

class GameObject;
class Mesh;
class ComponentCamera;

class Scene : public Module
{
public:
	Scene(Application* app, bool start_enabled = true);
	virtual ~Scene() {};

	bool Init() override;
	void CreateCamera();
	bool Start() override;

	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;

	bool CleanUp() override;

	GameObject* CreateGameObjectEmpty(const char* name);
	GameObject* CreateGameObjectChild(const char* name, GameObject* parent);
	GameObject* CreateGameObjectParent(const char* name, GameObject* child);
	GameObject* CreatePrimitive(const char* name, Mesh* mesh);
	void SaveSceneRequest() { saveSceneRequest = true; }
	void LoadSceneRequest() { loadSceneRequest = true; }
	GameObject* root;
	ComponentCamera* mainCamera = nullptr;

	std::vector<GameObject*> destroyList;

private:
	void Destroy(GameObject* obj);

	void UpdateGameObjects();
	bool SaveScene();
	void SaveGameObjects(GameObject* parent, JsonParser& rootFile);
	bool LoadScene();

	GameObject* LoadGameObject(JsonParser root);

	void LoadComponents(JsonParser& parent, std::string& num, GameObject*& gamObj, Transform*& transform);

	float4x4 strMatrixToF4x4(const char* convert);

	void RecursiveUpdate(GameObject* parent);

	JsonParser jsonFile;
	JSON_Value* rootFile;
	JsonParser rootGO;

	bool saveSceneRequest = false;
	bool loadSceneRequest = false;
};