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
	}	
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
			}
		}
	}
}
