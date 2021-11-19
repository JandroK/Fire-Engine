#pragma once
#include "Tab.h"

class GameTab : public Tab
{
public:
	GameTab();
	virtual ~GameTab() {};

	void Draw() override;
	void RecalculateAspectRatio();

	ImVec2 lastViewportSize;
};

