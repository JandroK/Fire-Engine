#include "Resource.h"
#include"Globals.h"

Resource::Resource(ResourceType type) : assetsFile(""), libraryFile(""), type(type), referenceCount(0)
{
}

Resource::~Resource()
{
	assetsFile.clear();
	libraryFile.clear();
	type = ResourceType::UNKNOWN;

	if (referenceCount != 0)
		LOG(LogType::L_NORMAL, "DELETING RESOURCE WITH MULTIPLE REFERENCE COUNTS");

	referenceCount = 0;
}

void Resource::SetAssetsPath(const char* _aPath)
{
	assetsFile = ASSETS_FOLDER;
	assetsFile += _aPath;
}

void Resource::SetLibraryPath(const char* _lPath)
{
	//libraryFile = LIBRARY_FOLDER;
	libraryFile = _lPath;
}

const char* Resource::GetFormatExt()
{
	switch (type)
	{
	case ResourceType::MESH: return ".fme";
	default: return ".unknown";
	}
}
