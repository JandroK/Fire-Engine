#pragma once

typedef unsigned int GLuint;
class Resource;

namespace TextureLoader
{
	void SaveToDss(char* buffer, int size, const char* name);
	GLuint LoadToMemory(char* buffer, int size, int* w = nullptr, int* h = nullptr);
};

