#pragma once

#include "stdafx.h"

extern class cVFS *vfs;

int ExtractFile( char* filename );

class cVFS
{
public:
	cVFS();
	~cVFS();

	bool OpenIndex(char* filename);
	void CloseIndex();
	unsigned long GetVFSCount();
	int GetVfsNames(char** names, unsigned long maxcount, short maxlen);
	unsigned long GetFileCount(const char* vfsfile);
	int GetFileNames(const char* name, char** files, unsigned long &maxfiles, unsigned long maxlen);
	void GetFileInfo(const char* filename, VFileInfo* info, bool CalcCrc);
	unsigned long AddFile(const char* insertvfs, const char* localpath, const char* vfspath, unsigned long attribute, BYTE EncryptionType = 0, BYTE Compression = 0, bool bUseDel = false);
	bool FileExists(const char* filename);
	bool FileExistsInVfs(const char* filename);
	unsigned long RemoveFile(const char* filename);
	short GetError();
	unsigned long GetTotFileCount();
	unsigned long GetCurVersion();
	void SetCurVersion(unsigned long newver);
	unsigned long GetStdVersion();
	void SetStdVersion(unsigned long newver);
	bool AddVfs(const char* vfsname);
	VFileHandle* OpenFile(const char* path);
	void CloseFile(VFileHandle handle);
	size_t FGetSize(VFileHandle handle);
	size_t FRead(void* buffer, unsigned long size, unsigned long count, VFileHandle handle);
	unsigned long FSeek(VFileHandle handle, unsigned long offset, unsigned long origin);
	void* FGetData(size_t *size, VFileHandle handle);
	unsigned long FTell(VFileHandle handle);
	unsigned long ComputeCrc(const char* filename);
	int AddPatch(char *filename);
	BOOL IsDots(const TCHAR* str);
	BOOL DeleteDirectory(const TCHAR* sPath);
	std::vector<std::string> patchfiles;
private:
	VHANDLE idx;
};