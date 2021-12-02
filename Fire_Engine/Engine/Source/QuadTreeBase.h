#pragma once
#include <list>
#include "GameObject.h"
#include "Math/float3.h"
#include "Geometry/AABB.h"
#include "Geometry/LineSegment.h"
#include <map>

class QT_Node;

class QuadTreeBase
{
public:
	QuadTreeBase(AABB aabb = AABB(float3(0, 0, 0),float3(0, 0, 0)));
	~QuadTreeBase();

	void Draw(QT_Node* node);
	void ReCalculateRootLimits();

	void ReGenerateRoot(AABB limits);
	void DeleteRoot();

	void AddGameObject(GameObject* obj) { goStatics.push_back(obj); ReCalculateRootLimits(); };
	void RemoveGameObject(GameObject* obj) { goStatics.remove(obj); ReCalculateRootLimits(); };

	float3* GetRootBoundingBox();
	int GetNumGOStatics() { return goStatics.size(); };

	void DrawBoundingBoxes(float3* points, float3 color = float3(1, 0.9f, 0));

	QT_Node* root = nullptr;
	
	bool drawQTree = false;
	bool pickQTree = false;

	int maxDivisions = 10;	// Maximum number of depth  
	int maxGObyNode = 1;	// Maximum number of objects by node 
	unsigned int numSubDivisions = 0;

private:
	// List of all static gameObjects of the scene
	std::list<GameObject*> goStatics;

	int index[24] =
	{ 0, 2, 2, 6, 6, 4, 4, 0,
	  0, 1, 1, 3, 3, 2, 4, 5,
	  6, 7, 5, 7, 3, 7, 1, 5 
	};
};

class MeshRenderer;
// This class represents each partition of its parent's space
class QT_Node
{
public:
	QT_Node(AABB& aabb);
	~QT_Node();

	bool HasChildrens() { return (childrens[0] != nullptr) ? true : false; };

	void Insert(GameObject* go);
	void CreateNodes();
	void ReDistributeChilds();

	void CollectIntersections(std::map<float, MeshRenderer*>& objects, LineSegment ray);

public:
	// Each partition has 1 parent
	QT_Node* parent = nullptr;
	// Each partition has 4 partitions  
	QT_Node* childrens[4] = { nullptr, nullptr, nullptr, nullptr };

	// List of objects that this node contains
	std::list<GameObject*> objects;
	// Variable created to have direct access to root
	QuadTreeBase* quadTree = nullptr;

	AABB boundingBox;
};