#include "stdafx.h"
#include "mydownloader.h"
#include "roseonline.h"

void CMyDownloader::OnStartDownload( char* filename )
{
	_snprintf( cpStatusText ,sizeof(cpStatusText), "Downloading file '%hs'", filename );
	//sprintf( cpStatusText, "Downloading file '%hs'", filename );
	uPrg2Val = 0;
	uPrg2Max = 0;
	RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
}

void CMyDownloader::OnProgress( ULONG ulProgress, ULONG ulProgressMax )
{
	uPrg2Val = ulProgress;
	uPrg2Max = ulProgressMax;
	RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
}