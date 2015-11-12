#pragma once
#include "downloader.h"

class CMyDownloader : public CDownloader
{
	void OnStartDownload( char* filename );
	void OnProgress( ULONG ulProgress, ULONG ulProgressMax );
};