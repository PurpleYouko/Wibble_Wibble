#include "StdAfx.h"
#include "CPunk.h"
#define isBadHandle(h) ( (h) == NULL || (h) == INVALID_HANDLE_VALUE )

LST CPunk::suspect;
char CPunk::WndText[256];

CPunk::CPunk(HWND hwnd, DWORD interval)
:m_hWnd(hwnd), m_interval(interval)
{
/*	suspect.push_back("cheat");
	suspect.push_back("macro");
	suspect.push_back("autoit");
	suspect.push_back("ollydbg");
	suspect.push_back("tsearch");
	suspect.push_back("wpe");
	suspect.push_back("serbio");
	suspect.push_back("ac tool");
	suspect.push_back("m domo");*/
	SetTimer(hwnd, IDT_PUNK, interval, (TIMERPROC) NULL );

}

CPunk::~CPunk( )

{
	suspect.clear();
}

bool CPunk::Check()
{
	EnumWindows( CPunk::KilWindow, (LPARAM) NULL );
	return true;
}

bool CPunk::KillPid( DWORD pid )
{
	HANDLE hProcess = NULL;
	hProcess = OpenProcess( PROCESS_TERMINATE, FALSE, pid );
	if ( isBadHandle( hProcess ) )
	{
		return false;
	}
	else
	{
		// kill process
		if ( !TerminateProcess( hProcess, (DWORD) -1 ) )
		{
			return false;
		}
		else
		{
			return true;
		}

		// close handle
		CloseHandle( hProcess );
	}
	return 0;
}

void CPunk::convertolower(char *txt, size_t len)
{
	size_t i = 0;

	for ( i = 0; i < len ; i ++ )
	{
		txt[i] = tolower(txt[i]);
	}
}

BOOL CALLBACK CPunk::KilWindow( HWND hwnd, LPARAM lParam )
{
	DWORD tid = 0;
	DWORD pid = 0;
	size_t i = 0;
	GetWindowText( hwnd, WndText, MAX_PATH );
	convertolower( WndText, strlen(WndText ) );

	for ( i = 0 ; i<suspect.size(); i ++ )
	{
		if ( strstr( WndText, suspect[i].c_str() ) != NULL )
		{
			tid = GetWindowThreadProcessId(hwnd, &pid );
			KillPid( pid );
		}
	}
	return TRUE;
}
