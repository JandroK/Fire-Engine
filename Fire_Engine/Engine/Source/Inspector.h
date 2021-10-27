#pragma once
#include "Tab.h"

class GameObject;

class Inspector : public Tab
{
public:
	Inspector();
	virtual ~Inspector();

	void Draw() override;

	GameObject* gameObjectSelected;
};