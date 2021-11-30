#pragma once
#include "Tab.h"

class QuadTreeTab : public Tab
{
public:
	QuadTreeTab();
	virtual ~QuadTreeTab() {};

	void Draw() override;
};