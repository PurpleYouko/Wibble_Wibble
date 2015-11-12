#include "stdafx.h"
#include "vfs.h"

cVFS::cVFS():idx(0)
{
}
cVFS::~cVFS()
{
	CloseIndex();
}

bool cVFS::OpenIndex(char* filename)
{
	struct stat st;
	int ret = stat("ROOT.VFS", &st);
	if(ret != 0)
	{
		FILE *fh;
		fopen_s(&fh, "ROOT.VFS", "w");
		fclose(fh);
	}
	idx = ::OpenVFS(filename, "r+");
	return (idx != 0);
}

void cVFS::CloseIndex()
{
	::CloseVFS(this->idx);
	this->idx = NULL;
}

unsigned long cVFS::GetVFSCount()
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::VGetVfsCount(this->idx);
}

int cVFS::GetVfsNames(char** names, unsigned long maxcount, short maxlen)
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::VGetVfsNames(this->idx, names, maxcount, maxlen);
}

unsigned long cVFS::GetFileCount(const char* vfsfile)
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::VGetFileCount(this->idx, vfsfile);
}

int cVFS::GetFileNames(const char* name, char** files, unsigned long &maxfiles, unsigned long maxlen)
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::VGetFileNames(this->idx, name, files, maxfiles, maxlen);
}

void cVFS::GetFileInfo(const char* filename, VFileInfo* info, bool CalcCrc)
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::VGetFileInfo(this->idx, filename, info, CalcCrc);
}

unsigned long cVFS::AddFile(const char* insertvfs, const char* localpath, const char* vfspath, unsigned long attribute, BYTE EncryptionType, BYTE Compression, bool bUseDel)
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::VAddFile(this->idx, insertvfs, localpath, vfspath, attribute, ComputeCrc(localpath), EncryptionType, Compression, bUseDel);
}

bool cVFS::FileExists(const char* filename)
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::VFileExists(this->idx, filename);
}

bool cVFS::FileExistsInVfs(const char* filename)
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::VFileExistsInVfs(this->idx, filename);
}

unsigned long cVFS::RemoveFile(const char* filename)
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::VRemoveFile(this->idx, filename);
}

short cVFS::GetError()
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::VGetError();
}

unsigned long cVFS::GetTotFileCount()
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::VGetTotFileCount(this->idx);
}

unsigned long cVFS::GetCurVersion()
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::VGetCurVersion(this->idx);
}

void cVFS::SetCurVersion(unsigned long newver)
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::VSetCurVersion(this->idx, newver);
}

unsigned long cVFS::GetStdVersion()
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::VGetStdVersion(this->idx);
}

void cVFS::SetStdVersion(unsigned long newver)
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::VSetStdVersion(this->idx, newver);
}

bool cVFS::AddVfs(const char* vfsname)
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::VAddVfs(this->idx, vfsname);
}

VFileHandle *cVFS::OpenFile(const char* path)
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::VOpenFile(path, this->idx);
}

void cVFS::CloseFile(VFileHandle handle)
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::VCloseFile(&handle);
}

size_t cVFS::FGetSize(VFileHandle handle)
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::vfgetsize(&handle);
}

size_t cVFS::FRead(void* buffer, unsigned long size, unsigned long count, VFileHandle handle)
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::vfread(buffer, size, count, &handle);
}

unsigned long cVFS::FSeek(VFileHandle handle, unsigned long offset, unsigned long origin)
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::vfseek(&handle, offset, origin);
}

void* cVFS::FGetData(size_t *size, VFileHandle handle)
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::vfgetdata(size, &handle);
}

unsigned long cVFS::FTell(VFileHandle handle)
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::vftell(&handle);
}

unsigned long cVFS::ComputeCrc(const char* filename)
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	return ::ComputeCrc(this->idx, filename);
}
int cVFS::AddPatch(char *filename)
{
	if(!idx)
	{
		OpenIndex("data.idx");
	}
	if(ExtractFile(filename) == -1)
	{
		return 0;
	}
	std::vector<std::string>::iterator i;
	for(i = patchfiles.begin(); i != patchfiles.end(); ++i)
	{
		std::string vfsfile = *i;
		vfsfile.erase(0, 5);
		if(this->FileExists(vfsfile.c_str()))
		{
			this->RemoveFile(vfsfile.c_str());
		}
		std::string localpath = *i;
		this->AddFile("3DDATA.VFS", localpath.c_str(), vfsfile.c_str(), (this->GetCurVersion()+1));
	}
	patchfiles.clear();
	DeleteDirectory("temp");
	return 1;
}
BOOL cVFS::IsDots(const TCHAR* str)
{
	if(_tcscmp(str,".") && _tcscmp(str,"..")) return FALSE;
	return TRUE;
}
BOOL cVFS::DeleteDirectory(const TCHAR* sPath)
{
	HANDLE hFind; // file handle
	WIN32_FIND_DATA FindFileData;

	TCHAR DirPath[MAX_PATH];
	TCHAR FileName[MAX_PATH];

	_tcscpy(DirPath,sPath);
	_tcscat(DirPath,"\\*"); // searching all files
	_tcscpy(FileName,sPath);
	_tcscat(FileName,"\\");

	// find the first file
	hFind = FindFirstFile(DirPath,&FindFileData);
	if(hFind == INVALID_HANDLE_VALUE)
		return FALSE;
	_tcscpy(DirPath,FileName);

	bool bSearch = true;
	while(bSearch)
	{ // until we find an entry
		if(FindNextFile(hFind,&FindFileData))
		{
			if(IsDots(FindFileData.cFileName))
				continue;
			_tcscat(FileName,FindFileData.cFileName);
			if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
				{
					_chmod(FileName, _S_IWRITE);
					::SetFileAttributesA(FileName, FILE_ATTRIBUTE_NORMAL);
				}
				// we have found a directory, recurse
				if(!DeleteDirectory(FileName))
				{
					FindClose(hFind);
					return FALSE; // directory couldn't be deleted
				}
				// remove the empty directory
				RemoveDirectory(FileName);
				_tcscpy(FileName,DirPath);
			}
			else
			{
				if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
				{
					// change read-only file mode
					_chmod(FileName, _S_IWRITE);
					::SetFileAttributesA(FileName, FILE_ATTRIBUTE_NORMAL);
				}
				if(!DeleteFile(FileName))
				{ // delete the file
					FindClose(hFind);
					return FALSE;
				}
				_tcscpy(FileName,DirPath);
			}
		}
		else
		{
			// no more files there
			if(GetLastError() == ERROR_NO_MORE_FILES)
				bSearch = false;
			else
			{
				// some error occurred; close the handle and return FALSE
				FindClose(hFind);
				return FALSE;
			}
		}
	}
	FindClose(hFind); // close the file handle
	return RemoveDirectory(sPath); // remove the empty directory
}