#pragma once
#include "Tab.h"
#include <vector>

class Configuration : public Tab
{
public:
	Configuration();
	~Configuration();

	void Draw() override;

	void UpdateLogs();
	void PushBackLog(std::vector<float> *log, float current);

private:
	std::vector<float> fpsLog;
	std::vector<float> msLog;
	std::vector<float> memoryLog;
};