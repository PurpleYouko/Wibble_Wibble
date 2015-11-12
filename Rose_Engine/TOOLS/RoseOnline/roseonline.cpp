#include "stdafx.h"
#include "roseonline.h"
#include "luafunc.h"
#include "patching.h"


bool bRunning;
HINSTANCE hMyInstance;
HWND hWnd;
HDC hBackBuffer;
HDC hMyGraphics[ MAX_GRAPHICS ];
CMyDownloader dMyDownloader;
ULONG uPlayBtnState;
ULONG uExitBtnState;
ULONG uUpdateBtnState;
ULONG uOptionsBtnState;
ULONG uDonateBtnState;
ULONG uForumBtnState;
ULONG uRegisterBtnState;
ULONG uNewsBarState[ MAX_NEWSTITLES ];
ULONG uPrg1Max, uPrg1Val, uPrg2Max, uPrg2Val;
char cpStatusText[ 1024 ];
char cpNews[10][2][1024];
char cpStartStr[ 1024 ];
int cpStartPortStr;
int ifsupdateid;
BLENDFUNCTION m_bf;
//INT GetEncoderClsid(const WCHAR* format, CLSID* pClsid);  // helper function

cVFS *vfs = new cVFS();

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	hMyInstance = hInstance;

	/*if( strlen( lpCmdLine ) > 0 && lpCmdLine[0] != '/' )
	{
		if( cAFS::ManualPatchAfs( "data.idx", lpCmdLine ) == 0 )
		{
			MessageBox( 0, cAFS::GetLastError( ), "Update Error", MB_OK );
		} else {
			MessageBox( 0, "Done patching!", "Update", MB_OK );
		}
		return 0;
	}*/
	if( strcmp( lpCmdLine, "/update" ) == 0 )
	{
		Sleep(5000);
		CopyFile( "launcher.exe", "RoseOnline.exe", FALSE );
		unsigned char path[ 0x200 ];

		PROCESS_INFORMATION pi;
		STARTUPINFO si = { 0 };
		sprintf( (char*)path, "RoseOnline.exe /complete" );
		CreateProcessA( 0, (char*)path, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi );
		ExitProcess( 0 );
	}
	else if( strcmp( lpCmdLine, "/complete" ) == 0 )
	{
		DeleteFile( "launcher.exe" );
	}

	// Check for graphics config
	int TriggerDetected = GetPrivateProfileInt("TRIGGER","DETECTED", 0, "./SevenHearts.ini");
	if( TriggerDetected == 0 )
	{
		ShellExecute( NULL, "open", "TriggerDetect.exe", NULL, NULL, SW_SHOWNORMAL);
		return 0;
	}

	// General init
	strcpy( cpStartStr, "" );
	strcpy( cpStatusText, "Loading..." );
	uPlayBtnState = 0;
	for( LONG i = 0; i < MAX_NEWSTITLES; i++ )
	{
		uNewsBarState[ i ] = 0;
		strcpy( cpNews[ i ][ 0 ], "" );
		strcpy( cpNews[ i ][ 1 ], "" );
	}
	uPrg1Max = 0; uPrg1Val = 0;
	uPrg2Max = 0; uPrg2Val = 0;
	uUpdateBtnState = uRegisterBtnState = uForumBtnState = uDonateBtnState = uOptionsBtnState = uExitBtnState = 1;

	// Load graphical resources
	for( LONG i = 0; i < MAX_GRAPHICS; i++ )
	{
		hMyGraphics[ i ] = CreateCompatibleDC( GetDC( 0 ) );
		SelectObject( hMyGraphics[ i ], LoadBitmap( hMyInstance, MAKEINTRESOURCE( 101 + i ) ) );
	}

	// Initialize main window
    WNDCLASSEX wc;
    ZeroMemory( &wc, sizeof( WNDCLASSEX ) );
    wc.cbSize = sizeof( WNDCLASSEX );
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)WindowProc;
    wc.hInstance = (HINSTANCE)GetModuleHandle( NULL );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = "RoseOnline";
    RegisterClassEx( &wc );

    hWnd = CreateWindow( "RoseOnline", "RoseOnline Launcher", WS_POPUP, ( GetSystemMetrics( SM_CXSCREEN ) / 2 ) - ( windowsizex / 2 ), ( GetSystemMetrics( SM_CYSCREEN ) / 2 ) - ( windowsizey / 2 ), windowsizex, windowsizey, NULL, NULL, wc.hInstance, NULL );
	ShowWindow( hWnd, 1 );
	hBackBuffer = CreateCompatibleDC( GetDC( 0 ) );
	SelectObject( hBackBuffer, CreateCompatibleBitmap ( GetDC( 0 ), windowsizex, windowsizey ) );
	SelectObject( hBackBuffer, CreateFont( 13, 0, 0, 0, 0, false, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma" ) );
	SetBkMode( hBackBuffer, TRANSPARENT );
	typedef DWORD (WINAPI *PSLWA)(HWND, DWORD, BYTE, DWORD);
	PSLWA pSetLayeredWindowAttributes;
	HMODULE hDLL = LoadLibrary ("user32");
	SetWindowLong (hWnd, GWL_EXSTYLE, GetWindowLong	(hWnd, GWL_EXSTYLE) |0x80000/*|WS_EX_TOOLWINDOW*/ );
	pSetLayeredWindowAttributes = (PSLWA) GetProcAddress(hDLL,"SetLayeredWindowAttributes");
	//pSetLayeredWindowAttributes (hWnd, RGB(255,255,255), 255, 1|2);
	pSetLayeredWindowAttributes (hWnd, RGB(236,233,216), 255, 1|2);//for pre multiplied alpha channel bmp
	FreeLibrary(hDLL);


	m_bf.BlendOp = AC_SRC_OVER;
	m_bf.BlendFlags = 0;
	m_bf.SourceConstantAlpha = 255;
	m_bf.AlphaFormat = AC_SRC_ALPHA;

	// Initialize downloader
	if( strcmp( lpCmdLine, "/test" ) == 0 )
		dMyDownloader.SetBaseRemotePath( "http://127.0.0.1/patch/" );
	else if( strcmp( lpCmdLine, "/test2" ) == 0 )
		dMyDownloader.SetBaseRemotePath( "http://127.0.0.1/patch/" );
	else if( strcmp( lpCmdLine, "/nup" ) == 0 )
	{
		uPlayBtnState = 1;
		sprintf( cpStatusText, "Skipped!" );
	}
	else
		dMyDownloader.SetBaseRemotePath( "http://127.0.0.1/patch/" );

	// Start updating
	if(!uPlayBtnState)
		CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)&DoUpdate, 0, 0, 0 );
	
	// Enter message loop
	MSG msg;
	while( bRunning )
	{
		GetMessage( &msg, NULL, 0, 0 );
		if( msg.message == WM_QUIT )
			break;
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	DestroyWindow( hWnd );

	return 0;
};