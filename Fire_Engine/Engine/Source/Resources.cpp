#include "Resources.h"
#include"Globals.h"

Resources::Resources(int _uid, Resources::Type _type) : uid(_uid), assetsFile(""), libraryFile(""), type(_type),
referenceCount(0)
{

}

Resources::~Resources()
{
	uid = 0;
	assetsFile.clear();
	libraryFile.clear();
	type = Type::UNKNOWN;

	if (referenceCount != 0)
		LOG(LogType::L_NORMAL, "DELETING RESOURCE WITH MULTIPLE REFERENCE COUNTS");

	referenceCount = 0;
}

void Resources::SetAssetsPath(const char* _aPath)
{
	assetsFile = _aPath;
}

void Resources::SetLibraryPath(const char* _lPath)
{
	libraryFile = _lPath;
}
