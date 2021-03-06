#pragma once

typedef unsigned int GLuint;
typedef unsigned int ILuint;
class Resource;

namespace TextureLoader
{
	bool SaveToDds(char* buffer, int size, const char* name);
	GLuint LoadToMemory(char* buffer, int size, int* w = nullptr, int* h = nullptr);
	//GLuint GenBufferID(ILuint imageId, int* w, int* h);
};

