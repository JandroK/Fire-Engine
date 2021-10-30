#pragma once
#include "Component.h"

class Texture;

class Material : public Component
{
public:
	Material(GameObject* obj);
	virtual ~Material();

	void OnEditor() override;
	int GetTextureID();

	Texture* texture = nullptr;
private:
	bool viewWithCheckers = false;
};