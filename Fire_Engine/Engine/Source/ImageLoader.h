#pragma once

#include<vector>
#include<string>
typedef unsigned int GLuint;
class Resources;

class ImageLoader
{
	GLuint LoadToMemory(char* buffer, int size, int* w = nullptr, int* h = nullptr);
	void SaveDDS(char* buffer, int size, const char* fileName);

	void Import(char* buffer, int bSize, Resources* res);

	void TakeScreenshot(int frameBuffer);
};

