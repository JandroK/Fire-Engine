#pragma once
#include <list>
#include "GameObject.h"
#include "Math/float3.h"
#include "Geometry/AABB.h"
#include "Geometry/LineSegment.h"

class QT_Node;

class QuadTreeBase
{
public:
	QuadTreeBase(AABB aabb = AABB(float3(-10, -10, -10),float3(10, 10, 10)));
	~QuadTreeBase();

	void Draw(QT_Node* node);
	void ReGenerateRootLimits(AABB limits);
	void DeleteRoot();

	void AddGameObject(GameObject* obj) { goStatics.push_back(obj); };
	void RemoveGameObject(GameObject* obj) { goStatics.remove(obj); };
	int GetNumGOStatics() { return goStatics.size(); };

	void DrawBoundingBoxes(float3* points, float3 color = float3::one);

	bool drawQuadTree = false;
	QT_Node* root = nullptr;

private:
	// List of all static gameObjects of the scene
	std::list<GameObject*> goStatics;

	int index[24] =
	{ 0, 2, 2, 6, 6, 4, 4, 0,
	  0, 1, 1, 3, 3, 2, 4, 5,
	  6, 7, 5, 7, 3, 7, 1, 5 
	};
};

// This class represents each partition of its parent's space
class QT_Node
{
public:
	QT_Node(AABB& aabb);
	~QT_Node();

	bool HasChildrens() { return (childrens[0] != nullptr) ? true : false; };

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
	unsigned int numSubDivisions = 0;
};