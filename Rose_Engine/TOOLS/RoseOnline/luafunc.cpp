#include "stdafx.h"
#include "luafunc.h"
#include "roseonline.h"

int lua_GetPatcherVersion( lua_State *L )
{
	lua_pushnumber( L, PATCHER_VERSION );
	return 1;
}

int lua_GetVfsVersion( lua_State *L )
{
	int retval = vfs->GetCurVersion();
	if( retval == 0 )
		MessageBox( 0, "Error", "Failed to get game data version", MB_OK );
	lua_pushnumber( L, retval );
	return 1;
}

int lua_DoPatchFile( lua_State *L )
{
	const char* path = lua_tostring( L, 1 );
	if( vfs->AddPatch((char*)path) == 0 ){
		MessageBox( 0, "Error", "Failed to patch", MB_OK );
	}
	//else{
	//	if (ifsupdateid>0){
	//		char buffer[ MAX_PATH ];
	//		sprintf( buffer, "data.idx%i", ifsupdateid );
	//		dMyDownloader.DownloadFile( buffer,"data.idx");
	//	}
	//}
	lua_pushnumber( L, 0 );
	return 1;
}

int lua_GetFileChecksum( lua_State *L )
{
	const char* path = lua_tostring( L, 1 );
	lua_pushnumber( L, vfs->ComputeCrc(path) );
	return 1;
}

int lua_RunFile( lua_State *L )
{
	const char* path = lua_tostring( L, 1 );
	PROCESS_INFORMATION pi;
	STARTUPINFO si = { 0 };
	CreateProcessA( 0, (char*)path, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi );
	return 0;
}

int lua_SetRosePath( lua_State *L )
{
	const char* ip = lua_tostring( L, 1 );
	strcpy( cpStartStr, ip );
	int port = (int)lua_tonumber( L, 2 );
	cpStartPortStr = port;
	return 0;
}

int lua_DownloadFile( lua_State *L )
{
	const char* path = lua_tostring( L, 1 );
	char buffer[ 128 ];
	sprintf( buffer, "%s", path );
	if( dMyDownloader.DownloadFile( buffer, buffer ) ) {
//		DecompressFile( "temp.bin", (char*)path );
		lua_pushnumber( L, 1 );
	} else {
		lua_pushnumber( L, 0 );
	}
	return 1;
}

int lua_DownloadFile2( lua_State *L )
{
	const char* path = lua_tostring( L, 1 );
	const char* lpath = lua_tostring( L, 2 );
	char buffer[ 128 ];
	sprintf( buffer, "%s", path );
	char buffer2[ 128 ];
	sprintf( buffer2, "%s", lpath );
	if( dMyDownloader.DownloadFile( buffer, buffer2) ) {
		//		DecompressFile( "temp.bin", (char*)path );
		if(!strcmp(buffer2, "NewTriggerVFS.dll"))
		{
			STARTUPINFO siStartupInfo = {sizeof(siStartupInfo)}; 
			PROCESS_INFORMATION piProcessInfo;
			if(!CreateProcess(NULL, "Updater.exe NewTriggerVFS.dll TriggerVFS.dll RoseOnline.exe", NULL, NULL, false, NULL, NULL, NULL, &siStartupInfo, &piProcessInfo))
			{
				char buff[256];
				memset(buff, 0, 256);
				sprintf(buff, "Error could not open: Updater.exe, error code: 0x%X 0x%x", GetLastError(), errno);
				MessageBox(NULL, buff, "Error", MB_OK | MB_ICONERROR);
				DeleteFile("NewTriggerVFS.dll");
				ExitProcess(-1);
			}
			ExitProcess(0);
		}
		lua_pushnumber( L, 1 );
	} else {
		if(!strcmp(buffer2, "NewTriggerVFS.dll"))
		{
			STARTUPINFO siStartupInfo = {sizeof(siStartupInfo)}; 
			PROCESS_INFORMATION piProcessInfo;
			if(!CreateProcess(NULL, "Updater.exe NewTriggerVFS.dll TriggerVFS.dll RoseOnline.exe", NULL, NULL, false, NULL, NULL, NULL, &siStartupInfo, &piProcessInfo))
			{
				char buff[256];
				memset(buff, 0, 256);
				sprintf(buff, "Error could not open: Updater.exe, error code: 0x%X 0x%x", GetLastError(), errno);
				MessageBox(NULL, buff, "Error", MB_OK | MB_ICONERROR);
				DeleteFile("NewTriggerVFS.dll");
				ExitProcess(-1);
			}
			ExitProcess(0);
		}
		lua_pushnumber( L, 0 );
	}
	return 1;
}

int lua_SetNewsItem( lua_State *L )
{
	int id = (int)lua_tonumber( L, 1 );
	const char* name = lua_tostring( L, 2 );
	const char* url = lua_tostring( L, 3 );
	
	strcpy( cpNews[ id ][ 0 ], name );
	strcpy( cpNews[ id ][ 1 ], url );

	return 0;
}

int lua_DeleteFile( lua_State *L )
{
	const char* path = lua_tostring( L, 1 );
	remove( path );
	return 0;
}

int lua_UpdatePatcher( lua_State *L )
{
	unsigned char path[ 0x200 ];
	PROCESS_INFORMATION pi;
	STARTUPINFO si = { 0 };
	sprintf( (char*)path, "launcher.exe /update" );
	CreateProcessA( 0, (char*)path, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi );
	ExitProcess( 0 );
	return 0;
}

int lua_SetVfsVersion( lua_State *L )
{
	int newver = (int)lua_tonumber( L, 1);
	vfs->SetCurVersion(newver);
	return 0;
}