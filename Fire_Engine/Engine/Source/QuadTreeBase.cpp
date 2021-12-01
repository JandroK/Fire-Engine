#include "QuadTreeBase.h"
#include "Glew/include/glew.h"
#include "MeshRenderer.h"

QuadTreeBase::QuadTreeBase(AABB aabb)
{
	ReGenerateRoot(aabb);
}

QuadTreeBase::~QuadTreeBase()
{
	DeleteRoot();
	goStatics.clear();
}

// Delete old root and regenerate new because his limts has changed
void QuadTreeBase::ReGenerateRoot(AABB limits)
{
	DeleteRoot();
	root = new QT_Node(limits);
	root->quadTree = this;
}

void QuadTreeBase::DeleteRoot()
{
	if (root != nullptr)
		delete root;
}

float3* QuadTreeBase::GetRootBoundingBox()
{
	float3 points[8];
	root->boundingBox.GetCornerPoints(points);
	return points;
}

// Draw the bounding box of the root and their nodes recursively 
void QuadTreeBase::Draw(QT_Node* node)
{
	if (drawQuadTree && !goStatics.empty())
	{
		float3 points[8];
		node->boundingBox.GetCornerPoints(points);
		DrawBoundingBoxes(points);
		if (node->HasChildrens())
		{
			for (int i = 0; i < 4; i++)
			{
				Draw(node->childrens[i]);
			}
		}
	}
}

void QuadTreeBase::ReCalculateRootLimits()
{
	// Calculate the min and max point of all static GO for generate the global bounding box
	if (!goStatics.empty())
	{
		std::list<GameObject*>::iterator it = goStatics.begin();
		float3 minPoint = static_cast<MeshRenderer*>((*it)->GetComponent(ComponentType::MESHRENDERER))->globalAABB.minPoint;
		float3 maxPoint = static_cast<MeshRenderer*>((*it)->GetComponent(ComponentType::MESHRENDERER))->globalAABB.maxPoint;
		it++;

		for (it; it != goStatics.end(); it++)
		{
			minPoint = minPoint.Min(static_cast<MeshRenderer*>((*it)->GetComponent(ComponentType::MESHRENDERER))->globalAABB.minPoint);
			maxPoint = maxPoint.Max(static_cast<MeshRenderer*>((*it)->GetComponent(ComponentType::MESHRENDERER))->globalAABB.maxPoint);
		}

		ReGenerateRoot(AABB(minPoint, maxPoint));
		numSubDivisions = 0;

		// Add Game Object
		for (std::list<GameObject*>::iterator it = goStatics.begin(); it != goStatics.end(); it++)
		{
			root->Insert((*it));
		}
	}	
}

// Function used for draw bounding box (as AABB/OBB)
void QuadTreeBase::DrawBoundingBoxes(float3* points, float3 color)
{
	glColor3fv(&color.x);
	glLineWidth(2.f);
	glBegin(GL_LINES);

	for (int i = 0; i < 24; i++)
	{
		glVertex3fv(&points[index[i]].x);
	}

	glEnd();
	glLineWidth(1.f);
	glColor3f(1.f, 1.f, 1.f);
}

QT_Node::QT_Node(AABB& aabb)
{
	boundingBox = aabb;
}

QT_Node::~QT_Node()
{
	if (HasChildrens())
	{
		for (int i = 0; i < 4; i++)
		{
			if (childrens[i] != nullptr)
			{
				delete childrens[i];
				childrens[i] = nullptr;
			}
		}
	}
}

void QT_Node::Insert(GameObject* go)
{
	// Calculate depth level
	QT_Node* node = this;
	int depth = 0;
	while (node->parent != nullptr)
	{
		depth++;
		node = node->parent;
	}
	if (depth > quadTree->numSubDivisions) quadTree->numSubDivisions = depth;

	// If I have not been split and I can still fit more objects
	if (!HasChildrens() && (objects.size() < quadTree->maxGObyNode || depth >= quadTree->maxDivisions))
		objects.push_back(go);
	else
	{
		if (!HasChildrens() && quadTree->numSubDivisions < quadTree->maxDivisions)
			CreateNodes();

		objects.push_back(go);
		ReDistributeChilds();
	}
}

void QT_Node::CreateNodes()
{
	AABB aabb;
	float3 newCenter[4];
	float3 center = boundingBox.CenterPoint();
	float3 size = boundingBox.HalfSize();
	size.y *= 2;
	float sizeX = size.x * 0.5f;
	float sizeZ = size.z * 0.5f;

	newCenter[0] = { center.x - sizeX, center.y, center.z + sizeZ };	// NW
	newCenter[1] = { center.x + sizeX, center.y, center.z + sizeZ };	// NE
	newCenter[2] = { center.x - sizeX, center.y, center.z - sizeZ };	// SW
	newCenter[3] = { center.x + sizeX, center.y, center.z - sizeZ };	// SE

	for (int i = 0; i < 4; i++)
	{
		aabb.SetFromCenterAndSize(newCenter[i], size);
		childrens[i] = new QT_Node(aabb);
		childrens[i]->quadTree = quadTree;
		childrens[i]->parent = this;
	}
}

void QT_Node::ReDistributeChilds()
{
	// Redistribute all game objects in the nodes
	for (std::list<GameObject*>::iterator it = objects.begin(); it != objects.end();)
	{
		AABB globalAABB(static_cast<MeshRenderer*>((*it)->GetComponent(ComponentType::MESHRENDERER))->globalAABB);

		// Check with how many nodes the object intersects 
		bool intersects[4];
		for (int i = 0; i < 4; ++i)
			intersects[i] = childrens[i]->boundingBox.Intersects(globalAABB);

		// If bounding box of object has intersect with all of them, we go to the next
		if (intersects[0] && intersects[1] && intersects[2] && intersects[3]) ++it;  
		else
		{
			for (int i = 0; i < 4; ++i)
				if (intersects[i]) childrens[i]->Insert((*it));
			it = objects.erase(it);
		}
	}
}
