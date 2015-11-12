#pragma once
#include "stdafx.h"
#include "vfs.h"

class CDownloader
{
public:
	void SetBaseRemotePath( char* baseremotepath );

	bool DownloadFile( char* remotepath, char* localpath, bool AddBase = true );

	virtual void OnStartDownload( char* filename ) = 0;
	virtual void OnProgress( ULONG ulProgress, ULONG ulProgressMax ) = 0;

protected:
	char m_pcBaseRemotePath[ MAX_PATH ];
};