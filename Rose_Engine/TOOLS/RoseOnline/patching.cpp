#include "stdafx.h"
#include "patching.h"
#include "luafunc.h"
#include "roseonline.h"

LRESULT CALLBACK WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
    {
		case WM_MOUSEMOVE:
		{
			uUpdateBtnState = uRegisterBtnState = uForumBtnState = uDonateBtnState = uOptionsBtnState = uExitBtnState = 1;

			//exit button
			if( CheckCollision( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) ) == 2 )
			{
				if( uExitBtnState >= 1 )
				{
					uExitBtnState = 2;
					if( wParam == MK_LBUTTON )
						uExitBtnState = 3;
				}
				RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
			} else {
				if( uExitBtnState >= 2 )
					uExitBtnState = 1;
				RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
			}

			//playbutton
			if( CheckCollision( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) ) == 3 )
			{
				if( uPlayBtnState >= 1 )
				{
					uPlayBtnState = 2;
					if( wParam == MK_LBUTTON )
						uPlayBtnState = 3;
				}
				RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
			} else {
				if( uPlayBtnState >= 2 )
					uPlayBtnState = 1;
				RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
			}

			for( LONG i = 0; i < MAX_NEWSTITLES; i++ )
			{
				if( CheckCollision( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) ) == 100 + i )
					uNewsBarState[ i ] = 1;
				else
					uNewsBarState[ i ] = 0;
				RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
			}
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			switch(CheckCollision( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) ))
			{
			case 1:
				{
					SendMessage( hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0 );
				}
				break;
			case 2:
				{
					if( uExitBtnState >= 2 )
						uExitBtnState = 3;
					RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
				}
				break;
			case 3:
				{
					if( uPlayBtnState >= 2 )
						uPlayBtnState = 3;
					RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
				}
				break;
			case 4:
				{
					uUpdateBtnState = 2;
					RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
				}
				break;
			case 5:	// Options
				{
					uOptionsBtnState = 2;
					RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
				}
				break;
			case 6:	// Forum
				{
					uForumBtnState = 2;
					RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
				}
				break;
			case 7: // Register
				{
					uRegisterBtnState = 2;
					RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
				}
				break;
// 			case 8: // Donate
// 				{
// 					uDonateBtnState = 2;
// 					RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
// 				}
// 				break;
			}
/*
// 			if( CheckCollision( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) ) == 1 )
// 			{
// 				SendMessage( hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0 );
// 			} else if( CheckCollision( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) ) == 3 ) { // Play Button
// 				if( uPlayBtnState >= 2 )
// 					uPlayBtnState = 3;
// 				RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
// 			}else if( CheckCollision( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) ) == 2 ) { // Exit Button
// 				if( uExitBtnState >= 2 )
// 					uExitBtnState = 3;
// 				RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
// 			}else if( CheckCollision( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) ) == 4 ) {	// Manual Patch
// 				uUpdateBtnState = 3;
// 				RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
// 				RunManualPatch();
// 			}
*/
			return 0;
		}
		case WM_LBUTTONUP:
		{
			switch(CheckCollision( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) ))
			{
			case 2:
				{
					PostQuitMessage( 0 );
				}
				break;
			case 3:
				{
					if( uPlayBtnState != 0 )
					{
						LaunchRose( cpStartStr, cpStartPortStr );
						PostQuitMessage( 0 );
					}
				}
				break;
			case 4:
				{
					uUpdateBtnState = 1;
					RunManualPatch();
					RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
				}
				break;
			case 5:	// Options
				{
					uOptionsBtnState = 1;
					
					unsigned char path[ 0x200 ];
					PROCESS_INFORMATION pi;
					STARTUPINFO si = { 0 };
					sprintf( (char*)path, "TriggerDetect.exe");
					CreateProcessA( 0, (char*)path, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi );
					
					RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
				}
				break;
			case 6:	// Forum
				{
					uForumBtnState = 1;
					ShellExecute(NULL, "open", "http://www.adventureirose.com/forums", NULL, NULL, SW_SHOWNORMAL);
					RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
				}
				break;
			case 7: // Register
				{
					uRegisterBtnState = 1;
					ShellExecute(NULL, "open", "http://www.adventureirose.com/register.php", NULL, NULL, SW_SHOWNORMAL);
					RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
				}
				break;
// 			case 8: // Donate
// 				{
// 					uDonateBtnState = 1;
// 					ShellExecute(NULL, "open", "http://www.extremeirose.com/web/?ct=pre-donate", NULL, NULL, SW_SHOWNORMAL);
// 					RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
// 				}
// 				break;
			default:
				{
					for( LONG i = 0; i < MAX_NEWSTITLES; i++ )
					{
						if( CheckCollision( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) ) == 100 + i )
							ShellExecute(NULL, "open", cpNews[ i ][ 1 ], NULL, NULL, SW_SHOWNORMAL);
					}
				}
				break;
			}
/*
// 			if( CheckCollision( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) ) == 2 ) {	// Exit Button
// 				PostQuitMessage( 0 );
// 			} else if( CheckCollision( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) ) == 3 ) { // Play Button
// 				if( uPlayBtnState != 0 )
// 				{
// 					LaunchRose( cpStartStr, cpStartPortStr );
// 					PostQuitMessage( 0 );
// 				}
// 			} else {
// 				for( LONG i = 0; i < MAX_NEWSTITLES; i++ )
// 				{
// 					if( CheckCollision( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) ) == 100 + i )
// 						ShellExecute(NULL, "open", cpNews[ i ][ 1 ], NULL, NULL, SW_SHOWNORMAL);
// 				}
// 			}
*/
			return 0;
		}
		case WM_PAINT:
		{
			HWND hDesktop = ::GetDesktopWindow();
			HDC hdcScreen = ::GetDC(hDesktop);
			//HDC hdcScreen = ::GetWindowDC(hDesktop);
			// Getting the desktop DC and create a compatible DC to work on.
			HDC hdcCompatible = CreateCompatibleDC(hdcScreen);
			RECT r;
			GetWindowRect(hWnd, &r);
			BitBlt(hBackBuffer, 0,0, windowsizex, windowsizey, hdcScreen, r.left, r.top, SRCCOPY);
			AlphaBlend( hBackBuffer, 0, 0, windowsizex, windowsizey, hMyGraphics[ GFX_MAINWND ], 0, 0, windowsizex, windowsizey, m_bf);
			
			// Draw News
			for( LONG i = 0; i < MAX_NEWSTITLES; i++ )
			{
				if( uNewsBarState[ i ] == 1 )
				{
					BitBlt( hBackBuffer, newsx, newsy + ( i * 15 ), 276, 15, hMyGraphics[ GFX_NEWSBAROVER ], 0, 0, SRCCOPY );
					SetTextColor( hBackBuffer, 0xFFFFFE );//3a4a5a );
				} else {
					SetTextColor( hBackBuffer, 0x5a6a7a );
				}

				if( strcmp( cpNews[ i ][ 0 ], "" ) != 0 )
				{
					char buffer[ 1024 ];
					strcpy( buffer, "  - " );
					strcat( buffer, cpNews[ i ][ 0 ] );
					DrawString( buffer, newsx, newsy + ( i * 15 ), 276, 22, 0 );
				}
			}

			// Draw Buttons
			if( uPlayBtnState == 1 )
				BitBlt( hBackBuffer, 24, 341, 84, 49, hMyGraphics[ GFX_PLAYENABLED ], 0, 0, SRCCOPY );
			else if( uPlayBtnState == 2 )
				BitBlt( hBackBuffer, 24, 341, 84, 49, hMyGraphics[ GFX_PLAYOVER ], 0, 0, SRCCOPY );
			else if( uPlayBtnState == 3 )
				BitBlt( hBackBuffer, 24, 341, 84, 49, hMyGraphics[ GFX_PLAYDOWN ], 0, 0, SRCCOPY );
			else
				BitBlt( hBackBuffer, 24, 341, 84, 49, hMyGraphics[ GFX_PLAYDISABLED ], 0, 0, SRCCOPY );

			if( uExitBtnState == 1 )
				BitBlt( hBackBuffer, windowsizex-36, 0, 36, 15, hMyGraphics[ GFX_EXIT1 ], 0, 1, SRCCOPY );
			else if( uExitBtnState == 2 )
				BitBlt( hBackBuffer, windowsizex-36, 0, 36, 15, hMyGraphics[ GFX_EXIT2 ], 0, 1, SRCCOPY );
			else if( uExitBtnState == 3 )
				BitBlt( hBackBuffer, windowsizex-36, 0, 36, 15, hMyGraphics[ GFX_EXIT3 ], 0, 1, SRCCOPY );

			// Manual Update Button
			if( uUpdateBtnState == 1 )
				BitBlt( hBackBuffer, 108, 365, 115, 25, hMyGraphics[ GFX_MANUALNORM ], 0, 0, SRCCOPY );
			else
				BitBlt( hBackBuffer, 108, 365, 115, 25, hMyGraphics[ GFX_MANUALDOWN ], 0, 0, SRCCOPY );

			// Options Button
			if( uOptionsBtnState == 1 )
				BitBlt( hBackBuffer, 119, 11, 54, 22, hMyGraphics[ GFX_OPTIONSNORM ], 0, 0, SRCCOPY );
			else
				BitBlt( hBackBuffer, 119, 11, 54, 22, hMyGraphics[ GFX_OPTIONSDOWN ], 0, 0, SRCCOPY );

			// Donate Button
// 			if( uDonateBtnState == 1 )
// 				BitBlt( hBackBuffer, 282, 12, 50, 19, hMyGraphics[ GFX_DONATENORM ], 0, 0, SRCCOPY );
// 			else
// 				BitBlt( hBackBuffer, 282, 12, 50, 19, hMyGraphics[ GFX_DONATEDOWN ], 0, 0, SRCCOPY );

			// Forum Button
			if( uForumBtnState == 1 )
				BitBlt( hBackBuffer, 173, 12, 50, 19, hMyGraphics[ GFX_FORUMNORM ], 0, 0, SRCCOPY );
			else
				BitBlt( hBackBuffer, 173, 12, 50, 19, hMyGraphics[ GFX_FORUMDOWN ], 0, 0, SRCCOPY );

			// Register Button
			if( uRegisterBtnState == 1 )
				BitBlt( hBackBuffer, 225, 12, 56, 19, hMyGraphics[ GFX_REGNORM ], 0, 0, SRCCOPY );
			else
				BitBlt( hBackBuffer, 225, 12, 56, 19, hMyGraphics[ GFX_REGDOWN ], 0, 0, SRCCOPY );
			
			// Draw progress bars
			BitBlt( hBackBuffer, 21, 409, 377, 17, hMyGraphics[ GFX_PROGRESSEMPTY ], 0, 0, SRCCOPY );
			BitBlt( hBackBuffer, 21, 429, 377, 17, hMyGraphics[ GFX_PROGRESSEMPTY ], 0, 0, SRCCOPY );
			if( uPrg1Max > 0 )
				BitBlt( hBackBuffer, 21, 409, (int)( 377.0f * uPrg1Val / uPrg1Max ), 17, hMyGraphics[ GFX_PROGRESSFULL ], 0, 0, SRCCOPY );
			if( uPrg2Max > 0 )
				BitBlt( hBackBuffer, 21, 429, (int)( 377.0f * uPrg2Val / uPrg2Max ), 17, hMyGraphics[ GFX_PROGRESSFULL ], 0, 0, SRCCOPY );

			// Draw status
			SetTextColor( hBackBuffer, 0xFFFFFE );
			DrawString( cpStatusText, 21, 392, 377, 17, DT_CENTER );
			
			// Copy back buffer
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint( hWnd, &ps );
			BitBlt( hdc, 0, 0, windowsizex, windowsizey, hBackBuffer, 0, 0, SRCCOPY );
			EndPaint( hWnd, &ps );
			return 0;
		}
		case WM_QUIT:
		{
            PostQuitMessage(0);
			return 0;
		}
	}
	return DefWindowProc( hWnd, message, wParam, lParam );
}

ULONG CheckCollision( ULONG x, ULONG y )
{
	if( x >= windowsizex-36 && y >= 0 && x < windowsizex && y < 15 )// close button
		return 2;
	else if( x >= 24 && y >= 341 && x < (24 + 84) && y < (341 + 49) )//play button
		return 3;
	else if( x >= 108 && y >= 365 && x < (108 + 115) && y < (365 + 25) )//Manual update button
		return 4;
	else if( x >= 119 && y >= 11 && x < (119 + 54) && y < (11 + 22) )//Option button
		return 5;
	else if( x >= 173 && y >= 12 && x < (173 + 50) && y < (12 + 19) )//Forum button
		return 6;
	else if( x >= 225 && y >= 12 && x < (225 + 56) && y < (12 + 19) )//Register button
		return 7;
// 	else if( x >= 282 && y >= 12 && x < (282 + 50) && y < (12 + 19) )//Donate button
// 		return 8;
	for( LONG i = 0; i < MAX_NEWSTITLES; i++ )//news items
	{
		if( x >= newsx && y >= (ULONG)( newsy + ( i * 15 ) ) && x < newsx + 393 && y < (ULONG)( newsy + ( i * 15 ) + 15 ) )
			return 100 + i;
	}
	if( x >= 0 && y >= 0 && x < windowsizex && y < windowsizey )// drag window
		return 1;
	return 0;
}

void DrawString( char* text, LONG x, LONG y, LONG width, LONG height, LONG flags )
{
	RECT myrect = { x, y, x + width, y + height };
	SetTextColor(hBackBuffer, 0xFFFFFE);
	DrawText( hBackBuffer, text, -1, &myrect, flags );
}

void ReadString( char* text, FILE* fh )
{
	WORD readlen;
	fread( &readlen, 2, 1, fh );
	fread( text, 1, readlen, fh );
	text[ readlen ] = 0;
}

int RunManualPatch( )
{
	/*unsigned char path[ 0x200 ];
	PROCESS_INFORMATION pi;
	STARTUPINFO si = { 0 };
	sprintf( (char*)path, "ManualUpdate.exe");
	CreateProcessA( 0, (char*)path, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi );*/
	MessageBox(NULL, "Manual Update is currently disabled, we will enable it as soon it is fixed", "Notice", MB_OK | MB_ICONINFORMATION);
	return 0;
}

// int lua_AddManualPatch( lua_State *L )
// {
// 	const char* path = lua_tostring( L, 1 );
// 	// Add to thing
// 	return 0;
// };
// 
// int lua_DoManualPatch( lua_State *L )
// {
// 	const char* path = lua_tostring( L, 1 );
// 
// 	unsigned char path2[ 0x200 ];
// 	PROCESS_INFORMATION pi;
// 	STARTUPINFO si = { 0 };
// 	sprintf( (char*)path2, "ExtreamIROSE.exe %s", path );
// 	CreateProcessA( 0, (char*)path2, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi );
// 	return 0;
// };
//
// void DoManualPatch( )
// {
// 	if( !dMyDownloader.DownloadFile( "mpatch.rol", "mpatch.dat" ) )
// 	{
// 		MessageBox( 0, "Could not download patch script!", "Critical Error", MB_OK );
// 		return;
// 	}
// 
// 	lua_State *L = lua_open( );
// 	luaL_openlibs(L);
// 
// 	// Adds the patch to the manual patch list
// 	lua_pushcfunction( L, lua_AddManualPatch );
// 	lua_setglobal( L, "AddManualPatch" );
// 	
// 	// Will launch the launcher with a param for the patch
// 	lua_pushcfunction( L, lua_DoManualPatch );
// 	lua_setglobal( L, "DoManualPatch" );
// }

void DoUpdate( )
{
	if( !dMyDownloader.DownloadFile( "patch.rol", "patch.dat" ) )
	{
		MessageBox( 0, "Could not download patch script!", "Critical Error", MB_OK );
		return;
	}

	lua_State *L = lua_open( );
	luaL_openlibs(L);

	lua_pushcfunction( L, lua_SetNewsItem );
	lua_setglobal( L, "SetNewsItem" );
	lua_pushcfunction( L, lua_GetPatcherVersion );
	lua_setglobal( L, "GetPatcherVersion" );
	lua_pushcfunction( L, lua_GetVfsVersion );
	lua_setglobal( L, "GetVfsVersion" );
	lua_pushcfunction( L, lua_DoPatchFile );
	lua_setglobal( L, "DoPatchFile" );
	lua_pushcfunction( L, lua_DownloadFile );
	lua_setglobal( L, "DownloadFile" );
	lua_pushcfunction( L, lua_DownloadFile2 );
	lua_setglobal( L, "DownloadFile2" );
	lua_pushcfunction( L, lua_GetFileChecksum );
	lua_setglobal( L, "GetFileChecksum" );
// 	lua_pushcfunction( L, lua_GetFileChecksum );
// 	lua_setglobal( L, "cf_GETFILECRC" );
	lua_pushcfunction( L, lua_SetRosePath );
	lua_setglobal( L, "SetRosePath" );
	lua_pushcfunction( L, lua_RunFile );
	lua_setglobal( L, "RunFile" );
	lua_pushcfunction( L, lua_DeleteFile );
	lua_setglobal( L, "DeleteFile" );
	lua_pushcfunction( L, lua_UpdatePatcher );
	lua_setglobal( L, "UpdatePatcher" );
	lua_pushcfunction( L, lua_SetVfsVersion );
	lua_setglobal( L, "SetVfsVersion" );


	if( luaL_loadfile( L, "patch.dat" ) )
	{
		MessageBox( 0, lua_tostring( L, -1 ), "Failed to load patcher file", MB_OK );
		return;
	}
	DeleteFile( "patch.dat" );

	if( lua_pcall( L, 0, 1, 0 ) )
	{
		MessageBox( 0, lua_tostring( L, -1 ), "Failed to properly start updater script!", MB_OK );
		return;
	}
	int retval = (int)lua_tonumber( L, -1 );
	if( retval == 0 ) {
		bRunning = false;
		sprintf( cpStatusText, "Patcher Updated - Restarting!" );
	} else if( retval == 1 ) {
		uPlayBtnState = 0;
		sprintf( cpStatusText, "Patching Failed!" );
	} else if( retval == 2 ) {
		uPlayBtnState = 1;
		sprintf( cpStatusText, "Done!" );
	}

	uPrg1Max = 1; uPrg1Val = 1;
	uPrg2Max = 1; uPrg2Val = 1;
	RedrawWindow( hWnd, 0, 0, RDW_INVALIDATE );
}
//#define LocalTest
//#define ServerTest
//#define Final
void LaunchRose( char* ip, int port )
{
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi;
	unsigned char myBuffer[ 0x200 ];

	
#if defined(ServerTest)
	sprintf( (char*)myBuffer, "TRose.exe @TRIGGER_SOFT@ _server 127.0.0.1 _port 29000");
#elif defined(LocalTest)
	sprintf( (char*)myBuffer, "TRose.exe @TRIGGER_SOFT@ _server 127.0.0.1 _port 29000");
#else
	sprintf( (char*)myBuffer, "TRose.exe @TRIGGER_SOFT@ _server %s _port %i", ip, port );
#endif
	if( CreateProcessA( 0, (char*)myBuffer, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi ) )
	{
		// Loaded OK
		char myDLLName[] = "AdventureRose.dll";
		DWORD dwWritten = 0;
		LPVOID myDLLSpace = VirtualAllocEx( pi.hProcess, 0, sizeof( myDLLName ), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
		WriteProcessMemory( pi.hProcess, myDLLSpace, myDLLName, sizeof( myDLLName ), &dwWritten );
		if( dwWritten != sizeof( myDLLName ) )
		{
			MessageBox( 0, "Failed to inject new game code", "WARNING", MB_ICONERROR | MB_OK );
			TerminateProcess( pi.hProcess, 0 );
		}
		else
		{
			{
				int tid = 0;
				HANDLE thread;

				CreateRemoteThread( pi.hProcess, 0, 0, (LPTHREAD_START_ROUTINE)GetProcAddress( GetModuleHandleA("kernel32.dll"), "LoadLibraryA"), myDLLSpace, 0, (LPDWORD)&tid );
				thread = OpenThread( THREAD_ALL_ACCESS, 0, tid );
				WaitForSingleObject( thread, 5000 );
				CloseHandle( thread );
			}

#if defined(ServerTest) || defined(Final)
			char myDLLName2[] = "RoseProtect.dll";
			dwWritten =0;
			myDLLSpace = VirtualAllocEx( pi.hProcess, 0, sizeof( myDLLName2 ), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
			WriteProcessMemory( pi.hProcess, myDLLSpace, myDLLName2, sizeof( myDLLName2 ), &dwWritten );
			if( dwWritten != sizeof( myDLLName2 ) )
			{
				MessageBox( 0, "Failed to inject protection code", "WARNING", MB_ICONERROR | MB_OK );
				TerminateProcess( pi.hProcess, 0 );
			}
			else
			{
				{
					int tid = 0;
					HANDLE thread;

					CreateRemoteThread( pi.hProcess, 0, 0, (LPTHREAD_START_ROUTINE)GetProcAddress( GetModuleHandleA("kernel32.dll"), "LoadLibraryA"), myDLLSpace, 0, (LPDWORD)&tid );
					thread = OpenThread( THREAD_ALL_ACCESS, 0, tid );
					WaitForSingleObject( thread, 5000 );
					CloseHandle( thread );
				}
#endif
				ResumeThread( pi.hThread );
#if defined(ServerTest) || defined(Final)
			}
#endif
		}
	} else {
		MessageBox( 0, "Could not find/load TRose.exe", "WARNING", MB_ICONERROR | MB_OK );
	}
}

int ExtractFile(char* filename)
{
	char* outputFolder = "temp\\";
	HANDLE                  hData;
	INT                     RHCode;
	INT                     PFCode;
	RARHeaderData           HeaderData;
	RAROpenArchiveDataEx    OpenArchiveData = { 0 };
	{
		OpenArchiveData.ArcName     = filename;
		//OpenArchiveData.CmtBuf    = NULL;
		//OpenArchiveData.CmtBufSize  = 0;
		OpenArchiveData.OpenMode    = RAR_OM_EXTRACT;
	}
	hData = RAROpenArchiveEx(&OpenArchiveData);
	if(OpenArchiveData.OpenResult != 0)
	{
		char buf[128];
		memset(buf, 0, sizeof(buf));
		sprintf_s(buf, "Error opening <%s>", filename);
		MessageBox(NULL, "Error!", buf, MB_OK|MB_ICONERROR);
		return -1;
	}
	RARSetPassword(hData, "8863da099df8adc830836744e19293ef");

	while((RHCode = RARReadHeader(hData, &HeaderData)) == 0)
	{
		PFCode = RARProcessFile(hData, RAR_EXTRACT, outputFolder, NULL);
		if(PFCode != 0)
		{
			char buf[128];
			memset(buf, 0, sizeof(buf));
			sprintf_s(buf, "Error extracting <%s>", filename);
			MessageBox(NULL, "Error!", buf, MB_OK|MB_ICONERROR);
			return -1;
		}
		struct stat st;
		std::string file = outputFolder;
		file += HeaderData.FileName;
		stat(file.c_str(), &st);
		if(st.st_mode & S_IFREG)
		{
			vfs->patchfiles.push_back(file);
		}
	}
	if(RHCode == ERAR_BAD_DATA)
	{
		char buf[128];
		memset(buf, 0, sizeof(buf));
		sprintf_s(buf, "Error <%s> is corrupt", filename);
		MessageBox(NULL, "Error!", buf, MB_OK|MB_ICONERROR);
		return -1;
	}
	RARCloseArchive(hData);
	DeleteFile(filename);
	return 0;
}