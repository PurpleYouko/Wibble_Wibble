#include "stdafx.h"
#include  "downloader.h"
#include "Callbackimplementation.h"

void CDownloader::SetBaseRemotePath( char* baseremotepath )
{
	strcpy( m_pcBaseRemotePath, baseremotepath );
}

bool CDownloader::DownloadFile( char* remotepath, char* localpath, bool AddBase )
{
	OnStartDownload( localpath );
	char buffer[ MAX_PATH ];
	if( AddBase )
		sprintf( buffer, "%s%s", m_pcBaseRemotePath, remotepath );
	else
		strcpy( buffer, remotepath );
	remove( localpath );
	DeleteUrlCacheEntry( buffer );
	CBSCallbackImpl tmp( this );
	return ( URLDownloadToFile( 0, buffer, localpath, 0, &tmp ) == S_OK );
}