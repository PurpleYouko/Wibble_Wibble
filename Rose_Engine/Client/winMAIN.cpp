// Client.cpp : Defines the entry point for the application.


//*-------------------------------------------------------------------------------------------------------------------*
// 2005 / 06 / 20 : nAvy
// User-defined description of the preprocessor :
//  1. _TAIWAN  : TAIWAN, PHILIPHIN Country code when connecting to the login server (currently 950) to send a definition to distinguish
//  2. _DE		: The definition of what a duplicate version of the executable (KR) only applied.
//
// 20060703 raning
// Stdafx.h set preprocessor.
//*-------------------------------------------------------------------------------------------------------------------*

#define NDEBUG				//PY: this disables all instances of assert that come after this point
							//Might entirely screw up all the calls to JSingleton. We will have to see. commented out for now
#include "stdafx.h"

#include "CApplication.h"
#include "Game.h"

#include "Network\\CNetwork.h"
#include "Util\\VFSManager.h"
#include "Util\\SystemInfo.h"
#include "Util\\classCRC.h"
#include "CClientStorage.h"
#include "System/CGame.h"
#include "Interface/ExternalUI/CLogin.h"
#include "Debug.h"
#include "TriggerInfo.h"

#include "Country.h"
#include "MiniDumper.h"
#include "Util/Cencryptdecrypt.h"
#include "Hasher.h"


//*--------------------------------------------------------------------------------------*/
// 95,98, me from Unicode regard Api Wrapper dll Load
//HMODULE LoadUnicowsProc(void);
//HMODULE g_hUnicows = NULL;
//#ifdef _cplusplus
//extern "C" {
//#endif
//extern FARPROC _PfnLoadUnicows = (FARPROC) &LoadUnicowsProc;
//#ifdef _cplusplus
//}
//#endif
//
//
//// Unicode Load
//HMODULE LoadUnicowsProc(void)
//{
//	g_hUnicows = LoadLibraryA("unicows.dll");
//    return g_hUnicows;
//}
//*--------------------------------------------------------------------------------------*/
typedef struct sHashComp
{
	unsigned int h1;
	unsigned int h2;
	unsigned short h3;
} sHashComp;

typedef union uHash
{
	sHashComp hashVal;
	BYTE hashByte[ 20 ];
} uHash;

//-------------------------------------------------------------------------------------------------
bool Init_DEVICE (void)
{
	bool bRet = false;

	//--------------------------[ engine related ]-----------------------//
	::initZnzin();
	::openFileSystem("data.idx");


	::doScript("scripts/init.lua");

	t_OptionResolution Resolution = g_ClientStorage.GetResolution();
	::setDisplayQualityLevel( c_iPeformances[g_ClientStorage.GetVideoPerformance()] );

	g_ClientStorage.ApplyAntiAliasingOption( g_ClientStorage.GetVideoAntiAliasing() );

	if(!g_pCApp->IsFullScreenMode())
	{
		RECT ClientRt;
		GetClientRect(g_pCApp->GetHWND(),&ClientRt);
		::setScreen(ClientRt.right, ClientRt.bottom, Resolution.iDepth, g_pCApp->IsFullScreenMode() );
	}
	else
		::setScreen(g_pCApp->GetWIDTH(), g_pCApp->GetHEIGHT(), Resolution.iDepth, g_pCApp->IsFullScreenMode() );


	bRet = ::attachWindow((const void*)g_pCApp->GetHWND());


	CD3DUtil::Init( );

	g_pSoundLIST = new CSoundLIST( g_pCApp->GetHWND() );
	g_pSoundLIST->Load ( "3DDATA\\STB\\FILE_SOUND.stb" );

	return bRet;
}



//-------------------------------------------------------------------------------------------------
void Free_DEVICE (void)
{
	delete g_pSoundLIST;

	CD3DUtil::Free ();

	//--------------------------[ engine related ]-----------------------//
	::detachWindow();

	::closeFileSystem();
	::destZnzin();
}

#ifdef __ARCFOUR

bool	g_bArcfourSucessed = false;

// Load Arcfour Key.
DWORD WINAPI MyThreadProc( LPVOID param )
{

	g_bArcfourSucessed = false;

#ifdef _DEBUG

	// Read the key from the key file.

	int			nKeyLen = 0;
	Crypt		crypt;

	// Read and write the key file on the server.
	crypt = g_EncryptDecrypt.EncryptDataLoad_Server( "key.txt" );
	if( crypt.bKeyJudgment )
	{
		g_pCApp->SetArcfourKey( crypt.sCyrptKey.c_str(), crypt.size );
		g_bArcfourSucessed = true;
		return 0;
	}

	// Read the release of a key in the executable file.
	crypt = g_EncryptDecrypt.EncryptDataLoad( "TRose.exe" );

	if( crypt.bKeyJudgment )
	{
		g_pCApp->SetArcfourKey( crypt.sCyrptKey.c_str(), crypt.size );
		g_bArcfourSucessed = true;
		return 0;
	}

#else
	//07. 01. 17 - 
    //Kim, Joo - Hyun: Do not use for the time being po arc (packet headers are different, the server
	//If you cut off until modified by the frequent occurrence do not use it. Packet headers are used. However, as packets are not encrypted.)
	g_bArcfourSucessed = true;

#ifdef __RELEASE_TEST
	crypt = g_EncryptDecrypt.EncryptDataLoad_Server( "key.txt" );
	if( crypt.bKeyJudgment )
	{
		g_pCApp->SetArcfourKey( crypt.sCyrptKey.c_str(), crypt.size );
		g_bArcfourSucessed = true;
		return 0;
	}
#endif

#endif

	// Failed to read key.
	return 0;
}

#endif

//-------------------------------------------------------------------------------------------------
int APIENTRY WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
#ifdef _USE_BG
	//	SetExceptionReport();
#endif


#ifndef _DEBUG
	// *-------------------------------------------------------------------* //
	// 2005. 5. 6. Joe Ho-dong
	HANDLE AppMutex = NULL;

	AppMutex = ::OpenMutex( MUTEX_ALL_ACCESS, false, __BUILD_REGION_NAME );
	if( AppMutex != NULL )
	{
		::ReleaseMutex( AppMutex);
		::CloseHandle( AppMutex);
		MessageBox( NULL, "Prevention of double execution", "TRose", MB_OK | MB_ICONERROR);
		return 0;
	}
#endif

	// TSVFS.dll The CRC is checked hajang!
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __TEST
#else
#ifndef _DEBUG
	classCRC crc;
	std::string	m_csFileName;

	TCHAR tstrFileName[255];

	DWORD dwSize = 0;

	uHash hashOut;
	unsigned int hashlength = 0;


	GetModuleFileName(NULL, tstrFileName, 255);
	m_csFileName = tstrFileName;

///////////////<crc check tsvfs.dll> 
	// Garnet: it check TSVFS crc and i don't want it or runtime error happen on release configuration
	/**
	char TSVFSDLL[1024];

	GetModuleFileName( GetModuleHandle("TSVFS.dll"), TSVFSDLL, 1024);

	dwSize = crc.FileCRC32(TSVFSDLL);
	if(dwSize != 0x2F6AA378)
	{
		MessageBox( NULL, "Runtime 2 error!", "TRose", MB_OK | MB_ICONERROR);
		return 0;
	}
	**/
///////////////<\crc check tsvfs.dll>
#endif  //_DEBUG

#endif	//__TEST


	//-------------------------------------------------------------------------------
	/// Init System object
	//-------------------------------------------------------------------------------
	g_pCApp		= CApplication::Instance ();




#ifdef __ARCFOUR

#ifdef __ARCFOURLOG
	FILE * pFile = fopen("ArcfourLog.txt", "wt");
	if(pFile)	fprintf( pFile, "Start Log\n" );
	fclose(pFile);
#endif


	DWORD IDThread;
	HANDLE hThread;
	//Arcfour thread start.
	hThread = CreateThread( NULL,
		0,
		(LPTHREAD_START_ROUTINE)MyThreadProc,
		NULL,
		0,
		&IDThread );
#endif
	// *-------------------------------------------------------------------* //


	/*_CrtSetDbgFlag (
	_CRTDBG_ALLOC_MEM_DF |
	_CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode ( _CRT_ERROR,
	_CRTDBG_MODE_DEBUG);*/


	//-------------------------------------------------------------------------------
	/// Country code recognition Country code recognition
	//-------------------------------------------------------------------------------
	CCountry::GetSingleton().CheckCountry();

	//KeyCrypt Execution
	//if( !m_npKeyCrypt.InitKeyCrypt() )
	//	return 0;
	///m_npKeyCrypt.IsGetUse() = FALSE;


	g_SystemInfo.CollectingSystemInfo();
	int iWindowVersion = g_SystemInfo.GetWindowsVersion();



	//-------------------------------------------------------------------------------
	/// Init Trigger VFS
	//-------------------------------------------------------------------------------
	VHANDLE hVFS = OpenVFS( "data.idx", (iWindowVersion == WINDOWS_98) ? "r" : "mr" ); //hVFS now contains the entire VFS
	(CVFSManager::GetSingleton()).SetVFS( hVFS );			//sets m_hVFile = pVFile where pVfile is hVFS
	(CVFSManager::GetSingleton()).InitVFS( VFS_TRIGGER_VFS ); //sets VFS type. VFS_NORMAL (0) VFS_TRIGGER_VFS (1)


	 DWORD dwCurVersion = VGetCurVersion(hVFS);
	 char temp[32];
	 sprintf(temp, "%d", dwCurVersion);
	 ClientLog(LOG_DEBUG,"VFS current version %d",dwCurVersion );

	 // 07. 1. 18 - Kim, Joo - Hyun client stores the patch version.
	 CGame::GetInstance().SetCurVersion(temp);
	//-------------------------------------------------------------------------------
	/// Get Time
	//-------------------------------------------------------------------------------
	GetLocalTime(	&g_GameDATA.m_SystemTime );


	//-------------------------------------------------------------------------------
	/// Init System object
	//-------------------------------------------------------------------------------
	//	g_pCApp		= CApplication::Instance ();

#ifdef __ARCFOUR

	//Raeds Arcfour Key before creating CNetwork
	WaitForSingleObject( hThread, INFINITE );
	CloseHandle( hThread );

	//Arcfour key 
	//If reading from the end client.
	if( g_bArcfourSucessed == false )
	{
		(CVFSManager::GetSingleton()).ReleaseAll();
		g_pCApp->Destroy ();

		MessageBox(NULL, "Fail - 1004", "TRose", MB_OK | MB_ICONERROR);

		return 0;
	}

#endif
	g_pNet		= CNetwork::Instance (hInstance);


	g_pCRange	= CRangeTBL::Instance ();


	//-------------------------------------------------------------------------------
	/// Load Range table
	//-------------------------------------------------------------------------------
	if ( !g_pCRange->Load_TABLE ("3DDATA\\TERRAIN\\O_Range.TBL") ) 
	{
		g_pCApp->ErrorBOX ( "3DDATA\\TERRAIN\\O_Range.TBL file open error", CUtil::GetCurrentDir (), MB_OK);
		return 0;
	}


	//-------------------------------------------------------------------------------
	/// IP/Port Setting
	//-------------------------------------------------------------------------------
	g_GameDATA.m_wServerPORT = TCP_LSV_PORT;
	g_GameDATA.m_ServerIP.Set( TCP_LSV_IP );

	if ( !g_pCApp->ParseArgument( lpCmdLine ) ) 
	{
		MessageBox( NULL, "Arg error!", "TRose", MB_OK | MB_ICONERROR);
		return 0;
	}

	//-------------------------------------------------------------------------------
	/// Data generated on the resolution of the window are required during the load here.
	//-------------------------------------------------------------------------------
	g_TblResolution.Load2( "3DDATA\\STB\\RESOLUTION.STB",	false, false );
	g_TblCamera.Load2( "3DDATA\\STB\\LIST_CAMERA.STB" ,false, false );

	//-------------------------------------------------------------------------------
	/// Load the Data stored on the client.
	//-------------------------------------------------------------------------------
	g_ClientStorage.Load();

	//-------------------------------------------------------------------------------
	///The index of the adjusted previous option to import the resolution referring to g_TblResolution
	///The resolution is adjusted.
	//-------------------------------------------------------------------------------
#pragma message("You can run the stored resolution is determined by testing and initializing the message box does not work TriggerDetect Shall run")
	t_OptionResolution Resolution = g_ClientStorage.GetResolution();
	/// Check range values
	UINT iFullScreen = g_ClientStorage.GetVideoFullScreen();

	g_pCApp->SetFullscreenMode( iFullScreen );

	if(!CGame::GetInstance().GetCurVersion().empty())
		g_pCApp->CreateWND ("classCLIENT", CStr::Printf("%s [Ver. %s]", __BUILD_REGION_NAME, CGame::GetInstance().GetCurVersion().c_str()) , Resolution.iWidth, Resolution.iHeight,Resolution.iDepth, hInstance);
	else
		g_pCApp->CreateWND ("classCLIENT", __BUILD_REGION_NAME, Resolution.iWidth, Resolution.iHeight,Resolution.iDepth, hInstance);



#ifndef _DEBUG
	// Information collection system
	TI_ReadSysInfoFile ();

#endif


	// *-------------------------------------------------------------------* //
	g_pObjMGR = CObjectMANAGER::Instance ();
	g_pCApp->ResetExitGame();

	bool bDeviceInitialized = Init_DEVICE();
	ClientLog(LOG_DEBUG,"Device initialization called");

	if ( bDeviceInitialized ) 
	{
		CGame::GetInstance().GameLoop();		//game loop started. CGame.cpp
	}
	else
	{
		MessageBox( NULL, "Device error!", "TRose", MB_OK | MB_ICONERROR);
#ifdef _USE_BG
		ReportZnzinLog( "Failed to initialize. Do you want to transfer the cause?", 10 );
#endif
	}



	// *-------------------------------------------------------------------* //

	Free_DEVICE ();


	// *-------------------------------------------------------------------* //
	g_TblCamera.Free();
	g_TblResolution.Free();
	// *-------------------------------------------------------------------* //

	g_pCApp->Destroy ();
	g_pNet->Destroy ();

	//-------------------------------------------------------------------------------
	///2004/3/26/nAvy:Release Configuration Debug Mode(F5 Run)If you cancel at the login screen when running
	///Error Flies. Do not know why, but I just run, the (bat Or file ctrl+f5) Error The annanda.
	//-------------------------------------------------------------------------------
	g_pCRange->Destroy ();

	/*if( g_hUnicows )
	{
	FreeLibrary( g_hUnicows );
	g_hUnicows = NULL;
	}*/

#ifndef _DEBUG
	if (AppMutex)
	{
		::ReleaseMutex(AppMutex);
		::CloseHandle(AppMutex);
	}
#endif

	return 0;
}



extern std::string sCurVersion;








//-------------------------------------------------------------------------------------------------
