#pragma once
#include "PhysFS/include/physfs.h"

#include <vector>
#include <string>

enum class ImportType {
	NOTYPE = -1,
	MESH,
	TEXTURE,
};

namespace StringLogic {

	std::string FileNameFromPath(const char* path);
	std::string GlobalToLocalPath(const char* globalPath);
}

namespace FileSystem 
{
	ImportType GetTypeFromPath(const char* path);

	void FSInit();
	void FSDeInit();
	void CreateLibraryDirectories();

	// Utility functions
	bool AddPath(const char* path_or_zip);
	bool RemovePath(const char* dir);
	bool Exists(const char* file);
	bool CreateDir(const char* dir);
	bool IsDirectory(const char* file);
	std::vector<const char*> GetFilesFromFolder(const char* folder);

	std::string NormalizePath(const char* fullPath);
	std::string UnNormalizePath(const char* fullPath);

	std::string ExtractLocalDiskBackward(const char* fullPath);
	std::string ExtractLocalDiskForward(const char* fullPath);

	// Open for Read/Write
	unsigned int LoadToBuffer(const char* file, char** buffer);
	unsigned int Save(const char* file, const void* buffer, unsigned int size, bool append = false);
	void Copy(const char* source, const char* dest, char* buffer);

	bool Remove(const char* file);
	void GetFileName(const char* file, std::string& fileName, bool extension);
	std::string GetExtFromPath(const char* path);

	inline const char* GetBasePath() { return PHYSFS_getBaseDir(); }
	inline const char* GetWritePath() { return PHYSFS_getWriteDir(); }
	const char* GetReadPaths();

	void OnGui();
};