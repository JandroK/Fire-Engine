#pragma once
#include "Tab.h"
#include <string>
#include <vector>

class GameObject;

class Inspector : public Tab
{
public:
	Inspector();
	virtual ~Inspector() {};

	void Draw() override;
	void CalculateMaxWidth(std::vector<std::string> list, int &width);

	GameObject* gameObjectSelected = nullptr;

private:
	std::string DrawList(const char* label, std::vector<std::string>* list, std::string &item, int width);

	std::vector<std::string> tags;
	std::vector<std::string> layers;

	// This values isn't hardcoded!, this values have been extracted from the "CalculateMaxWidth" function
	int maxWidthTag = 125;
	int maxWidthLayers = 146;
};