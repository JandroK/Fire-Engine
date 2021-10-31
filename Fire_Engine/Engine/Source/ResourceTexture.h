#pragma once
#include"Resource.h"
#include"Color.h"

struct Rect
{
	int x, y, w, h;
	Rect(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}
	Rect() : x(0), y(0), w(0), h(0) {}
	bool Contains(Rect b);
};

typedef unsigned int GLuint;


class Texture : public Resource
{
public:
	Texture(std::string path, std::string name = "");
	~Texture() {};

	bool LoadToMemory() override;
	bool UnloadFromMemory() override;
	Rect GetTextureChunk(Rect area);

public:
	int texWidth = 0;
	int texHeight = 0;

	GLuint textureID = -1;
	Color color;

	std::string path;
	std::string name;
};
