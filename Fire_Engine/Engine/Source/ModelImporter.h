#pragma once

class Resource;

namespace ModelImporter
{
	void Import(const char* fullPath, char* buffer, int bSize, Resource* res);
}